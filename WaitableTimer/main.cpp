#include <iostream>
#include <string>
#include <Windows.h>

void __stdcall OnTimer(void * Param, DWORD Low, DWORD high) {
	::wprintf_s(L"TID: %u, Ticks: %u\n", ::GetCurrentThreadId(), ::GetTickCount64());
}

int wmain(const int argc, const wchar_t* const argv[]) {

	FILETIME ftime{ 0 };
	LARGE_INTEGER interval{ 0 };
	SYSTEMTIME sysTime{ 0 };

	// Creates the waitable timer
	HANDLE hTimer = ::CreateWaitableTimer(nullptr, TRUE, nullptr);
	if (!hTimer) {
		::wprintf_s(L"[-] Failed creating waitable timer: %u", ::GetLastError());
		return 1;
	}

	// When its going to start (Change to see it working)
	sysTime.wYear = 2024;
	sysTime.wDay = 25;
	sysTime.wMonth = 12;
	sysTime.wHour = 0;
	sysTime.wMinute = 0;
	sysTime.wSecond = 0;
	sysTime.wMilliseconds = 0;

	// Translate to local time zone
	::TzSpecificLocalTimeToSystemTime(nullptr, &sysTime, &sysTime);

	// Converts to FILETIME
	::SystemTimeToFileTime(&sysTime, &ftime);

	// Setting interval
	interval.QuadPart = *reinterpret_cast<ULONGLONG*>(&ftime);

	// Set Waitable Timer config
	::SetWaitableTimer(hTimer, &interval, 0, OnTimer, nullptr, FALSE);

	// Print main thread id information
	::wprintf_s(L"Main Thread ID: %u\n", ::GetCurrentThreadId());

	// Uses alertable sleep to allow APCs execution
	while (true) {
		::wprintf_s(L"[+] Waiting. Allowing APCs to execute\n");
		::SleepEx(INFINITE, TRUE);
	}

	return 0;
}