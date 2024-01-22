#include "BasicSysInfo.h"
#include <Windows.h>



BasicSysInfo::BasicSysInfo() : 
_buildNumber(0), _minorVersion(0), _majorVersion(0), _numberOfProcessors(0),
_pageSize(0), _maxAppAddr(0), _minAppAddr(0), _processorArchitecture(L"(null)"), _username(L"(null)"),
_computerName(L"(null)"), _windowsDirectory(L"(null)"), _sysRAM(0), _localeName(L"(null)"),
_processorName(L"(null)")
{
	SYSTEM_INFO si{ 0 };
	DWORD dwBuffSize{ 256 };
	byte* KUSER_SHARED_DATA = reinterpret_cast<byte*>(0x7FFE0000);
	_username.resize(256);
	_computerName.resize(256);
	_windowsDirectory.resize(512);
	_localeName.resize(16);
	_processorName.resize(256);
	_baseBoardManu.resize(256);

	

	::GetUserDefaultLocaleName(const_cast<LPWSTR>(_localeName.data()), 16);
	::GetNativeSystemInfo(&si);
	::GetPhysicallyInstalledSystemMemory(&_sysRAM);
	
	::GetUserName(const_cast<LPWSTR>(_username.data()), &dwBuffSize);

	dwBuffSize = 256;

	::GetComputerName(const_cast<LPWSTR>(_computerName.data()), &dwBuffSize);
	::GetWindowsDirectory(const_cast<LPWSTR>(_windowsDirectory.data()), 512);

	dwBuffSize = 256;

	_buildNumber = *reinterpret_cast<ULONG*>(KUSER_SHARED_DATA + 0x0260);;
	_minorVersion = *reinterpret_cast<ULONG*>(KUSER_SHARED_DATA + 0x0270);
	_majorVersion = *reinterpret_cast<ULONG*>(KUSER_SHARED_DATA + 0x026C);
	_numberOfProcessors = si.dwNumberOfProcessors;
	_pageSize = si.dwPageSize;
	_maxAppAddr = si.lpMaximumApplicationAddress;
	_minAppAddr = si.lpMinimumApplicationAddress;
	
	switch (si.wProcessorArchitecture) {
	case PROCESSOR_ARCHITECTURE_AMD64:
		_processorArchitecture = L"x64 (AMD or Intel)";
		break;
	case PROCESSOR_ARCHITECTURE_ARM:
		_processorArchitecture = L"ARM";
		break;
	case PROCESSOR_ARCHITECTURE_ARM64:
		_processorArchitecture = L"ARM64";
		break;
	case PROCESSOR_ARCHITECTURE_IA64:
		_processorArchitecture = L"Intel Itanium-based";
		break;
	case PROCESSOR_ARCHITECTURE_INTEL:
		_processorArchitecture = L"x86";
		break;
	case PROCESSOR_ARCHITECTURE_UNKNOWN:
		_processorArchitecture = L"Unknown";
	default:
		break;
	}


	// Get screen resolution
	RECT rectDesktop;
	const HWND hDesktopWindow = ::GetDesktopWindow();
	::GetWindowRect(hDesktopWindow, &rectDesktop);

	_di.width = rectDesktop.right;
	_di.height = rectDesktop.bottom;

	// Get processor name
	::RegGetValue(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", L"ProcessorNameString", RRF_RT_REG_SZ, nullptr, reinterpret_cast<LPBYTE>(const_cast<LPWSTR>(_processorName.data())), &dwBuffSize);
	
	dwBuffSize = 256;

	// Get Motherboard Name
	::RegGetValue(HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\BIOS", L"BaseBoardManufacturer", RRF_RT_REG_SZ, nullptr, reinterpret_cast<LPBYTE>(const_cast<LPWSTR>(_baseBoardManu.data())), &dwBuffSize);
	

}