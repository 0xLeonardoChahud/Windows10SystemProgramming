#include <iostream>
#include <random>
#include <memory>
#include <Windows.h>

#define THREADS_NUMBER 4

DWORD WINAPI InitSubsystem1(LPVOID);
DWORD WINAPI InitSubsystem2(LPVOID);
DWORD WINAPI InitSubsystem3(LPVOID);
DWORD WINAPI InitSubsystem4(LPVOID);

int wmain(const int argc, const wchar_t* const argv[]) {

	SYNCHRONIZATION_BARRIER syncBarrier;
	::InitializeSynchronizationBarrier(&syncBarrier, THREADS_NUMBER, -1);

	LPTHREAD_START_ROUTINE initSubsys[] = { InitSubsystem1, InitSubsystem2, InitSubsystem3, InitSubsystem4 };

	std::wcout << L"[+] System initialization started" << std::endl;

	std::unique_ptr<HANDLE[]> hThreads{ std::make_unique<HANDLE[]>(THREADS_NUMBER) };
	uint8_t i{ 0 };
	for (LPTHREAD_START_ROUTINE func : initSubsys) {
		hThreads[i++] = ::CreateThread(nullptr, 0, func, &syncBarrier, 0, nullptr);
	}

	::WaitForMultipleObjects(THREADS_NUMBER, hThreads.get(), TRUE, INFINITE);

	std::wcout << L"[+] System initialization complete" << std::endl;




	return 0;
}

DWORD WINAPI InitSubsystem1(LPVOID lpParam) {
	const DWORD ctid{ ::GetCurrentThreadId() };
	PSYNCHRONIZATION_BARRIER ptrSyncBarrier = static_cast<PSYNCHRONIZATION_BARRIER>(lpParam);
	std::random_device rd;
	std::uniform_int_distribution<int> dist(1, 10);
	const uint16_t workDelay = dist(rd) * 1000;
	
	::Sleep(workDelay);
	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 1: Starting phase 1" << std::endl;
	// Do work
	::Sleep(workDelay);
	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 1: Phase 1 ended" << std::endl;
	
	::EnterSynchronizationBarrier(ptrSyncBarrier, SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY);

	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 1: Starting phase 2" << std::endl;
	::Sleep(1500);

	return 0;
}


DWORD WINAPI InitSubsystem2(LPVOID lpParam) {
	const DWORD ctid{ ::GetCurrentThreadId() };
	PSYNCHRONIZATION_BARRIER ptrSyncBarrier = static_cast<PSYNCHRONIZATION_BARRIER>(lpParam);
	std::random_device rd;
	std::uniform_int_distribution<int> dist(1, 10);
	const uint16_t workDelay = dist(rd) * 512;

	::Sleep(workDelay);
	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 2: Starting phase 1" << std::endl;
	// Do work
	::Sleep(workDelay);
	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 2: Phase 1 ended" << std::endl;

	::EnterSynchronizationBarrier(ptrSyncBarrier, SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY);

	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 2: Starting phase 2" << std::endl;
	::Sleep(1500);

	return 0;
}

DWORD WINAPI InitSubsystem3(LPVOID lpParam) {
	const DWORD ctid{ ::GetCurrentThreadId() };
	PSYNCHRONIZATION_BARRIER ptrSyncBarrier = static_cast<PSYNCHRONIZATION_BARRIER>(lpParam);
	std::random_device rd;
	std::uniform_int_distribution<int> dist(1, 10);
	const uint16_t workDelay = dist(rd) * 512;

	::Sleep(workDelay);
	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 3: Starting phase 1" << std::endl;
	// Do work
	::Sleep(workDelay);
	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 3: Phase 1 ended" << std::endl;

	::EnterSynchronizationBarrier(ptrSyncBarrier, SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY);

	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 3: Starting phase 2" << std::endl;
	::Sleep(1500);

	return 0;
}


DWORD WINAPI InitSubsystem4(LPVOID lpParam) {
	const DWORD ctid{ ::GetCurrentThreadId() };
	PSYNCHRONIZATION_BARRIER ptrSyncBarrier = static_cast<PSYNCHRONIZATION_BARRIER>(lpParam);
	std::random_device rd;
	std::uniform_int_distribution<int> dist(1, 10);
	const uint16_t workDelay = dist(rd) * 512;

	::Sleep(workDelay);
	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 4: Starting phase 1" << std::endl;
	// Do work
	::Sleep(workDelay);
	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 4: Phase 1 ended" << std::endl;

	::EnterSynchronizationBarrier(ptrSyncBarrier, SYNCHRONIZATION_BARRIER_FLAGS_BLOCK_ONLY);

	std::wcout << L"[ tid:" << ctid << " ] " << "Subsystem 4: Starting phase 2" << std::endl;
	::Sleep(1500);

	return 0;
}




