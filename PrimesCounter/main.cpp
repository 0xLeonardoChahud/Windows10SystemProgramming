// [ Chapter 5: Thread Basics ]
// Title: The primes counter application
// App Style: Command line
// Topics covered:
// Chapter 5: Thread creation
// Chapter 5: Waiting on waitable objects
// Chapter 5: Thread functions prototypes



#include <iostream>
#include <vector>
#include <new>
#include <memory>
#include <Windows.h>

typedef struct _INTERVAL_DATA_ {
	uint64_t from;
	uint64_t to;
	uint64_t count;
} INTERVAL_DATA, *PINTERVAL_DATA;


// Explaining why limit = sqrt(n).
// Let n be a nonprime number.
// So there is p and q such that p * q = n, with p and q belonging to the natural set.
// Now, consider the following equation: sqrt(n) * sqrt(n) = n.
// If we understand that each factor of n in this equation is a sort of "weight",
// we rapidly conclude that if we vary the first factor up
// then we have to vary the second factor down proportionally.
// Because of that, there is at least one factor lesser than or equal to the sqrt(n).
bool isPrime(_In_ const uint64_t number) {
	if (number < 2)
		return false;

	uint64_t limit{ static_cast<uint64_t>(::sqrt(number))};
	for (std::size_t i = 2; i <= limit; i++) {
		if (number % i == 0)
			return false;
	}
	return true;
}

// This is the function (code) that all threads created will run.
// First, this prototype is a must due to compatibility with the Windows API.
// Now, WINAPI may be the main doubt for the ones who are new to Windows programming.
// WINAPI is just a typedef for __stdcall which then should clarify all your doubts.
// stdcall is what we call a "calling convention" that should be interpreted as
// a set of rules that dictate how parameters should be passed to a function in a function call
// and also which function (caller or callee) should be responsible for cleaning the storage used.
// Parameters should be passed on the stack? (cdecl and stdcall) On certain registers first? (fastcall)
// It is that type of question that the calling conventions aim to answer properly.
// Finally, what we are saying with the WINAPI part is that the function CountPrimes will use the stdcall calling convention
// and we do this because it is necessary according to the microsoft documentation for the CreateThread function.
DWORD WINAPI CountPrimes(_Inout_ PVOID pParameter) {
	PINTERVAL_DATA pData{ static_cast<PINTERVAL_DATA>(pParameter) };
	DWORD count{ 0 };
	uint64_t from{ pData->from };
	uint64_t to{ pData->to };
	for (std::size_t i = from; i <= to; i++) {
		if (isPrime(i)) {
			count++;
		}
	}

	pData->count = count;
	return count;
}

int wmain(const int argc, const wchar_t* const argv[]) {

	// Usage PrimesCounter.exe <from> <to> <threads>
	if (argc < 4) {
		::wprintf(L"[ info ] Usage: PrimesCounter.exe <from> <to> <threads>\n");;
		return 1;
	}

	// perThread -> stores how many numbers each thread will check.
	// restPerThread -> additional numbers for each thread to check.
	uint64_t totalPrimes{ 0 };
	uint64_t from{ static_cast<uint64_t>(::_wtoi(argv[1])) };
	uint64_t to{ static_cast<uint64_t>(::_wtoi(argv[2])) };
	uint8_t nThreads{ static_cast<uint8_t>(::_wtoi(argv[3])) };
	uint64_t perThread{ static_cast<uint64_t>((to - from + 1) / nThreads) };
	uint64_t restPerThread{ static_cast<uint64_t>((to - from + 1) % nThreads) };
	DWORD currentThreadId{ 0 };
	ULONGLONG startTime{ 0 };
	ULONGLONG endTime{ 0 };

	// Check parameters
	if ((nThreads < 1 || nThreads > 64) || (from < 1) || (to < 1)) {
		::wprintf(L"[-] Invalid parameters specified\n");
		return 1;
	}

	// We will need one handle for each thread obviously.
	std::unique_ptr<HANDLE[]> ptrHandleList{ std::make_unique<HANDLE[]>(nThreads) };

	// For each thread, we will also need an INTERVAL_DATA structure to store how many primes were count.
	std::unique_ptr<INTERVAL_DATA[]> ptrIntervalData{ std::make_unique<INTERVAL_DATA[]>(nThreads) };

	// Get current time
	startTime = ::GetTickCount64();
	
	// We are considering closed intervals (containing the numbers from each end)
	// We also need tidx (thread index) to go through the ptrIntervalData and ptrHandleList sequentially
	::wprintf(L"\t%-25ws%-20ws\n", L"Thread Id", L"Interval");
	for (std::size_t i = from, tidx = 0; i <= to; tidx++) {
		
		// If restPerThread == 0 then each thread received an equal number of numbers to verify
		// Otherwise, we add one number sequentially until there are no more numbers to count.
		// If the number of threads (nThreads) is greater than the interval size,
		// then (restPerThread = interval size) and each thread will verify one number.
		// Also, in this last case the number of threads will be equal to the interval size due to condition ( i <= to )
		ptrIntervalData[tidx].count = 0;
		ptrIntervalData[tidx].from = i;
		if (restPerThread != 0) {
			ptrIntervalData[tidx].to = (i + perThread);
			restPerThread--;
		}
		else {
			ptrIntervalData[tidx].to = (i + perThread - 1);
		}
		
		i = ptrIntervalData[tidx].to + 1;
		
		// Creates thread
		ptrHandleList[tidx] = ::CreateThread(nullptr, 0, CountPrimes, &ptrIntervalData[tidx], 0, &currentThreadId);
		if (!ptrHandleList[tidx]) {
			::wprintf(L"[-] Failed creating thread: %u", ::GetLastError());
			break;
		}

		::wprintf(L"\t%-25u [%I64u,%I64u]\n", currentThreadId, ptrIntervalData[tidx].from, ptrIntervalData[tidx].to);
		currentThreadId = 0;

	}

	// Wait until all threads are finished
	::WaitForMultipleObjects(nThreads, ptrHandleList.get(), TRUE, INFINITE);

	// Aggregate each threads work
	for (std::size_t i = 0; i < nThreads; i++) {
		totalPrimes += ptrIntervalData[i].count;
		if (ptrHandleList[i])
			::CloseHandle(ptrHandleList[i]);
	}

	// Get current time
	endTime = ::GetTickCount64();
	
	// Display information
	::wprintf(L"\nInterval: %I64u, Threads: %lu\nTotal primes: %I64u, Elapsed: %llu msec\n", (to - from + 1), nThreads, totalPrimes, (endTime - startTime));
	
	return 0;
}
