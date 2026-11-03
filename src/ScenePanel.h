#pragma once
#include <GL/glew.h>
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <memory>
#include <set>
#include "Scene.h"

// Область 3D-сцены: wxGLCanvas с контекстом OpenGL и управлением камерой.
class ScenePanel : public wxGLCanvas {
public:
    explicit ScenePanel(wxWindow* parent);
    ~ScenePanel() override;
    void SetFog(std::unique_ptr<FogModel> fog);   // выбрать модель тумана
    void SetFogEnabled(bool enabled);             // включить / выключить туман

private:
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnMotion(wxMouseEvent& event);
    void OnWheel(wxMouseEvent& event);
    void OnEnter(wxMouseEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnCaptureLost(wxMouseCaptureLostEvent& event);

    bool InitGL();   // однократная инициализация OpenGL при первой отрисовке
    bool FailGL(const wxString& message);

    wxGLContext* context_ = nullptr;
    bool glReady_ = false;
    bool glFailed_ = false;      // не повторяем неудачную инициализацию каждый кадр
    Scene scene_;

    wxTimer timer_;              // таймер кадров
    wxStopWatch clock_;          // для расчёта времени между кадрами
    long lastTickMs_ = 0;
    std::set<int> keys_;         // нажатые сейчас клавиши

    bool dragging_ = false;      // зажата ли левая кнопка мыши
    wxPoint lastMouse_;
};
