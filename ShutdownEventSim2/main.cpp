#include <iostream>
#include <string>
#include <Windows.h>
#include <wil/resource.h>

#define PROCESS_PRINT(msg) (std::wcout << L"[ pid:" << (::GetCurrentProcessId()) << " ]" << msg << std::endl)

void __stdcall OnShutdown(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WAIT WaitObj, TP_WAIT_RESULT WaitResult);

int wmain(const int argc, const wchar_t* const argv[]) {

	if (argc != 2) {
		std::wcerr << L"Usage: ShutdownEventSim2.exe <number_of_child_processes>\n" << std::endl;
		return 0;
	}

	const uint8_t numberChildProcs{ static_cast<uint8_t>(::_wtoi(argv[1])) };
	const bool isChildProcess{ numberChildProcs == 255 };
	wil::unique_handle hShutdownEvent{ ::CreateEvent(nullptr, TRUE, FALSE, L"MyShutdownEvent2") };

	if (!isChildProcess) {
		std::wstring exeCommand{ std::wstring(argv[0]) + L" 255"};
		std::unique_ptr<HANDLE[]> hProcs{ std::make_unique<HANDLE[]>(numberChildProcs) };
		std::unique_ptr<HANDLE[]> hThrs{ std::make_unique<HANDLE[]>(numberChildProcs) };

		STARTUPINFO si{ sizeof(STARTUPINFO) };
		PROCESS_INFORMATION pi{ 0 };
		
		for (std::size_t i = 0; i < numberChildProcs; i++) {
			::CreateProcess(nullptr, const_cast<PWSTR>(exeCommand.data()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);

			hProcs[i] = pi.hProcess;
			hThrs[i] = pi.hThread;

			::ZeroMemory(&si, sizeof(STARTUPINFO));
			::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
			si.cb = sizeof(STARTUPINFO);
		}

		// User interaction
		PROCESS_PRINT(L"[+] Press [enter] to send shutdown event to child processes");
		std::getwchar();
		::SetEvent(hShutdownEvent.get());

		PROCESS_PRINT(L"[+] Press [enter] to exit");
		std::getwchar();

		// cleanup
		for (std::size_t i = 0; i < numberChildProcs; i++) {
			if (hProcs[i]) {
				::CloseHandle(hProcs[i]);
			}

			if (hThrs[i]) {
				::CloseHandle(hThrs[i]);
			}
		}

	}
	else {
		PTP_WAIT ptrWaitObj{ ::CreateThreadpoolWait(OnShutdown, nullptr, nullptr) };
		if (!ptrWaitObj) {
			std::wcerr << L"[-] Failed creating thread pool wait object." << std::endl;
			return 2;
		}


		::SetThreadpoolWait(ptrWaitObj, hShutdownEvent.get(), nullptr);
	
		// DO WORK
		::Sleep(INFINITE);
	}



	return 0;
}


void __stdcall OnShutdown(PTP_CALLBACK_INSTANCE Instance, PVOID Context, PTP_WAIT WaitObj, TP_WAIT_RESULT WaitResult) {
	// PROCESS_PRINT(L"[+] Shutting down process...");

	// SHUTDOWN ROUTINE
	::Sleep(5000);

	::CloseThreadpoolWait(WaitObj);
	exit(1);
}