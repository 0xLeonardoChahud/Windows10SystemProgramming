#include "App.h"
#include "MainFrame.h"

wxIMPLEMENT_APP(App);

bool App::OnInit() {
	MainFrame* mainFrame = new MainFrame("Queue Demo App");
	mainFrame->SetClientSize(wxSize(840, 550));
	mainFrame->SetBackgroundColour(wxColor(255, 255, 255));
	mainFrame->Center();
	mainFrame->Show();
	return true;
}