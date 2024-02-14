#include <iostream>
#include <memory>
#include <string>
#include <algorithm>
#include <Windows.h>


int wmain(const int argc, const wchar_t* const argv[]) {

	std::size_t bufferSize{ 1 << 14 };
	std::unique_ptr<wchar_t[]> buffer{std::make_unique<wchar_t[]>(bufferSize)};
	std::wstring deviceName{ L"" };
	std::wstring entry{ L"" };
	wchar_t* ptrEntry{ nullptr };

	

	for (;0 == ::QueryDosDevice(nullptr, buffer.get(), bufferSize);) {
		if (ERROR_INSUFFICIENT_BUFFER == ::GetLastError()) {
			bufferSize *= 2;
			buffer = std::make_unique<wchar_t[]>(bufferSize);
			continue;
		}
		else {
			std::wcout << L"[-] Error: " << ::GetLastError() << std::endl;
			return 0xff;
		}
	}


	if (argc == 2) {
		deviceName = std::wstring(argv[1]);
		std::transform(deviceName.begin(), deviceName.end(), deviceName.begin(), std::tolower);
	}

	ptrEntry = buffer.get();
	for (; *ptrEntry != L'\0';) {
		entry = std::wstring(ptrEntry);
		std::transform(entry.begin(), entry.end(), entry.begin(), std::tolower);
		if (deviceName.empty())
			std::wcout << entry << std::endl;
		else
			if (entry.find(deviceName) != std::string::npos)
				std::wcout << entry << std::endl;
		ptrEntry = ptrEntry + entry.size() + sizeof(byte);
	}
	




	return 0;
}