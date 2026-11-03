#include "ScenePanel.h"
#include <iostream>

namespace {
// Параметры окна рисования: цвет RGBA, двойная буферизация, буфер глубины 24 бита.
wxGLAttributes MakeDisplayAttrs() {
    wxGLAttributes attrs;
    attrs.PlatformDefaults().RGBA().DoubleBuffer().Depth(24).EndList();
    return attrs;
}
}  // namespace

ScenePanel::ScenePanel(wxWindow* parent)
    : wxGLCanvas(parent, MakeDisplayAttrs(), wxID_ANY, wxDefaultPosition, wxSize(800, 600), wxFULL_REPAINT_ON_RESIZE),
      timer_(this) {
    wxGLContextAttrs ctxAttrs;
    ctxAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 3).EndList();
    context_ = new wxGLContext(this, nullptr, &ctxAttrs);

    SetBackgroundStyle(wxBG_STYLE_PAINT);

    Bind(wxEVT_PAINT, &ScenePanel::OnPaint, this);
    Bind(wxEVT_SIZE, &ScenePanel::OnSize, this);
    Bind(wxEVT_TIMER, &ScenePanel::OnTimer, this);
    Bind(wxEVT_KEY_DOWN, &ScenePanel::OnKeyDown, this);
    Bind(wxEVT_KEY_UP, &ScenePanel::OnKeyUp, this);
    Bind(wxEVT_LEFT_DOWN, &ScenePanel::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &ScenePanel::OnLeftUp, this);
    Bind(wxEVT_MOTION, &ScenePanel::OnMotion, this);
    Bind(wxEVT_MOUSEWHEEL, &ScenePanel::OnWheel, this);
    Bind(wxEVT_ENTER_WINDOW, &ScenePanel::OnEnter, this);
    Bind(wxEVT_KILL_FOCUS, &ScenePanel::OnKillFocus, this);
    Bind(wxEVT_MOUSE_CAPTURE_LOST, &ScenePanel::OnCaptureLost, this);

    timer_.Start(16);
    clock_.Start();
}

ScenePanel::~ScenePanel() {
    timer_.Stop();
    if (context_) {
        if (glReady_) {
            SetCurrent(*context_);
            scene_.ReleaseGL();
        }
        delete context_;
    }
}

bool ScenePanel::InitGL() {
    if (glFailed_) return false;
    if (glReady_) return true;
    if (!context_ || !context_->IsOK())
        return FailGL(L"Не удалось создать контекст OpenGL 3.3. Проверьте поддержку OpenGL и драйвер видеокарты.");

    if (!SetCurrent(*context_))
        return FailGL(L"Не удалось активировать контекст OpenGL.");
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();   // загрузка функций OpenGL
    if (err != GLEW_OK) {
        std::cerr << "GLEW error: " << glewGetErrorString(err) << std::endl;
        return FailGL(L"Не удалось загрузить функции OpenGL (GLEW). Проверьте драйвер видеокарты.");
    }
    glGetError();

    if (!scene_.InitGL()) {
        scene_.ReleaseGL();
        return FailGL(L"Не удалось подготовить 3D-сцену: ошибка сборки шейдеров. Подробности — в консоли.");
    }
    glReady_ = true;
    return true;
}

bool ScenePanel::FailGL(const wxString& message) {
    glFailed_ = true;
    timer_.Stop();
    keys_.clear();
    // Показываем сообщение после завершения текущего обработчика отрисовки.
    CallAfter([this, message] {
        wxMessageBox(message, L"Ошибка 3D-визуализации", wxOK | wxICON_ERROR, this);
    });
    return false;
}

void ScenePanel::SetFog(std::unique_ptr<FogModel> fog) {
    scene_.SetFog(std::move(fog));
    Refresh(false);
}

void ScenePanel::SetFogEnabled(bool enabled) {
    scene_.SetFogEnabled(enabled);
    Refresh(false);
}

void ScenePanel::OnPaint(wxPaintEvent&) {
    wxPaintDC dc(this);
    if (!IsShownOnScreen() || !InitGL()) return;

    if (!SetCurrent(*context_)) {
        FailGL(L"Не удалось активировать контекст OpenGL при отрисовке.");
        return;
    }
    wxSize size = GetClientSize() * GetContentScaleFactor();
    scene_.Render(size.GetWidth(), size.GetHeight());
    SwapBuffers();
}

void ScenePanel::OnSize(wxSizeEvent& event) {
    Refresh(false);
    event.Skip();
}

// Вызывается примерно 60 раз в секунду: двигаем камеру по нажатым клавишам и перерисовываем кадр.
void ScenePanel::OnTimer(wxTimerEvent&) {
    long now = clock_.Time();
    float dt = (now - lastTickMs_) / 1000.0f;   // секунды с прошлого кадра
    lastTickMs_ = now;
    if (dt > 0.1f) dt = 0.1f;

    const float speed = 10.0f * dt;   // 10 единиц в секунду
    Camera& cam = scene_.GetCamera();
    if (keys_.count('W')) cam.MoveForward(speed);
    if (keys_.count('S')) cam.MoveForward(-speed);
    if (keys_.count('D')) cam.MoveRight(speed);
    if (keys_.count('A')) cam.MoveRight(-speed);
    if (keys_.count('E')) cam.MoveUp(speed);
    if (keys_.count('Q')) cam.MoveUp(-speed);

    Refresh(false);
}

void ScenePanel::OnKeyDown(wxKeyEvent& event) {
    keys_.insert(event.GetKeyCode());
    event.Skip();
}

void ScenePanel::OnKeyUp(wxKeyEvent& event) {
    keys_.erase(event.GetKeyCode());
    event.Skip();
}

void ScenePanel::OnLeftDown(wxMouseEvent& event) {
    SetFocus();
    dragging_ = true;
    lastMouse_ = event.GetPosition();
    CaptureMouse();
}

void ScenePanel::OnLeftUp(wxMouseEvent&) {
    dragging_ = false;
    if (HasCapture()) ReleaseMouse();
}

// Пока левая кнопка зажата, движение мыши поворачивает камеру.
void ScenePanel::OnMotion(wxMouseEvent& event) {
    if (!dragging_) return;
    wxPoint p = event.GetPosition();
    float dx = static_cast<float>(p.x - lastMouse_.x);
    float dy = static_cast<float>(p.y - lastMouse_.y);
    lastMouse_ = p;
    scene_.GetCamera().Rotate(dx * 0.2f, -dy * 0.2f);
}

void ScenePanel::OnWheel(wxMouseEvent& event) {
    float steps = event.GetWheelRotation() / static_cast<float>(event.GetWheelDelta());
    scene_.GetCamera().MoveForward(steps * 2.0f);
}

// Клавиши приходят окну, которое в фокусе, поэтому берём фокус при наведении мыши.
void ScenePanel::OnEnter(wxMouseEvent&) { SetFocus(); }

void ScenePanel::OnKillFocus(wxFocusEvent& event) {
    keys_.clear();
    event.Skip();
}

void ScenePanel::OnCaptureLost(wxMouseCaptureLostEvent&) { dragging_ = false; }
