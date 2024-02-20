#include <iostream>
#include <string>
#include <memory>
#include <Windows.h>


DWORD __stdcall WorkerThread(LPVOID lpParam);
VOID MessageFunc(ULONG_PTR Param);

int wmain(const int argc, const wchar_t* const argv[]) {

	HANDLE hThread{ nullptr };
	HANDLE hEvent{ nullptr };

	hEvent = ::CreateEvent(nullptr, TRUE, FALSE, nullptr);
	if (nullptr == hEvent) {
		::wprintf_s(L"[-] Failed creating event object: %u\n\n", ::GetLastError());
		return 0xff;
	}

	hThread = ::CreateThread(nullptr, 0, WorkerThread, hEvent, 0, nullptr);
	if (nullptr == hThread) {
		::wprintf_s(L"[-] Failed creating worker thread: %u\n\n", ::GetLastError());
		::CloseHandle(hEvent);
		return 0xff;
	}

	::Sleep(1000);

	::QueueUserAPC(MessageFunc, hThread, reinterpret_cast<ULONG_PTR>(nullptr));
	::QueueUserAPC(MessageFunc, hThread, reinterpret_cast<ULONG_PTR>(nullptr));

	::wprintf_s(L"[ info ] Press enter to shutdown WorkerThread\n");
	std::getwchar();

	::SetEvent(hEvent);
	::WaitForSingleObject(hEvent, INFINITE);
	::CloseHandle(hEvent);
	::CloseHandle(hThread);
	

	return 0;
}


DWORD __stdcall WorkerThread(LPVOID lpParam) {
	const DWORD ctid{ ::GetCurrentThreadId() };
	HANDLE hEvent{ static_cast<HANDLE>(lpParam) };

	::wprintf_s(L"[ tid:%u ] WorkerThread here!\n", ctid);
	
	while (true) {
		::wprintf_s(L"[ tid:%u ] Waiting in alertable state to run APCs\n", ctid);
		if (::WaitForSingleObjectEx(hEvent, INFINITE, TRUE) == WAIT_OBJECT_0) {
			break;
		}
	}

	::wprintf_s(L"[ tid:%u ] WorkerThread says bye!\n", ctid);

	return 0;
}


VOID MessageFunc(ULONG_PTR Param) {
	::wprintf_s(L"[+] Executing APC...\n");
	const DWORD ctid{ ::GetCurrentThreadId() };
	for (std::size_t i = 1; i < 6; i++) {
		::wprintf_s(L"[ tid:%u ] MessageFunc running (%I64u)...\n", ctid, i);
		::Sleep(1000);
	}

}
