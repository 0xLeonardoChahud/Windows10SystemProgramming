#pragma once
#include <wx/wx.h>
#include <wx/spinctrl.h>
#include <wx/listctrl.h>
#include "Simulation.h"

class MainFrame : public wxFrame
{
public:
	MainFrame(const wxString& title);


private:
	// Methods
	void InitCtrls(void);
	void BindCtrls(void);

	void AddItemToView(wxListView* const lv, uint64_t number, uint8_t isPrime);

private:
	// Event Handlers
	void OnExitBtnClicked(wxCommandEvent&);
	void OnStartBtnClicked(wxCommandEvent&);
	void OnStopBtnClicked(wxCommandEvent&);
	void OnProcessedListInsert(wxEvent &);
	void OnClearBtnClicked(wxCommandEvent&);

private:
	// Func Variables
	uint64_t itensProduced;
	Simulation MainSimulation;

private:
	// Widgets
	
	// Main panel
	wxPanel* mainPanel;

	// Labels
	wxStaticText* mainTitle;
	wxStaticText* consumerThreadsTitle;
	wxStaticText* producerThreadsTitle;
	
	wxStaticText* processedItensLabel;
	wxStaticText* itensToBeConsumedLabel;

	// Status labels
	wxStaticText* primesInfoLabel;
	wxStaticText* itensProcessedLabel;
	wxStaticText* numberConsumersLabel;
	wxStaticText* numberProdcuersLabel;

	// Buttons
	wxButton* startBtn;
	wxButton* stopBtn;
	wxButton* exitBtn;
	wxButton* clearBtn;

	// Spin ctrls
	wxSpinCtrl* consumerSpinCtrl;
	wxSpinCtrl* producerSpinCtrl;

	// List views
	wxListView* processedView;
	wxListView* producedView;
	
	

};

