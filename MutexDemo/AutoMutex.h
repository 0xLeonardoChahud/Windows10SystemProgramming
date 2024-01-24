#pragma once
#include <iostream>
#include <Windows.h>
class AutoMutex
{
public:
	AutoMutex(const wchar_t * const mutexName = nullptr);
	~AutoMutex();

	// Funcs
	void Acquire(DWORD dwTime = INFINITE) noexcept;
	void Release(void) noexcept;

	// Getter
	HANDLE get(void) const noexcept { return _hMutex; }
	
private:
	enum {MUTEX_NAME_LENGTH = 256};
	HANDLE _hMutex;
	wchar_t _mutexName[MUTEX_NAME_LENGTH];
};

