#include <iostream>
#include <memory>
#include <Windows.h>

// Thread function prototype
VOID __stdcall DisplayMsgThread(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_TIMER TimerObject);

int wmain(const int argc, const wchar_t* const argv[]) {
	
	const DWORD ctid{ ::GetCurrentThreadId() };
	
	SYSTEMTIME _sysTime{ 0 };
	FILETIME _ftime{ 0 };

	// Set local time
	_sysTime.wYear			= 2024;
	_sysTime.wMonth			= 2;
	_sysTime.wDay			= 8;
	_sysTime.wHour			= 20;
	_sysTime.wMinute		= 13;
	_sysTime.wSecond		= 0;
	_sysTime.wMilliseconds	= 0;

	// Translate local time to UTS
	::TzSpecificLocalTimeToSystemTime(nullptr, &_sysTime, &_sysTime);
	
	// Translate SystemTime to FileTime
	::SystemTimeToFileTime(&_sysTime, &_ftime);

	// Creates thread pool timer object with error handling
	PTP_TIMER TpTimerObj{ ::CreateThreadpoolTimer(DisplayMsgThread, nullptr, nullptr) };
	if (!TpTimerObj) {
		::wprintf_s(L"[-] Failed creating thread pool timer object.\n");
		return -1;
	}

	// Sets the timer
	::SetThreadpoolTimer(TpTimerObj, &_ftime, 1000, 0);

	// Looping in the main thread to simulate work
	while (true) {
		::wprintf_s(L"[ tid:%u ] Main thread doing some job.\n", ctid);
		::Sleep(2000);
	}
	
	return 0;
}

// DisplayMsgThread: just prints a message with its thread id
VOID __stdcall DisplayMsgThread(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_TIMER TimerObject) {
	::wprintf_s(L"[ tid:%u ] Display message thread here!\n", ::GetCurrentThreadId());
}







