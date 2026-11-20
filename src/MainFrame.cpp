#include "MainFrame.h"
#include "FogModel.h"
#include <wx/file.h>
#include <wx/filedlg.h>
#include <wx/fileconf.h>
#include <wx/wfstream.h>

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
    modelChoice_->Append(L"Экспоненциальный");
    modelChoice_->Append(L"Квадратичный экспоненциальный");
    modelChoice_->SetSelection(0);
    box->Add(modelChoice_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);

    box->Add(new wxStaticText(panel, wxID_ANY, L"Цвет тумана"), 0, wxALL, pad);
    colourPicker_ = new wxColourPickerCtrl(panel, wxID_ANY, wxColour(200, 210, 220));
    box->Add(colourPicker_, 0, wxLEFT | wxRIGHT | wxBOTTOM, pad);

    densityLabel_ = new wxStaticText(panel, wxID_ANY, wxEmptyString);
    box->Add(densityLabel_, 0, wxALL, pad);
    densitySlider_ = new wxSlider(panel, wxID_ANY, 50, 0, 200);
    box->Add(densitySlider_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, pad);
    densitySlider_->Bind(wxEVT_SLIDER, &MainFrame::OnParamChanged, this);

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
    auto* loadBtn = new wxButton(panel, wxID_ANY, L"Загрузить");
    auto* shotBtn = new wxButton(panel, wxID_ANY, L"Скриншот");

    buttons->Add(saveBtn, 1, wxRIGHT, pad);
    buttons->Add(loadBtn, 1, wxRIGHT, pad);
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
    loadBtn->Bind(wxEVT_BUTTON, &MainFrame::OnLoad, this);
    shotBtn->Bind(wxEVT_BUTTON, &MainFrame::OnScreenshot, this);
}

void MainFrame::OnParamChanged(wxCommandEvent&) { ApplyFogFromControls(); }
void MainFrame::OnColourChanged(wxColourPickerEvent&) { ApplyFogFromControls(); }

void MainFrame::ApplyFogFromControls() {
    if (endSlider_->GetValue() <= startSlider_->GetValue()) {
        endSlider_->SetValue(startSlider_->GetValue() + 1);
    }

    const wxColour c = colourPicker_->GetColour();
    const Vec3 color(c.Red() / 255.0f, c.Green() / 255.0f, c.Blue() / 255.0f);
    const int model = modelChoice_->GetSelection();
    const bool linear = model == 0;

    if (linear) {
        scenePanel_->SetFog(std::make_unique<LinearFog>(color, FogStart(), FogEnd()));
    } else if (model == 1) {
        scenePanel_->SetFog(std::make_unique<ExpFog>(color, FogDensity()));
    } else {
        scenePanel_->SetFog(std::make_unique<Exp2Fog>(color, FogDensity()));
    }

    startSlider_->Enable(linear);
    endSlider_->Enable(linear);
    startLabel_->Enable(linear);
    endLabel_->Enable(linear);
    densitySlider_->Enable(!linear);
    densityLabel_->Enable(!linear);
    scenePanel_->SetFogEnabled(fogCheck_->GetValue());

    UpdateLabels();
    UpdateStatus();
}

void MainFrame::UpdateLabels() {
    densityLabel_->SetLabel(wxString::Format(L"Плотность: %.3f", FogDensity()));
    startLabel_->SetLabel(wxString::Format(L"Начальная дистанция: %.0f", FogStart()));
    endLabel_->SetLabel(wxString::Format(L"Конечная дистанция: %.0f", FogEnd()));
}

void MainFrame::UpdateStatus() {
    wxString text = L"Модель: ";
    text += modelChoice_->GetStringSelection();

    if (modelChoice_->GetSelection() == 0) {
        text += wxString::Format(L" | дистанции: %.0f–%.0f", FogStart(), FogEnd());
    }

    text += fogCheck_->GetValue() ? L" | туман включён" : L" | туман выключен";
    SetStatusText(text);
}

void MainFrame::OnSave(wxCommandEvent&) {
    wxFileDialog dialog(this, L"Сохранить параметры тумана",
                        wxEmptyString, L"tuman.fog",
                        L"Параметры тумана (*.fog)|*.fog",
                        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() != wxID_OK) {
        return;
    }

    const wxColour color = colourPicker_->GetColour();
    const wxString data = wxString::Format(
        L"version=1\nmodel=%d\nenabled=%d\nred=%d\ngreen=%d\nblue=%d\n"
        L"density_milli=%d\nstart=%d\nend=%d\n",
        modelChoice_->GetSelection(), fogCheck_->GetValue() ? 1 : 0,
        static_cast<int>(color.Red()), static_cast<int>(color.Green()),
        static_cast<int>(color.Blue()), densitySlider_->GetValue(),
        startSlider_->GetValue(), endSlider_->GetValue());

    bool saved;
    {
        wxLogNull silence;
        wxTempFile file(dialog.GetPath());
        saved = file.IsOpened() && file.Write(data) && file.Commit();
    }

    if (!saved) {
        wxMessageBox(L"Не удалось сохранить параметры тумана.",
                     L"Ошибка сохранения", wxOK | wxICON_ERROR, this);
        return;
    }

    SetStatusText(L"Параметры тумана сохранены");
}

void MainFrame::OnLoad(wxCommandEvent&) {
    wxFileDialog dialog(this, L"Загрузить параметры тумана",
                        wxEmptyString, wxEmptyString,
                        L"Параметры тумана (*.fog)|*.fog",
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dialog.ShowModal() != wxID_OK) {
        return;
    }

    wxFileInputStream input(dialog.GetPath());
    if (!input.IsOk()) {
        wxMessageBox(L"Не удалось открыть файл.",
                     L"Ошибка загрузки", wxOK | wxICON_ERROR, this);
        return;
    }

    wxFileConfig settings(input);
    long version = 0, model = 0, enabled = 0;
    long red = 0, green = 0, blue = 0;
    long density = 0, start = 0, end = 0;

    const bool readAll = settings.Read(L"version", &version)
        && settings.Read(L"model", &model)
        && settings.Read(L"enabled", &enabled)
        && settings.Read(L"red", &red)
        && settings.Read(L"green", &green)
        && settings.Read(L"blue", &blue)
        && settings.Read(L"density_milli", &density)
        && settings.Read(L"start", &start)
        && settings.Read(L"end", &end);

    const auto inRange = [](long value, long min, long max) {
        return value >= min && value <= max;
    };

    if (!readAll || version != 1
        || !inRange(model, 0, static_cast<long>(modelChoice_->GetCount()) - 1)
        || !inRange(enabled, 0, 1)
        || !inRange(red, 0, 255) || !inRange(green, 0, 255) || !inRange(blue, 0, 255)
        || !inRange(density, densitySlider_->GetMin(), densitySlider_->GetMax())
        || !inRange(start, startSlider_->GetMin(), startSlider_->GetMax())
        || !inRange(end, endSlider_->GetMin(), endSlider_->GetMax())
        || end <= start) {
        wxMessageBox(L"Файл содержит некорректные параметры тумана.",
                     L"Ошибка загрузки", wxOK | wxICON_ERROR, this);
        return;
    }

    modelChoice_->SetSelection(static_cast<int>(model));
    fogCheck_->SetValue(enabled == 1);
    colourPicker_->SetColour(wxColour(static_cast<unsigned char>(red),
                                      static_cast<unsigned char>(green),
                                      static_cast<unsigned char>(blue)));
    densitySlider_->SetValue(static_cast<int>(density));
    startSlider_->SetValue(static_cast<int>(start));
    endSlider_->SetValue(static_cast<int>(end));

    ApplyFogFromControls();
}

void MainFrame::OnScreenshot(wxCommandEvent&) {
    wxFileDialog dialog(this, L"Сохранить скриншот сцены",
                        wxEmptyString, L"scene.png",
                        L"Изображение PNG (*.png)|*.png",
                        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() != wxID_OK) {
        return;
    }

    if (!scenePanel_->SaveScreenshot(dialog.GetPath())) {
        wxMessageBox(L"Не удалось сохранить скриншот.",
                     L"Ошибка экспорта", wxOK | wxICON_ERROR, this);
        return;
    }

    SetStatusText(L"Скриншот сохранён");
}