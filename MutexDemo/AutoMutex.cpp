#include "AutoMutex.h"

AutoMutex::AutoMutex(const wchar_t* const mutexName) {
	_hMutex = nullptr;
	::ZeroMemory(_mutexName, sizeof(wchar_t)*MUTEX_NAME_LENGTH);

	_hMutex = ::CreateMutex(nullptr, FALSE, mutexName);
	if (!_hMutex) {
		::wprintf_s(L"[-] Error creating mutex object: %u", ::GetLastError());
		return;
	}

	if (mutexName) {
		::wcscpy_s(_mutexName, MUTEX_NAME_LENGTH, _mutexName);
	}
}

AutoMutex::~AutoMutex() {
	::CloseHandle(_hMutex);
}

void AutoMutex::Acquire(DWORD dwTime) noexcept {
	::WaitForSingleObject(_hMutex, dwTime);
}

void AutoMutex::Release(void) noexcept {
	::ReleaseMutex(_hMutex);
}