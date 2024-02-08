#include <iostream>
#include <memory>
#include <Windows.h>


VOID __stdcall DisplayMsgThread(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_TIMER TimerObject);



int wmain(const int argc, const wchar_t* const argv[]) {
	
	const DWORD ctid{ ::GetCurrentThreadId() };
	SYSTEMTIME _sysTime{ 0 };
	FILETIME _ftime{ 0 };

	_sysTime.wYear			= 2024;
	_sysTime.wMonth			= 2;
	_sysTime.wDay			= 8;
	_sysTime.wHour			= 20;
	_sysTime.wMinute		= 13;
	_sysTime.wSecond		= 0;
	_sysTime.wMilliseconds	= 0;

	::TzSpecificLocalTimeToSystemTime(nullptr, &_sysTime, &_sysTime);
	::SystemTimeToFileTime(&_sysTime, &_ftime);

	PTP_TIMER TpTimerObj{ ::CreateThreadpoolTimer(DisplayMsgThread, nullptr, nullptr) };
	if (!TpTimerObj) {
		::wprintf_s(L"[-] Failed creating thread pool timer object.\n");
		return -1;
	}

	::SetThreadpoolTimer(TpTimerObj, &_ftime, 1000, 0);

	while (true) {
		::wprintf_s(L"[ tid:%u ] Main thread doing some job.\n", ctid);
		::Sleep(2000);
	}
	
	return 0;
}


VOID __stdcall DisplayMsgThread(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_TIMER TimerObject) {
	::wprintf_s(L"[ tid:%u ] Display message thread here!\n", ::GetCurrentThreadId());
}







