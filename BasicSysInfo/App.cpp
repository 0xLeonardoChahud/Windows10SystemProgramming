#include "App.h"
#include "MainFrame.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(App);

bool App::OnInit() {
	MainFrame* mf = new MainFrame("Basic System Information");
	mf->SetBackgroundColour(wxColour("white"));
	mf->Show();


	return true;
}