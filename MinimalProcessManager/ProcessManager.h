#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
#include <TlHelp32.h>
#include "PEParser.h"

class ProcessManager {
private:
	std::wstring processName;
	std::wstring imagePathName;
	DWORD processId;
	HANDLE hProcess;
	PEParser peParser;

	static void* ptrSuspendProcess;
	static void* ptrResumeProcess;
	
	void Cleanup(void);
	HANDLE getHandleFromPid(_In_ const DWORD pid);
	const std::wstring getImagePathName(_In_ const HANDLE hProcess);
	DWORD GetProcessIdByName(_In_ const std::wstring& procName) const;
	const wchar_t* getPriorityText(_In_ const DWORD priorityValue) const;


	

public:
	ProcessManager(_In_ const DWORD pid = 0);
	~ProcessManager();
	void UpdateProcess(_In_ const DWORD pid);
	void UpdateProcess(_In_ const std::wstring& procName);

	
	// Getters
	const std::wstring& getProcessName(void) const { return this->processName; }
	const std::wstring& getImagePath(void) const { return this->imagePathName; }
	DWORD getProcessId(void) const { return this->processId; }


	// Functionality
	DWORD DisplayProcessList(_In_ const bool filter = false, _In_ const std::wstring& procName = L"(none)") const;
	void DisplayProcessInfo(void) const;
	void TerminateProcess(_In_ const DWORD exitCode = 0);
	void SuspendProcess(void);
	void ResumeProcess(void);
	void SetPriorityClass(_In_ const DWORD newPriority);

	// Peripherical Functionality
	void CreateSpoofedPProcess(_In_ const std::wstring& processPath, _In_ const std::wstring& parentProcessPath);
	void CreateSpoofedPProcess(_In_ const std::wstring& processPath, _In_ const DWORD pid);
	


};
