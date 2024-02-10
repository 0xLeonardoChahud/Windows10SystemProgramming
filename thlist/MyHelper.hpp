#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <Windows.h>
#include <TlHelp32.h>

typedef struct _THREAD_INFO_ {
	DWORD Id;
	DWORD Pid;
	int Priority;
	FILETIME CreateTime;
	DWORD CPUTime;
	std::wstring processName;

} THREAD_INFO, *PTHREAD_INFO;

std::vector<THREAD_INFO> EnumThreads(const int pid);

