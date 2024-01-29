#include "MainFrame.h"
#include "BasicSysInfo.h"
#include <intrin.h>

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(400, 450)) {
	InitControls();
	BindEventHandlers();
}

void MainFrame::InitControls() {
	BasicSysInfo bsi;
	wxSize displaySize = this->GetSize();
	ULONGLONG RAM_GB = bsi.getInstalledRAM() / (1024 * 1024);
	mainPanel = new wxPanel(this);
	TitleLabel = new wxStaticText(mainPanel, wxID_ANY, "Basic System Information", wxPoint(0,20), wxSize(displaySize.GetWidth(), -1), wxALIGN_CENTER_HORIZONTAL);
	
	exitBtn = new wxButton(mainPanel, wxID_ANY, "Exit", wxPoint(displaySize.GetWidth() - 100, displaySize.GetHeight() - 80));

	processorNameLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Processor Name: %ws", bsi.getProcessorName()), wxPoint(20, 50));
	UsernameLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Username: %ws", bsi.getUsername()), wxPoint(20, 70));
	minAppAddrLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Minimum Application Address: 0x%p", bsi.getMinAppAddr()), wxPoint(20, 90));
	maxAppAddrLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Maximum Application Address: 0x%p", bsi.getMaxAppAddr()), wxPoint(20, 110));
	pageSizeLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Page size: %u KB", bsi.getPageSize()), wxPoint(20, 130));

	majorVersionLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Major Version: %u", bsi.getMajorVersion()), wxPoint(20, 150));
	minorVersionLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Minor Version: %u", bsi.getMinorVersion()), wxPoint(20, 170));
	buildNumberLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Build Number: %u", bsi.getBuildNumber()), wxPoint(20, 190));

	computerNameLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Computer Name: %ws", bsi.getComputerName()), wxPoint(20, 210));
	windowsDirectoryLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Windows Directory: %ws", bsi.getWindowsDirectory()), wxPoint(20, 230));

	numberOfProcessorsLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Number of logical processors: %u", bsi.getNumberOfProcessors()), wxPoint(20, 250));
	sysRAMLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Physical RAM: %llu KB / %llu GB", bsi.getInstalledRAM(), RAM_GB), wxPoint(20, 270));
	screenResolutionLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Screen Resolution: %llu x %llu", bsi.getDisplayInfo().width, bsi.getDisplayInfo().height), wxPoint(20, 290));
	localeNameLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Locale Name: %ws", bsi.getLocaleName()), wxPoint(20, 310));
	motherboardLabel = new wxStaticText(mainPanel, wxID_ANY, wxString::Format("Motherboard Manufacturer Name: %ws", bsi.getBaseboardManufacturer()), wxPoint(20, 330));
}

void MainFrame::BindEventHandlers() {
	exitBtn->Bind(wxEVT_BUTTON, &MainFrame::OnExitBtnClicked, this);
}




// Event handling
void MainFrame::OnExitBtnClicked(wxCommandEvent& evt) {
	this->Close();
}