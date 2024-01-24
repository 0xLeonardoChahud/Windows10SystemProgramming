#include <iostream>
#include <string>
#include <Windows.h>
#include "AutoMutex.h"

bool WriteToFile(const PCWSTR filePath, const PCWSTR data, const uint64_t bufferLength);

int wmain(const int argc, const wchar_t* const argv[]) {
	
	if (argc != 2) {
		::wprintf_s(L"Usage: MutexDemo.exe <file_path>\n");
		return 0;
	}

	const DWORD cpid{ ::GetCurrentProcessId() };
	const wchar_t* const filePath = argv[1];
	const std::wstring fileData = L"Text from process: " + std::to_wstring(cpid) + L"\n";

	AutoMutex autoMutex(L"ExampleMutex");

	::wprintf_s(L"[+] Process Id: %d. Mutex handle: 0x%x\n", cpid, ::HandleToUlong(autoMutex.get()));
	::wprintf_s(L"[+] Press [enter] to begin\n");
	::getchar();
	::wprintf_s(L"[+] Writing to file\n");
	
	for (std::size_t i = 0; i < 100; i++) {
		::Sleep(::GetTickCount64() & 0xff);
		
		autoMutex.Acquire();
		WriteToFile(filePath, fileData.data(), fileData.length() * sizeof(wchar_t));

		autoMutex.Release();
	}


	return 0;
}


bool WriteToFile(const PCWSTR filePath, const PCWSTR data, const uint64_t bufferLength) {
	DWORD bytes{ 0 };
	BOOL ret{ FALSE };
	HANDLE hFile{ ::CreateFile(filePath, GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, 0, nullptr) };
	if (INVALID_HANDLE_VALUE == hFile) {
		::wprintf_s(L"[-] Error while opening file to write: %u\n", ::GetLastError());
		return false;
	}

	::SetFilePointer(hFile, 0, nullptr, FILE_END);
	ret = ::WriteFile(hFile, data, bufferLength, &bytes, nullptr);
	::CloseHandle(hFile);

	return static_cast<bool>(ret);
}




