// TODO
#include <iostream>
#include <Windows.h>
#include "Wrappers.h"

typedef struct _THREAD_DATA_ {
	uint64_t number;
	AutoReadWriteLock asrwl;
} THREAD_DATA, *PTHREAD_DATA;


DWORD WINAPI ReaderThread(_In_opt_ LPVOID lpParam);
DWORD WINAPI WriterThread(_Inout_opt_ LPVOID lpParam);


int wmain(const int argc, const wchar_t* const argv[]) {


	// Usage: RWLocks.exe <reader_threads> <writer_threads>
	//
	if (argc < 3) {
		std::wcerr << L"Usage: RWLocks.exe <n_reader_threads> <n_writer_threads>" << std::endl;
		return 0;
	}

	uint8_t nReaderThreads{ static_cast<uint8_t>(::_wtoi(argv[1]))};
	uint8_t nWriterThreads{ static_cast<uint8_t>(::_wtoi(argv[2]))};
	
	if (nReaderThreads > 63 || nWriterThreads > 63) {
		std::wcerr << L"[-] Number of reader or writer threads exceeds maximum limit" << std::endl;
		return 1;
	}

	THREAD_DATA tdata{ 2002 };
	std::unique_ptr<HANDLE[]> hlReaderThreads{ std::make_unique<HANDLE[]>(nReaderThreads) };
	std::unique_ptr<HANDLE[]> hlWriterThreads{ std::make_unique<HANDLE[]>(nWriterThreads) };

	for (std::size_t i = 0; i < nReaderThreads; i++) {
		hlReaderThreads[i] = ::CreateThread(nullptr, 0, ReaderThread, &tdata, 0, nullptr);
		if (!hlReaderThreads[i]) {
			std::wcerr << L"[-] Error while creating reader thread: " << ::GetLastError() << std::endl;
			return 1;
		}
		::Sleep(100);
	}

	for (std::size_t i = 0; i < nWriterThreads; i++) {
		hlWriterThreads[i] = ::CreateThread(nullptr, 0, WriterThread, &tdata, 0, nullptr);
		if (!hlWriterThreads[i]) {
			std::wcerr << L"[-] Error while creating writer thread: " << ::GetLastError() << std::endl;
			return 1;
		}
	}


	::WaitForMultipleObjects(nReaderThreads, hlReaderThreads.get(), TRUE, INFINITE);


	for (std::size_t i = 0; i < nReaderThreads; i++) {
		if (hlReaderThreads[i])
			::CloseHandle(hlReaderThreads[i]);
	}

	for (std::size_t i = 0; i < nWriterThreads; i++) {
		if (hlWriterThreads[i])
			::CloseHandle(hlWriterThreads[i]);
	}




	return 0;
}




DWORD WINAPI ReaderThread(_In_opt_ LPVOID lpParam) {
	if (nullptr == lpParam)
		return 1;

	PTHREAD_DATA pdata{ static_cast<PTHREAD_DATA>(lpParam) };
	DWORD dwThreadId{ ::GetCurrentThreadId() };

	while (true) {
		pdata->asrwl.LockShared();
		std::wcout << L"[ tid:" << dwThreadId << L" ]\t Reader Thread: " << pdata->number << std::endl;
		pdata->asrwl.UnlockShared();
		::Sleep(1 * 1000);
	}
	
	return 0;
}

DWORD WINAPI WriterThread(_Inout_opt_ LPVOID lpParam) {
	if (nullptr == lpParam)
		return 1;
	
	PTHREAD_DATA pdata{ static_cast<PTHREAD_DATA>(lpParam) };
	DWORD dwThreadId{ ::GetCurrentThreadId() };

	while (true) {
		::Sleep(5 * 1000);
		pdata->asrwl.LockExclusive();
		pdata->number++;
		std::wcout << L"[ tid:" << dwThreadId << " ]\t Writer Thread. New value: " << pdata->number << L". Holding for 5 seconds..." << std::endl;
		::Sleep(5 * 1000);
		pdata->asrwl.UnlockExclusive();
	}
}

