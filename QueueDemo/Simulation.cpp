#include <random>
#include <iostream>
#include <memory>
#include "Simulation.h"

uint64_t Simulation::_nPrimes{ 0 };
uint64_t Simulation::_nProduced{ 0 };
uint64_t Simulation::_nConsumed{ 0 };
HANDLE Simulation::_hAbortEvent{ nullptr };
AutoCriticalSection Simulation::_acs;
AutoConditionVariable Simulation::_acv;
std::queue<Simulation::WORK_ITEM> Simulation::_workItems;
wxListCtrl* Simulation::_ptrConsumedView{ nullptr };
wxListCtrl* Simulation::_ptrProducedView{ nullptr };


Simulation::Simulation(const uint8_t nConsumers, const uint8_t nProducers, wxListCtrl* pview, wxListCtrl* cview)
	: _nConsumers(nConsumers), _nProducers(nProducers)

{
	_hConsumers = std::make_unique<HANDLE[]>(_nConsumers);
	_hProducers = std::make_unique<HANDLE[]>(_nProducers);
	_hAbortEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
	_ptrConsumedView = cview;
	_ptrProducedView = pview;
}

Simulation::~Simulation() {
	this->StopSimulation();
	this->Cleanup();
	::CloseHandle(_hAbortEvent);
}

void Simulation::StopSimulation(void) {
	for (std::size_t i = 0; i < _nConsumers; i++) {
		if (_hConsumers[i]) {
			::CloseHandle(_hConsumers[i]);
			_hConsumers[i] = nullptr;
		}
	}

	for (std::size_t i = 0; i < _nProducers; i++) {
		if (_hProducers[i]) {
			::CloseHandle(_hProducers[i]);
			_hProducers[i] = nullptr;
		}
	}

	_hConsumers.reset(nullptr);
	_hProducers.reset(nullptr);
	_nConsumers = 0;
	_nProducers = 0;

	// Should terminate all threads
	::SetEvent(_hAbortEvent);
}

void Simulation::Cleanup(void) {
	if (_ptrConsumedView)
		_ptrConsumedView->DeleteAllItems();
	
	if (_ptrProducedView)
		_ptrProducedView->DeleteAllItems();

	_hConsumers.reset(nullptr);
	_hProducers.reset(nullptr);

	while (!_workItems.empty()) {
		_workItems.pop();
	}
	
	_ptrConsumedView = nullptr;
	_ptrProducedView = nullptr;
	_nConsumers = 0;
	_nProducers = 0;
	_nConsumed = 0;
	_nProduced = 0;
	_nPrimes = 0;

}

bool Simulation::StartSimulation(const uint8_t nConsumers, const uint8_t nProducers, wxListCtrl* pview, wxListCtrl* cview) {

	this->Cleanup();

	_nConsumers = nConsumers;
	_nProducers = nProducers;
	_ptrProducedView = pview;
	_ptrConsumedView = cview;

	_hConsumers = std::make_unique<HANDLE[]>(_nConsumers);
	_hProducers = std::make_unique<HANDLE[]>(_nProducers);

	
	if (_nConsumers <= 0 || _nProducers <= 0 || (_nConsumers + _nProducers) >= 64) {
		return false;
	}

	if (pview == nullptr || cview == nullptr)
		return false;

	::ResetEvent(_hAbortEvent);

	// Create Threads
	for (std::size_t i = 0; i < _nConsumers; i++) {
		_hConsumers[i] = ::CreateThread(nullptr, 0, Simulation::ConsumerThread, nullptr, 0, nullptr);
		if (!_hConsumers[i]) {
			// Error
			return false;
		}
	}

	for (std::size_t i = 0; i < _nProducers; i++) {
		_hProducers[i] = ::CreateThread(nullptr, 0, Simulation::ProducerThread, nullptr, 0, nullptr);
		if (!_hProducers[i]) {
			// Error
			return false;
		}
	}

	return true;
}

DWORD WINAPI Simulation::ProducerThread(LPVOID lpParam) {
	std::random_device rd;
	std::uniform_int_distribution<unsigned long long> dist(1000, 1000000000);
	std::uniform_int_distribution<unsigned short int> delayDist(1, 5);

	if (_ptrProducedView == nullptr)
		return 0xff;

	const uint16_t thDelay{ static_cast<uint16_t>(delayDist(rd) * 256) };
	uint64_t number{ 0 };
	bool abort{ false };
	while (!abort) {
		::Sleep(thDelay);
		number = dist(rd);
		WORK_ITEM wi;
		wi.number = number;

		_acs.Enter();
		if (::WaitForSingleObject(_hAbortEvent, 0) == WAIT_OBJECT_0) {
			abort = true;
		}
		_nProduced++;
		_workItems.push(wi);
		
		// UI Handling
		int index = _ptrProducedView->GetItemCount();
		_ptrProducedView->InsertItem(index, std::to_string(_nProduced));
		_ptrProducedView->SetItem(index, 1, std::to_string(number));
		_ptrProducedView->SetItem(index, 2, wxT("Unknown"));


		_acs.Leave();
		::WakeAllConditionVariable(_acv.get());
	}

	return 0;
}

DWORD WINAPI Simulation::ConsumerThread(LPVOID lpParam) {
	std::random_device rd;
	std::uniform_int_distribution<unsigned short int> delayDist(1, 5);

	const uint16_t thDelay{ static_cast<unsigned short int>(delayDist(rd) * 256) };
	bool abort{ false };

	if (_ptrConsumedView == nullptr)
		return 0xff;

	while (!abort) {
		::Sleep(thDelay);
		_acs.Enter();
		
		if (::WaitForSingleObject(_hAbortEvent, 0) == WAIT_OBJECT_0) {
			abort = true;
			_acs.Leave();
			break;
		}


		while (_workItems.empty()) { 
			::SleepConditionVariableCS(_acv.get(), _acs.get(), INFINITE);
		}

		WORK_ITEM wi = _workItems.front();
		_workItems.pop();
		wi.isPrime = isPrime(wi.number);
		if (wi.isPrime)
			_nPrimes++;

		_nConsumed++;


		// UI Handlign
		int itemIndex = 0;
		WORK_ITEM _wi = getWorkItem(itemIndex);
		if (wi.number != _wi.number) {
			wxMessageBox(wxString::Format("%I64u : %I64u", wi.number, _wi.number));
			return 0xff;
		}

		_ptrProducedView->DeleteItem(itemIndex);

		int cindex = _ptrConsumedView->GetItemCount();
		_ptrConsumedView->InsertItem(cindex, std::to_string(_nConsumed));
		_ptrConsumedView->SetItem(cindex, 1, std::to_string(wi.number));
		_ptrConsumedView->SetItem(cindex, 2, std::to_string(wi.isPrime));
		_acs.Leave();

	}

	return 0;
}

bool Simulation::isPrime(const uint64_t number) {
	if (number == 2)
		return true;

	if (number <= 1)
		return false;

	for (std::size_t i = 2; i < ::sqrt(number); i++) {
		if (number % i == 0)
			return false;
	}

	return true;
}

Simulation::WORK_ITEM Simulation::getWorkItem(int index) {
	WORK_ITEM wi;
	wxListItem li;
	li.SetId(index);
	li.SetMask(wxLIST_MASK_TEXT);
	li.SetColumn(1);

	_ptrProducedView->GetItem(li);

	wi.number = ::_wtoi(li.GetText());
	return wi;
}

