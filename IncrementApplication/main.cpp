// [ Chapter 7: Thread Synchronization (Intra-Process) ]
// Title: Simple Increment Application
// App Style: Command line
// Topics covered:
// Chapter 7: Simple increment application (command line)
// Chapter 7: Critical Sections
// Chapter 7: Interlocked Functions
// Chapter 7: Data race example
// Chapter 7: RAII
// Remarks: You may alter program functionality by passing different parameters in the command line.


#include <iostream>
#include <Windows.h>
#include "Wrappers.h"

enum ATOMIC_OPS { NO_ATOMIC = 1, ATOM_INTERLOCKED_INC, ATOM_CRITICAL_SECTION};

// Structure that will be shared by the threads.
// AutoCriticalSection is a class that acts as a wrapper around a CRITICAL_SECTION object
// allowing it to be released when the variable acs goes out of escope.
// In this case, we dont have to worry calling DeleteCriticalSection in the wmain function.
typedef struct _INCREMENT_INFO_ {
	uint64_t total;
	const uint64_t cnt;
	const ATOMIC_OPS atomic;
	AutoCriticalSection acs;
} INCREMENT_INFO, *PINCREMENT_INFO;

// Thread Function Implementation
DWORD WINAPI IncrementThread(LPVOID lpParam) {
	PINCREMENT_INFO pinfo = static_cast<PINCREMENT_INFO>(lpParam);
	uint64_t& totalCnt = pinfo->total;

	// Check if the user specified any atomic method to be used.
	// Obviously, this code is meant just to illustrate the different
	// ways we have to achieve intra-process synchronization and
	// solve data races.
	if (pinfo->atomic == NO_ATOMIC) {
		for (std::size_t i = 0; i < pinfo->cnt; i++) {
			totalCnt++;
		}
	}
	else if (pinfo->atomic == ATOM_INTERLOCKED_INC) {
		for (std::size_t i = 0; i < pinfo->cnt; i++) {
			::InterlockedIncrement(&totalCnt);
		}
	}
	else if (pinfo->atomic == ATOM_CRITICAL_SECTION) {
		for (std::size_t i = 0; i < pinfo->cnt; i++) {
			pinfo->acs.Enter();
			totalCnt++;
			pinfo->acs.Leave();
		}
	}

	return 0;
}



int wmain(const int argc, const wchar_t* const argv[]) {

	// Usage: simpleInc.exe <n_threads> <count>
	// <n_threads>	: Number of threads.
	// <count>		: How much each thread will add to the final sum.
	// <atomic>		: Enable atomic operations on incrementing

	if (argc < 4) {
		std::wcerr << L"Usage: SimpleInc.exe <n_threads> <count> <atomic>\n\t<atomic> : 1 -> No synchronizing, 2 -> interlocked increment, 3 -> critical section\n" << std::endl;
		return 0;
	}

	
	// Capture parameters from command line and fill shared structure
	const uint8_t nThreads{ static_cast<uint8_t>(::_wtoi(argv[1])) };
	const uint64_t cnt{ static_cast<uint64_t>(::_wtoi(argv[2])) };
	const ATOMIC_OPS atomic{ static_cast<ATOMIC_OPS>(::_wtoi(argv[3])) };

	INCREMENT_INFO incInfo{ 0, cnt, atomic};
	
	// Maximum number of threads for a process is 64 (we already have one)
	if (nThreads >= 63) {
		std::wcerr << L"[-] Invalid number of threads >= 63" << std::endl;
		return 1;
	}

	// Create a list which will store all the thread handles
	std::unique_ptr<HANDLE[]> thlist = std::make_unique<HANDLE[]>(nThreads);

	// Create threads
	for (std::size_t i = 0; i < nThreads; i++) {
		thlist[i] = ::CreateThread(nullptr, 0, IncrementThread, &incInfo, 0, nullptr);
		if (!thlist[i]) {
			std::wcerr << L"[-] Error while creating thread." << std::endl;
		}
	}

	// Wait for all threads to finish their work.
	::WaitForMultipleObjects(nThreads, thlist.get(), TRUE, INFINITE);

	// Close thread handles
	for (std::size_t i = 0; i < nThreads; i++) {
		if (thlist[i])
			::CloseHandle(thlist[i]);
	}


	// Print information
	std::wcout << L"[ info ] Number of threads: " << nThreads << std::endl;
	std::wcout << L"[ info ] Count for each thread: " << cnt << std::endl;
	std::wcout << L"[ info ] Expected result (threads * count): " << nThreads * cnt << std::endl;
	std::wcout << L"[ info ] Actual result: " << incInfo.total << std::endl;
	std::wcout << L"[ info ] Is the result correct? : " << std::boolalpha << (incInfo.total == (nThreads * cnt)) << L"\n" << std::endl;


	return 0;
}





