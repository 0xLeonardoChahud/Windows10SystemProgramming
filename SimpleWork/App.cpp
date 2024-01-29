#include "App.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(App);

bool App::OnInit() {
	MainFrame* mf = new MainFrame();
	mf->SetBackgroundColour(wxColor("white"));
	mf->Show();

	return true;
}