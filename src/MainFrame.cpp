#include "MainFrame.h"
#include "FogModel.h"

MainFrame::MainFrame()
    : wxFrame(nullptr, wxID_ANY, L"Моделирование тумана", wxDefaultPosition, wxSize(1180, 720)) {
    CreateStatusBar();
    CreateControls();
    ApplyFogFromControls();
    Centre();
}

void MainFrame::CreateControls() {
    auto* root = new wxBoxSizer(wxHORIZONTAL);

    // Левая часть: 3D-сцена (растягивается на всё свободное место)
    scenePanel_ = new ScenePanel(this);
    root->Add(scenePanel_, 1, wxEXPAND);

    // Правая часть: панель управления фиксированной ширины
    auto* panel = new wxPanel(this);
    auto* box = new wxBoxSizer(wxVERTICAL);
    const int pad = 6;

    box->Add(new wxStaticText(panel, wxID_ANY, L"Модель тумана"), 0, wxALL, pad);
    modelChoice_ = new wxChoice(panel, wxID_ANY);
    modelChoice_->Append(L"Линейный");
    modelChoice_->SetSelection(0);
    box->Add(modelChoice_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);

    box->Add(new wxStaticText(panel, wxID_ANY, L"Цвет тумана"), 0, wxALL, pad);
    colourPicker_ = new wxColourPickerCtrl(panel, wxID_ANY, wxColour(200, 210, 220));
    box->Add(colourPicker_, 0, wxLEFT | wxRIGHT | wxBOTTOM, pad);

    startLabel_ = new wxStaticText(panel, wxID_ANY, wxEmptyString);
    box->Add(startLabel_, 0, wxALL, pad);
    startSlider_ = new wxSlider(panel, wxID_ANY, 5, 0, 100);
    box->Add(startSlider_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);

    endLabel_ = new wxStaticText(panel, wxID_ANY, wxEmptyString);
    box->Add(endLabel_, 0, wxALL, pad);
    endSlider_ = new wxSlider(panel, wxID_ANY, 45, 1, 150);
    box->Add(endSlider_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);

    fogCheck_ = new wxCheckBox(panel, wxID_ANY, L"Туман включён");
    fogCheck_->SetValue(true);
    box->Add(fogCheck_, 0, wxALL, pad);

    auto* buttons = new wxBoxSizer(wxHORIZONTAL);
    auto* saveBtn = new wxButton(panel, wxID_ANY, L"Сохранить");
    auto* shotBtn = new wxButton(panel, wxID_ANY, L"Скриншот");
    buttons->Add(saveBtn, 1, wxRIGHT, pad);
    buttons->Add(shotBtn, 1);
    box->Add(buttons, 0, wxEXPAND | wxALL, pad);

    box->AddStretchSpacer(1);   // пустое место, прижимает подсказку к низу
    auto* help = new wxStaticText(panel, wxID_ANY,
        L"Камера (наведите курсор на сцену):\nW/A/S/D — движение\nQ/E — вниз/вверх\nЛКМ + мышь — поворот\nКолесо — вперёд/назад");
    box->Add(help, 0, wxALL, pad);

    panel->SetSizer(box);
    panel->SetMinSize(wxSize(300, -1));
    root->Add(panel, 0, wxEXPAND);
    SetSizer(root);

    // Привязка обработчиков событий
    modelChoice_->Bind(wxEVT_CHOICE, &MainFrame::OnParamChanged, this);
    startSlider_->Bind(wxEVT_SLIDER, &MainFrame::OnParamChanged, this);
    endSlider_->Bind(wxEVT_SLIDER, &MainFrame::OnParamChanged, this);
    fogCheck_->Bind(wxEVT_CHECKBOX, &MainFrame::OnParamChanged, this);
    colourPicker_->Bind(wxEVT_COLOURPICKER_CHANGED, &MainFrame::OnColourChanged, this);
    saveBtn->Bind(wxEVT_BUTTON, &MainFrame::OnSave, this);
    shotBtn->Bind(wxEVT_BUTTON, &MainFrame::OnScreenshot, this);
}

void MainFrame::OnParamChanged(wxCommandEvent&) { ApplyFogFromControls(); }
void MainFrame::OnColourChanged(wxColourPickerEvent&) { ApplyFogFromControls(); }

void MainFrame::ApplyFogFromControls() {
    // Конечная дистанция должна быть больше начальной
    if (endSlider_->GetValue() <= startSlider_->GetValue())
        endSlider_->SetValue(startSlider_->GetValue() + 1);

    // Собираем модель тумана из значений элементов и передаём в сцену
    wxColour c = colourPicker_->GetColour();
    Vec3 color(c.Red() / 255.0f, c.Green() / 255.0f, c.Blue() / 255.0f);
    scenePanel_->SetFog(std::make_unique<LinearFog>(color, FogStart(), FogEnd()));
    scenePanel_->SetFogEnabled(fogCheck_->GetValue());

    UpdateLabels();
    UpdateStatus();
}

void MainFrame::UpdateLabels() {
    startLabel_->SetLabel(wxString::Format(L"Начальная дистанция: %.0f", FogStart()));
    endLabel_->SetLabel(wxString::Format(L"Конечная дистанция: %.0f", FogEnd()));
}

void MainFrame::UpdateStatus() {
    wxString text = wxString::Format(L"Модель: линейная | дистанции: %.0f–%.0f", FogStart(), FogEnd());
    text += fogCheck_->GetValue() ? L" | туман включён" : L" | туман выключен";
    SetStatusText(text);
}

void MainFrame::OnSave(wxCommandEvent&) {
    SetStatusText(L"Сохранение параметров будет добавлено в бета-версии");
}

void MainFrame::OnScreenshot(wxCommandEvent&) {
    SetStatusText(L"Экспорт скриншотов будет добавлен в бета-версии");
}