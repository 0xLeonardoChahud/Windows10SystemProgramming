#include <iostream>
#include <string>
#include <Windows.h>
#include <wil/resource.h>

#define PROCESS_PRINT(msg) (std::wcout << L"[ pid:" << (::GetCurrentProcessId()) << " ]" << msg << std::endl)

int wmain(const int argc, const wchar_t* const argv[]) {

	// Usage: ShutdownEventSim.exe <number_child_processes>
	if (argc < 2) {
		std::wcerr << L"Usage: ShutdownEventSim.exe <number_child_processes>" << std::endl;
		return 0;
	}

	LARGE_INTEGER li;
	::QueryPerformanceCounter(&li);
	::Sleep(li.HighPart);
	const uint8_t numberProcesses{ static_cast<uint8_t>(::_wtoi(argv[1])) };
	const bool childProcess{ numberProcesses == 255 };
	wil::unique_handle hShutdownEvt{ ::CreateEvent(nullptr, TRUE, FALSE, L"MyShutdownEvent") };

	if (!childProcess) {
		std::wstring exeName = std::wstring(argv[0]) + L" 255";
		std::unique_ptr<HANDLE[]> hProcs{ std::make_unique<HANDLE[]>(numberProcesses) };
		std::unique_ptr<HANDLE[]> hThrs{ std::make_unique<HANDLE[]>(numberProcesses) };

		PROCESS_INFORMATION pi;
		STARTUPINFO si;

		::ZeroMemory(&si, sizeof(STARTUPINFO));
		::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

		si.cb = sizeof(STARTUPINFO);

		for (std::size_t i = 0; i < numberProcesses; i++) {
			::CreateProcess(nullptr, const_cast<PWSTR>(exeName.data()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
			hProcs[i] = pi.hProcess;
			hThrs[i] = pi.hThread;

			::ZeroMemory(&si, sizeof(STARTUPINFO));
			::ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
			si.cb = sizeof(STARTUPINFO);
		}

		PROCESS_PRINT(L"[+] Press [enter] to send shutdown event to child processes");
		std::getwchar();
		::SetEvent(hShutdownEvt.get());

		PROCESS_PRINT(L"[+] Press [enter] to exit");
		std::getwchar();

		// cleanup
		for (std::size_t i = 0; i < numberProcesses; i++) {
			if (hProcs[i]) {
				::CloseHandle(hProcs[i]);
			}

			if (hThrs[i]) {
				::CloseHandle(hThrs[i]);
			}
		}
	}
	else {

		::WaitForSingleObject(hShutdownEvt.get(), INFINITE);

	}




	return 0;
}