#include <iostream>
#include <vector>
#include <new>
#include <memory>
#include <Windows.h>

typedef struct _INTERVAL_DATA_ {
	unsigned int from;
	unsigned int to;
	unsigned int count;
} INTERVAL_DATA, *PINTERVAL_DATA;


// Explaining why limit = sqrt(n).
// Let n be a nonprime number.
// So there is p and q such that p * q = n, with p and q belonging to the natural set.
// Now, consider the following equation: sqrt(n) * sqrt(n) = n.
// If we understand that each factor of n in this equation is a sort of "weight",
// we rapidly conclude that if we vary the first factor up
// then we have to vary the second factor down proportionally.
// Because of that, there is at least one factor lesser than or equal to the sqrt(n).
bool isPrime(unsigned int number) {
	if (number < 2)
		return false;

	unsigned int limit{ static_cast<unsigned int>(::sqrt(number))};
	for (unsigned int i = 2; i <= limit; i++) {
		if (number % i == 0)
			return false;
	}
	return true;
}

DWORD WINAPI CountPrimes(PVOID pParameter) {
	PINTERVAL_DATA pData{ static_cast<PINTERVAL_DATA>(pParameter) };
	unsigned int count{ 0 };
	unsigned int from{ pData->from };
	unsigned int to{ pData->to };
	for (unsigned int i = from; i <= to; i++) {
		if (isPrime(i)) {
			count++;
		}
	}

	pData->count = count;
	return count;
}

int wmain(const int argc, const wchar_t* argv[]) {

	// Usage PrimesCounter.exe <from> <to> <threads>
	if (argc < 4) {
		printf("[ info ] Usage: PrimesCounter.exe <from> <to> <threads>\n");;
		return 1;
	}

	// perThread -> stores how many numbers threads will check.
	// restPerThread -> additional numbers to check.
	unsigned int totalPrimes{ 0 };
	unsigned int from{ static_cast<unsigned int>(_wtoi(argv[1])) };
	unsigned int to{ static_cast<unsigned int>(_wtoi(argv[2])) };
	unsigned int nThreads{ static_cast<unsigned int>(_wtoi(argv[3])) };
	unsigned int perThread{ static_cast<unsigned int>((to - from + 1) / nThreads) };
	unsigned int restPerThread{ static_cast<unsigned int>((to - from + 1) % nThreads) };
	DWORD currentThreadId{ 0 };
	ULONG startTime{ 0 };
	ULONG endTime{ 0 };

	// Check parameters
	if ((nThreads < 1 || nThreads > 64) || (from < 1) || (to < 1)) {
		printf("[-] Invalid parameters specified\n");
		return 1;
	}

	// We will need one handle for each thread obviously.
	std::unique_ptr ptrHandleList{ std::make_unique<HANDLE[]>(nThreads) };

	// For each thread, we will also need an INTERVAL_DATA structure to store how many primes were count.
	std::unique_ptr ptrIntervalData{ std::make_unique<INTERVAL_DATA[]>(nThreads) };

	// Get current time
	startTime = ::GetTickCount64();
	
	// We are considering closed intervals (containing the numbers from each end)
	// We also need tidx (thread index) to go through the ptrIntervalData and ptrHandleList sequentially
	printf("\t%-25s%-20s\n", "Thread Id", "Interval");
	for (unsigned int i = from, tidx = 0; i <= to; tidx++) {
		
		// If restPerThread == 0, then each thread received an equal number of numbers to verify
		// Otherwise, we add one number sequentially until there are no more numbers to count
		// If the number of threads (nThreads) is grater than the interval size,
		// then restPerThread = interval size and each thread will verify one number.
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
			printf("[-] Failed creating thread: %u", ::GetLastError());
			break;
		}

		printf("\t%-25u [%d,%d]\n", currentThreadId, ptrIntervalData[tidx].from, ptrIntervalData[tidx].to);
		currentThreadId = 0;

	}

	// Wait until all threads are finished
	::WaitForMultipleObjects(nThreads, ptrHandleList.get(), TRUE, INFINITE);

	// Aggregate each threads work
	for (unsigned int i = 0; i < nThreads; i++) {
		totalPrimes += ptrIntervalData[i].count;
		if (ptrHandleList[i])
			::CloseHandle(ptrHandleList[i]);
	}

	// Get current time
	endTime = ::GetTickCount64();
	
	// Display information
	printf("\nInterval: %d, Threads: %d\nTotal primes: %u, Elapsed: %u msec\n", (to - from + 1), nThreads, totalPrimes, (endTime - startTime));
	
	return 0;
}
