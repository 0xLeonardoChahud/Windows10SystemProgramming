#include "MainFrame.h"
#include "Simulation.h"

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title), itensProduced(0) {
	InitCtrls();
	BindCtrls();
}


void MainFrame::InitCtrls(void) {

	wxFont* titleFont = new wxFont(20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont* normalFont = new wxFont(12, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxPoint mainTitlePos;
	wxPoint procViewPos;

	this->SetFont(normalFont->GetBaseFont());

	mainPanel = new wxPanel(this);

	mainTitle = new wxStaticText(mainPanel, wxID_ANY, wxT("Queue Demo App"), wxPoint(40, 20));
	mainTitle->SetFont(titleFont->GetBaseFont());
	mainTitlePos = mainTitle->GetPosition();


	consumerThreadsTitle = new wxStaticText(mainPanel, wxID_ANY, wxT("Consumer Threads"), wxPoint(80, 80));
	producerThreadsTitle = new wxStaticText(mainPanel, wxID_ANY, wxT("Producer Threads"), wxPoint(consumerThreadsTitle->GetPosition().x + 450, 80));

	consumerSpinCtrl = new wxSpinCtrl(mainPanel, wxID_ANY, "", wxPoint(consumerThreadsTitle->GetPosition().x + 50, consumerThreadsTitle->GetPosition().y + 35), wxDefaultSize, wxSP_ARROW_KEYS, 0, 63, 1);
	producerSpinCtrl = new wxSpinCtrl(mainPanel, wxID_ANY, "", wxPoint(producerThreadsTitle->GetPosition().x + 50, producerThreadsTitle->GetPosition().y + 35), wxDefaultSize, wxSP_ARROW_KEYS, 0, 63, 1);

	processedItensLabel = new wxStaticText(mainPanel, wxID_ANY, wxT("Processed itens"), wxPoint(mainTitle->GetPosition().x, consumerSpinCtrl->GetPosition().y + 50));
	itensToBeConsumedLabel = new wxStaticText(mainPanel, wxID_ANY, wxT("Itens to be consumed"), wxPoint(mainTitle->GetPosition().x + 400, producerSpinCtrl->GetPosition().y + 50));

	processedView = new wxListView(mainPanel, wxID_ANY, wxPoint(mainTitlePos.x, processedItensLabel->GetPosition().y + 30), wxSize(350, 180));
	producedView = new wxListView(mainPanel, wxID_ANY, wxPoint(itensToBeConsumedLabel->GetPosition().x, itensToBeConsumedLabel->GetPosition().y + 30), wxSize(350, 180));

	processedView->AppendColumn(wxT("ID"));
	processedView->AppendColumn(wxT("Number"));
	processedView->AppendColumn(wxT("Is Prime"));

	producedView->AppendColumn(wxT("ID"));
	producedView->AppendColumn(wxT("Number"));
	producedView->AppendColumn(wxT("Is Prime"));

	processedView->SetColumnWidth(0, processedView->GetSize().GetWidth() * 0.2);
	processedView->SetColumnWidth(1, processedView->GetSize().GetWidth() * 0.55);
	processedView->SetColumnWidth(2, processedView->GetSize().GetWidth() * 0.25);

	producedView->SetColumnWidth(0, producedView->GetSize().GetWidth() * 0.2);
	producedView->SetColumnWidth(1, producedView->GetSize().GetWidth() * 0.55);
	producedView->SetColumnWidth(2, producedView->GetSize().GetWidth() * 0.25);


	procViewPos = processedView->GetPosition();

	primesInfoLabel = new wxStaticText(mainPanel, wxID_ANY, wxT("Primes: 0"), wxPoint(procViewPos.x, procViewPos.y + 200));
	itensProcessedLabel = new wxStaticText(mainPanel, wxID_ANY, wxT("Itens processed: 0"), wxPoint(procViewPos.x, primesInfoLabel->GetPosition().y + 30));
	numberConsumersLabel = new wxStaticText(mainPanel, wxID_ANY, wxT("Number of Consumer Threads: 0"), wxPoint(primesInfoLabel->GetPosition().x + 200, primesInfoLabel->GetPosition().y));
	numberProdcuersLabel = new wxStaticText(mainPanel, wxID_ANY, wxT("Number of Producer Threads: 0"), wxPoint(itensProcessedLabel->GetPosition().x + 200, itensProcessedLabel->GetPosition().y));

	startBtn = new wxButton(mainPanel, wxID_ANY, wxT("Start"), wxPoint(itensProcessedLabel->GetPosition().x, itensProcessedLabel->GetPosition().y + 50));
	stopBtn = new wxButton(mainPanel, wxID_ANY, wxT("Stop"), wxPoint(startBtn->GetPosition().x + 100, startBtn->GetPosition().y));
	exitBtn = new wxButton(mainPanel, wxID_ANY, wxT("Exit"), wxPoint(715, startBtn->GetPosition().y));
	clearBtn = new wxButton(mainPanel, wxID_ANY, wxT("Clear"), wxPoint(stopBtn->GetPosition().x + 100, stopBtn->GetPosition().y));
}

void MainFrame::BindCtrls(void) {
	exitBtn->Bind(wxEVT_BUTTON, &MainFrame::OnExitBtnClicked, this);
	startBtn->Bind(wxEVT_BUTTON, &MainFrame::OnStartBtnClicked, this);
	stopBtn->Bind(wxEVT_BUTTON, &MainFrame::OnStopBtnClicked, this);
	clearBtn->Bind(wxEVT_BUTTON, &MainFrame::OnClearBtnClicked, this);
	processedView->Bind(wxEVT_LIST_INSERT_ITEM, &MainFrame::OnProcessedListInsert, this);
}

// isPrime
// 1 -> True
// 2 -> False
// 3 -> Unknown
void MainFrame::AddItemToView(wxListView* const lv, uint64_t number, uint8_t isPrime) {
	int iCount = lv->GetItemCount();
	lv->InsertItem(iCount, std::to_string(itensProduced + 1));
	lv->SetItem(iCount, 1, std::to_string(number));
	if (isPrime == 1)
		lv->SetItem(iCount, 2, "true");
	else if (isPrime == 2)
		lv->SetItem(iCount, 2, "false");
	else if (isPrime == 3)
		lv->SetItem(iCount, 2, "Unknown");
	itensProduced++;
}


// Event Handlers
void MainFrame::OnExitBtnClicked(wxCommandEvent& evt) {
	this->Close();
}

void MainFrame::OnStartBtnClicked(wxCommandEvent& evt) {
	const uint8_t nConsumers{ static_cast<uint8_t>(consumerSpinCtrl->GetValue()) };
	const uint8_t nProducers{ static_cast<uint8_t>(producerSpinCtrl->GetValue()) };

	if (nConsumers <= 0 || nProducers <= 0 || (nConsumers + nProducers) >= 64) {
		wxMessageBox(wxT("Invalid number of threads specified"), wxT("Error"), wxICON_ERROR);
		return;
	}
	
	MainSimulation.StartSimulation(nConsumers, nProducers, producedView, processedView);
	numberProdcuersLabel->SetLabelText(wxString::Format("Number of Producer Threads: %u", MainSimulation.getNumberProducers()));
	numberConsumersLabel->SetLabelText(wxString::Format("Number of Consumer Threads: %u", MainSimulation.getNumberConsumers()));
}

void MainFrame::OnStopBtnClicked(wxCommandEvent& evt) {
	MainSimulation.StopSimulation();
}

void MainFrame::OnProcessedListInsert(wxEvent& evt) {
	primesInfoLabel->SetLabelText(wxString::Format("Primes: %I64u", MainSimulation.getNumberPrimes()));
	itensProcessedLabel->SetLabelText(wxString::Format("Itens processed: %I64u", MainSimulation.getNumberProcessed()));
}

void MainFrame::OnClearBtnClicked(wxCommandEvent& evt) {
	processedView->DeleteAllItems();
	producedView->DeleteAllItems();
	consumerSpinCtrl->SetValue(1);
	producerSpinCtrl->SetValue(1);
}










