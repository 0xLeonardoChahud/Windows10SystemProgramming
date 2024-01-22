#pragma once
#include <wx/wx.h>

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);
private:

	void OnExitBtnClicked(wxCommandEvent& evt);

	void InitControls();
	void BindEventHandlers();

	wxPanel* mainPanel;
	wxButton* exitBtn;
	wxStaticText* UsernameLabel;
	wxStaticText* minAppAddrLabel;
	wxStaticText* maxAppAddrLabel;
	wxStaticText* pageSizeLabel;
	wxStaticText* buildNumberLabel;
	wxStaticText* majorVersionLabel;
	wxStaticText* minorVersionLabel;
	wxStaticText* computerNameLabel;
	wxStaticText* windowsDirectoryLabel;
	wxStaticText* numberOfProcessorsLabel;
	wxStaticText* sysRAMLabel;
	wxStaticText* screenResolutionLabel;
	wxStaticText* localeNameLabel;
	wxStaticText* processorNameLabel;
	wxStaticText* motherboardLabel;

	wxStaticText* TitleLabel;
	// wxStaticText* PermissionLabel;
};

