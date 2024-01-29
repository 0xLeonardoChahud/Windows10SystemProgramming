#pragma once
#include <queue>
#include <wx/wx.h>
#include <wx/listctrl.h>

class AutoCriticalSection {
public:
	AutoCriticalSection() {
		::ZeroMemory(&_cs, sizeof(CRITICAL_SECTION));
		::InitializeCriticalSection(&_cs);
	}

	~AutoCriticalSection() {
		::DeleteCriticalSection(&_cs);
	}

	void Enter(void) {
		::EnterCriticalSection(&_cs);
	}

	void Leave(void) {
		::LeaveCriticalSection(&_cs);
	}

	PCRITICAL_SECTION get(void) { return &_cs; }
private:
	CRITICAL_SECTION _cs;
};
class AutoConditionVariable {
public:
	AutoConditionVariable() {
		::ZeroMemory(&_cv, sizeof(CONDITION_VARIABLE));
		::InitializeConditionVariable(&_cv);
	}

	PCONDITION_VARIABLE get(void) { return &_cv; }

private:
	CONDITION_VARIABLE _cv;
};

class Simulation
{
public:
	Simulation(const uint8_t nConsumers = 0, const uint8_t nProducers = 0, wxListCtrl* pview = nullptr, wxListCtrl* cview = nullptr);
	~Simulation();

	void StopSimulation(void);

	// Getters
	uint8_t getNumberConsumers(void) { return _nConsumers; }
	uint8_t getNumberProducers(void) { return _nProducers; }
	uint64_t getNumberPrimes(void) { return _nPrimes; }
	uint64_t getNumberProcessed(void) { return _nConsumed; }

	// Setters
	void setNumberConsumers(const uint8_t nConsumers) { _nConsumers = nConsumers; }
	void setNumberProducers(const uint8_t nProducers) { _nProducers = nProducers; }

	typedef struct _WORK_ITEM_ {
		uint64_t number{ 0 };
		bool isPrime{ false };
	} WORK_ITEM, *PWORK_ITEM;

	// Init
	bool StartSimulation(const uint8_t nConsumers = 0, const uint8_t nProducers = 0, wxListCtrl* pview = nullptr, wxListCtrl* cview = nullptr);

private:
	// Methods

	void Cleanup(void);

	// Thread functions
	static DWORD WINAPI ProducerThread(LPVOID lpParam);
	static DWORD WINAPI ConsumerThread(LPVOID lpParam);
	static WORK_ITEM getWorkItem(int index);
	static bool isPrime(const uint64_t number);
private:
	uint8_t _nConsumers;
	uint8_t _nProducers;
	
	std::unique_ptr<HANDLE[]> _hConsumers;
	std::unique_ptr<HANDLE[]> _hProducers;


	// Static vars
	static std::queue<WORK_ITEM> _workItems;
	static AutoCriticalSection _acs;
	static AutoConditionVariable _acv;
	static uint64_t _nPrimes;
	static uint64_t _nProduced;
	static uint64_t _nConsumed;
	static HANDLE _hAbortEvent;
	static wxListCtrl* _ptrProducedView;
	static wxListCtrl* _ptrConsumedView;

};

