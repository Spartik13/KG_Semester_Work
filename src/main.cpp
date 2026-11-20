#include <wx/wx.h>
#include "MainFrame.h"
#include <wx/imagpng.h>

// Создаёт главное окно при запуске.
class FogApp : public wxApp {
public:
    bool OnInit() override {
        wxImage::AddHandler(new wxPNGHandler);
        auto* frame = new MainFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(FogApp);