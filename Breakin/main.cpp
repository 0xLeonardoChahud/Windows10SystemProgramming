#include <iostream>
#include <memory>
#include <Windows.h>



int wmain(const int argc, const wchar_t* const argv[]) {

	if (argc != 2) {
		std::wcout << L"Usage: Breakin.exe <pid>\n" << std::endl;
		return 0;
	}

	const DWORD pid{ static_cast<DWORD>(::_wtoi(argv[1]))};
	DWORD tid{ static_cast<DWORD>(0) };
	HANDLE hThread{ nullptr };
	HANDLE hProc{ nullptr };
	
	hProc = ::OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, pid);
	if (!hProc) {
		std::wcerr << L"[-] Failed opening process handle: " << ::GetLastError() << std::endl;
		return 1;
	}

	std::wcout << "[+] Process handle obtained." << std::endl;
	
	hThread = ::CreateRemoteThread(hProc, nullptr, 0, (LPTHREAD_START_ROUTINE)DebugBreak, nullptr, 0, &tid);
	if (!hThread) {
		std::wcerr << L"[-] Failed creating remote thread: " << ::GetLastError() << std::endl;
		::CloseHandle(hProc);
		return 1;
	}

	std::wcout << "[+] Remote thread created with id: " << tid << std::endl;

	::CloseHandle(hProc);
	::CloseHandle(hThread);
	
	return 0;
}