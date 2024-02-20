#include <iostream>
#include <string>
#include <memory>
#include <Windows.h>

struct FILE_INFORMATION : public BY_HANDLE_FILE_INFORMATION {
private:
	void display_systime(const FILETIME& ftime) const {
		SYSTEMTIME st{ 0 };
		::FileTimeToSystemTime(&ftime, &st);
		::SystemTimeToTzSpecificLocalTime(nullptr, &st, &st);
		::wprintf_s(L"%u/%u/%u, %u:%u:%u:%u\n", st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	}
public:

	FILE_INFORMATION() {

	}

	void Display(void) const {

		std::wcout << L"Creation Time: ";
		display_systime(this->ftCreationTime);

		std::wcout << L"Modified Time: ";
		display_systime(this->ftLastWriteTime);

		std::wcout << L"Access Time: ";
		display_systime(this->ftLastAccessTime);

	}
};

int wmain(const int argc, const wchar_t* const argv[]) {

	if (argc != 2) {
		std::wcout << L"aUsage: FileInfo.exe <file_path>\n" << std::endl;
		return 0;
	}

	const std::wstring filePath{ argv[1] };
	HANDLE hFile{ nullptr };
	LARGE_INTEGER fileSize{ 0 };
	FILE_INFORMATION fileInfo;

	hFile = ::CreateFile(filePath.data(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if (INVALID_HANDLE_VALUE == hFile) {
		std::wcerr << L"[-] Error while opening file: " << ::GetLastError() << std::endl;
		return 0xff;
	}

	::GetFileSizeEx(hFile, &fileSize);
	::GetFileInformationByHandle(hFile, &fileInfo);
	 
	std::wcout << L"File Size: " << fileSize.QuadPart << std::endl;
	fileInfo.Display();

	::CloseHandle(hFile);

	return 0;
}