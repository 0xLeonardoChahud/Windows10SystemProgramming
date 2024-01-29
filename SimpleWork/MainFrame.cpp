#include "MainFrame.h"
#include <TlHelp32.h>

MainFrame::MainFrame() : wxFrame(nullptr, wxID_ANY, "Simple Work", wxDefaultPosition, wxSize(600,300)) {
	InitCtrls();
	BindCtrls();
}


void MainFrame::InitCtrls() {
	// Getting client size
	const wxSize clientSize = this->GetClientSize();

	// Button config
	_mainPanel = new wxPanel(this);
	_submitBtn = new wxButton(_mainPanel, wxID_ANY, "Submit Work Item", wxPoint(20, 20));
	_submit10Btn = new wxButton(_mainPanel, wxID_ANY, "Submit 10 Work Items", wxPoint(20, 60));
	_clearBtn = new wxButton(_mainPanel, wxID_ANY, "Clear", wxPoint(20, clientSize.GetHeight() - 40));

	// Label config
	_threadsLabel = new wxStaticText(_mainPanel, wxID_ANY, "Threads: 0", wxPoint(20, 120));
	_workItemsLabel = new wxStaticText(_mainPanel, wxID_ANY, "Work Items:", wxPoint(250, 20));

	// ListView config
	_workItemsView = new wxListBox(_mainPanel, wxID_ANY, wxPoint(250, 40), wxSize(300, 200));

	_updateTimer = new wxTimer(this, wxID_ANY);
}


void MainFrame::BindCtrls() {
	_submitBtn->Bind(wxEVT_BUTTON, &MainFrame::OnSubmitWorkBtnClicked, this);
	_submit10Btn->Bind(wxEVT_BUTTON, &MainFrame::OnSubmitWork10BtnClicked, this);
	_clearBtn->Bind(wxEVT_BUTTON, &MainFrame::OnClearBtnClicked, this);
	this->Bind(wxEVT_TIMER, &MainFrame::UpdateThreadNumber, this);
	_updateTimer->Start(1000);
}

uint8_t MainFrame::GetCurrentNumberOfThreads() {
	const DWORD cpid{ ::GetCurrentProcessId() };
	PROCESSENTRY32W procEntry{ sizeof(PROCESSENTRY32W) };
	HANDLE hSnapshot{ ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) };
	if (!hSnapshot) {
		wxMessageBox("Error while getting number of threads in the current process", "Error", wxICON_ERROR);
		return -1;
	}

	if (!::Process32First(hSnapshot, &procEntry)) {
		wxMessageBox("Error while getting number of threads in the current process", "Error", wxICON_ERROR);
		::CloseHandle(hSnapshot);
		return -1;
	}

	do {
		if (procEntry.th32ProcessID == cpid) {
			return procEntry.cntThreads;
		}
	} while (::Process32Next(hSnapshot, &procEntry));

	::CloseHandle(hSnapshot);

	return -1;
}

// Event handling
void MainFrame::OnClearBtnClicked(const wxCommandEvent& evt) {
	_workItemsView->Clear();
}

void MainFrame::UpdateThreadNumber(const wxTimerEvent& evt) {
	const uint8_t numberOfThreads{ this->GetCurrentNumberOfThreads() };
	_threadsLabel->SetLabel(wxString::Format("Threads: %u", numberOfThreads));
}

void MainFrame::OnSubmitWorkBtnClicked(const wxCommandEvent& evt) {
	if (!::TrySubmitThreadpoolCallback(MainFrame::OnCallback, this, nullptr)) {
		wxMessageBox("Failed registering new thread to thread pool", "Error", wxICON_ERROR);
	}
}

void MainFrame::OnSubmitWork10BtnClicked(const wxCommandEvent& evt) {
	bool error{ false };

	for (std::size_t i = 0; i < 10; i++) {
		if (!::TrySubmitThreadpoolCallback(MainFrame::OnCallback, this, nullptr)) {
			error = true;
			break;
		}
	}

	if (error)
		wxMessageBox("Failed registering new thread to thread pool", "Error", wxICON_ERROR);
}

void NTAPI MainFrame::OnCallback(PTP_CALLBACK_INSTANCE instance, PVOID context) {
	const DWORD ctid{ ::GetCurrentThreadId() };
	MainFrame* ptrMf = static_cast<MainFrame*>(context);
	wxString enterString = wxString::Format("+ Thread %u added", ctid);
	wxString leaveString = wxString::Format("- Thread %u ended", ctid);
	ptrMf->_workItemsView->Insert(enterString, 0);

	::Sleep(10 * (::GetTickCount64() & 0xff));

	ptrMf->_workItemsView->Insert(leaveString, 0);
}

