#include <wx/wx.h>
#include "MainFrame.h"

// Создаёт главное окно при запуске.
class FogApp : public wxApp {
public:
    bool OnInit() override {
        auto* frame = new MainFrame();
        frame->Show(true);
        return true;
    }
};

wxIMPLEMENT_APP(FogApp);