#pragma once
#include "ScenePanel.h"
#include <wx/wx.h>
#include <wx/clrpicker.h>

// Главное окно: слева 3D-сцена, справа панель управления, внизу строка состояния.
class MainFrame : public wxFrame {
public:
    MainFrame();

private:
    void CreateControls();
    void OnParamChanged(wxCommandEvent& event);
    void OnColourChanged(wxColourPickerEvent& event);
    void OnSave(wxCommandEvent& event);
    void OnScreenshot(wxCommandEvent& event);

    void ApplyFogFromControls();   // реакция на изменение любого элемента панели
    void UpdateLabels();           // подписи у ползунков
    void UpdateStatus();           // строка состояния

    float FogStart() const { return static_cast<float>(startSlider_->GetValue()); }
    float FogEnd() const { return static_cast<float>(endSlider_->GetValue()); }

    ScenePanel* scenePanel_ = nullptr;
    wxChoice* modelChoice_ = nullptr;
    wxColourPickerCtrl* colourPicker_ = nullptr;
    wxSlider* startSlider_ = nullptr;
    wxSlider* endSlider_ = nullptr;
    wxCheckBox* fogCheck_ = nullptr;
    wxStaticText* startLabel_ = nullptr;
    wxStaticText* endLabel_ = nullptr;
};