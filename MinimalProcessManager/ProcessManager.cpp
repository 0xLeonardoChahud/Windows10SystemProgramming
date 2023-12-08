#include "ProcessManager.h"


typedef NTSTATUS (*ptrNtSuspendProcess)(HANDLE hProcess);
typedef NTSTATUS(*ptrNtResumeProcess)(HANDLE hProcess);

void* ProcessManager::ptrResumeProcess = nullptr;
void* ProcessManager::ptrSuspendProcess = nullptr;

ProcessManager::ProcessManager(const DWORD pid) : processId(pid), 
							   hProcess(nullptr), 
							   imagePathName(L"(none)"), 
							   processName(L"(none)") {
	
	// Set up suspend/resume function ptrs
	HMODULE hNtdll = ::GetModuleHandle(L"ntdll.dll");
	if (!hNtdll) {
		printf("[-] Failed setting up suspend/resume functionality\n");
		return;
	}

	if (ptrSuspendProcess == nullptr || ptrResumeProcess == nullptr) {
		ProcessManager::ptrSuspendProcess = ::GetProcAddress(hNtdll, "NtSuspendProcess");
		ProcessManager::ptrResumeProcess = ::GetProcAddress(hNtdll, "NtResumeProcess");
	}

	if (pid == 0)
		return;

	this->hProcess = this->getHandleFromPid(pid);
	this->imagePathName = this->getImagePathName(this->hProcess);
	this->processName.assign(imagePathName.substr(imagePathName.find_last_of('\\') + 1));

}


ProcessManager::~ProcessManager() {
	this->Cleanup();
}


void ProcessManager::UpdateProcess(const std::wstring& procName) {
	// Just a wrapper
	this->UpdateProcess(this->GetProcessIdByName(procName));
}


void ProcessManager::UpdateProcess(const DWORD pid) {
	this->Cleanup();
	this->processId = pid;
	this->hProcess = this->getHandleFromPid(pid);
	this->imagePathName = this->getImagePathName(this->hProcess);
	this->processName.assign(imagePathName.substr(imagePathName.find_last_of('\\') + 1));
}


HANDLE ProcessManager::getHandleFromPid(const DWORD pid) {
	HANDLE hTemp = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hTemp) {
		printf("[-] Failed acquiring handle to process\n");
		return nullptr;
	}
	return hTemp;
}


const std::wstring ProcessManager::getImagePathName(const HANDLE _hProcess) {
	DWORD pathSize{ MAX_PATH };
	std::wstring _imagePathName;
	_imagePathName.resize(pathSize + 1);

	if (!::QueryFullProcessImageName(_hProcess, 0, const_cast<PWSTR>(_imagePathName.data()), &pathSize)) {
		printf("[-] Failed acquiring process image\n");
		::CloseHandle(_hProcess);
		return L"(none)";
	}

	return _imagePathName;
}


void ProcessManager::Cleanup(void) {
	this->processId = 0;
	if (this->hProcess)
		::CloseHandle(this->hProcess);
	this->hProcess = nullptr;
	this->imagePathName = L"(none)";
	this->processName = L"(none)";
}

// Functionality
DWORD ProcessManager::DisplayProcessList(const bool filter, const std::wstring& targetProcName) const {
	PROCESS_INFORMATION procInfo{ 0 };
	PROCESSENTRY32W procEntry{ 0 };
	HANDLE hSnapshot{ nullptr };

	::ZeroMemory(&procInfo, sizeof(PROCESS_INFORMATION));
	::ZeroMemory(&procEntry, sizeof(PROCESSENTRY32W));
	procEntry.dwSize = sizeof(PROCESSENTRY32W);


	hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnapshot) {
		printf("[-] Failed acquiring snapshot for process list\n");
		return 0;
	}

	::Process32First(hSnapshot, &procEntry);
	
	if (!filter)
		printf("\t%-20s%-20s\n", "Process Id", "Image Name");
	
	do {
		if (!filter)
			printf("\t%-20u%-20ws\n", procEntry.th32ProcessID, procEntry.szExeFile);
		if (filter && targetProcName.compare(L"(none)")) {
			if (!targetProcName.compare(procEntry.szExeFile)) {
				::CloseHandle(hSnapshot);
				return procEntry.th32ProcessID;
			}
		}

	} while (::Process32Next(hSnapshot, &procEntry));

	::CloseHandle(hSnapshot);

	return 0;

}


void ProcessManager::DisplayProcessInfo(void) const {
	printf("- - - - - PROCESS INFORMATION - - - - -\n");
	printf("Process Id: %u\nProcess Name: %ws\nImage Full Path: %ws\n\n", this->processId, this->processName.data(), this->imagePathName.data());
	printf("- - - - - - - - - - - - - - - - - - - -");
}


DWORD ProcessManager::GetProcessIdByName(const std::wstring& procName) {
	return this->DisplayProcessList(true, procName);
}


void ProcessManager::TerminateProcess(DWORD ExitCode) {
	::TerminateProcess(this->hProcess, ExitCode);
	this->Cleanup();
}


void ProcessManager::SuspendProcess(void) {
	if (this->hProcess)
		static_cast<ptrNtSuspendProcess>(ProcessManager::ptrSuspendProcess)(this->hProcess);
}


void ProcessManager::ResumeProcess(void) {
	if (this->hProcess)
		static_cast<ptrNtResumeProcess>(ProcessManager::ptrResumeProcess)(this->hProcess);
}
