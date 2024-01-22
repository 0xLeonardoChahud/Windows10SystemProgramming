#pragma once
#include <iostream>
#include <string>
#include <Windows.h>

class BasicSysInfo
{
public:
	BasicSysInfo();

	typedef struct _DISPLAY_INFO_ {
		uint64_t width;
		uint64_t height;
	} DISPLAY_INFO, *PDISPLAY_INFO;

	const std::wstring& getUsername(void) const { return _username; }
	const std::wstring& getProcessorArchitecture(void) const { return _processorArchitecture; }
	const std::wstring& getComputerName(void) const { return _computerName; }
	const std::wstring& getWindowsDirectory(void) const { return _windowsDirectory; }
	const std::wstring& getLocaleName(void) const { return _localeName; }
	const std::wstring& getProcessorName(void) const { return _processorName; }
	const std::wstring& getBaseboardManufacturer(void) const { return _baseBoardManu; }
	DWORD getNumberOfProcessors(void) const { return _numberOfProcessors; }
	DWORD getPageSize(void) const { return _pageSize; }
	DWORD getMajorVersion(void) const { return _majorVersion; }
	DWORD getMinorVersion(void) const { return _minorVersion; }
	DWORD getBuildNumber(void) const { return _buildNumber; }
	LPVOID getMaxAppAddr(void) const { return _maxAppAddr; }
	LPVOID getMinAppAddr(void) const { return _minAppAddr; }
	ULONGLONG getInstalledRAM(void) const { return _sysRAM; }
	DISPLAY_INFO getDisplayInfo(void) const { return _di; }

private:
	DWORD _numberOfProcessors;
	DWORD _pageSize;
	DWORD _buildNumber;
	DWORD _minorVersion;
	DWORD _majorVersion;
	LPVOID _maxAppAddr;
	LPVOID _minAppAddr;
	ULONGLONG _sysRAM;
	std::wstring _processorArchitecture;
	std::wstring _username;
	std::wstring _computerName;
	std::wstring _windowsDirectory;
	std::wstring _localeName;
	std::wstring _processorName;
	std::wstring _baseBoardManu;
	DISPLAY_INFO _di;
};

