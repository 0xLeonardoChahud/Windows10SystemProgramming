#pragma once
#include <wx/wx.h>
#include <wx/timer.h>

class MainFrame : public wxFrame
{
public:
	MainFrame();

private:
	// Functions
	void InitCtrls();
	void BindCtrls();
	uint8_t GetCurrentNumberOfThreads();

	// CALLBACK
	static void NTAPI OnCallback(PTP_CALLBACK_INSTANCE instance, PVOID context);

	// Event handling
		// Button Handling
	void OnSubmitWorkBtnClicked(const wxCommandEvent&);
	void OnSubmitWork10BtnClicked(const wxCommandEvent&);
	void OnClearBtnClicked(const wxCommandEvent&);
		
		// List View handling
	
		
		// Update thread handling
	void UpdateThreadNumber(const wxTimerEvent&);
		
private:
	// Controls
	wxPanel* _mainPanel;

	wxButton* _submitBtn;
	wxButton* _submit10Btn;
	wxButton* _clearBtn;
	
	wxStaticText* _threadsLabel;
	wxStaticText* _workItemsLabel;

	wxListBox* _workItemsView;

	wxTimer* _updateTimer;
};

