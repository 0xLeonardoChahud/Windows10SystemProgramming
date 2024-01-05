#include "ProcessManager.h"


typedef NTSTATUS (*ptrNtSuspendProcess)(HANDLE hProcess);
typedef NTSTATUS(*ptrNtResumeProcess)(HANDLE hProcess);

void* ProcessManager::ptrResumeProcess = nullptr;
void* ProcessManager::ptrSuspendProcess = nullptr;

// (1). Because we use not fully documented functions such as SuspendProcess and ResumeProcess
// I have implemented a setup funcionality in the begining of the constructor.
// Basically, we get those functions from the ntdll.dll module which is loaded by default
// in every windows executable. Because of that, we can obtain their addresses with GetProcAddres.
// Of course, we need to provide proper prototyping so we can use them in code.

// (2). Here, I try, if a process id has been provided, to acquire some process information.
// The MOST IMPORTANT member variable here is "hProcess" because it is a handle to the process
// obtained through the specified process id and it is used basically for EVERY windows api process
// related functions.
// We also try to get the full path of the executable used to create the process specified by the user.
// Finally, we trunk imagePathName so we can store just the executable name, without its full path.
ProcessManager::ProcessManager(_In_ const DWORD pid) : processId(pid), 
							   hProcess(nullptr), 
							   imagePathName(L"(none)"), 
							   processName(L"(none)"), peParser(imagePathName) {
	
	// (1)
	if (ptrSuspendProcess == nullptr || ptrResumeProcess == nullptr) {
		HMODULE hNtdll = ::GetModuleHandle(L"ntdll.dll");
		if (!hNtdll) {
			::wprintf(L"[-] Failed setting up suspend/resume functionality\n");
			return;
		}

		ProcessManager::ptrSuspendProcess = ::GetProcAddress(hNtdll, "NtSuspendProcess");
		ProcessManager::ptrResumeProcess = ::GetProcAddress(hNtdll, "NtResumeProcess");
	}

	// (2)
	if (pid == 0)
		return;

	this->hProcess = this->getHandleFromPid(pid);
	this->imagePathName = this->getImagePathName(this->hProcess);
	this->processName.assign(imagePathName.substr(imagePathName.find_last_of('\\') + 1));

}

// (1). The destructor of the class uses a private Cleanup function to guarantee that
// it frees every resource allocated properly
ProcessManager::~ProcessManager() {
	// (1)
	this->Cleanup();
}

// (1). This function just exists to enable the programmer who is using our class
// to update the current process based on a process name instead of a process id
// giving the ProcessManager class more versatility.
void ProcessManager::UpdateProcess(_In_ const std::wstring& procName) {
	// (1)
	this->UpdateProcess(this->GetProcessIdByName(procName));
}


// (1). First, we call the cleanup function to release every resource previously allocated.
// So we can properly update the current process information without having memory leaks and
// memory related bugs in general.
// (2). We update each member variable and do some error checking.
// (3). We also reload the PEParser class instance so it refers to the executable currently
// specified.
void ProcessManager::UpdateProcess(_In_ const DWORD pid) {
	// (1)
	this->Cleanup();
	
	// (2)
	this->processId = pid;
	this->hProcess = this->getHandleFromPid(pid);
	if (!this->hProcess)
		return;
	this->imagePathName = this->getImagePathName(this->hProcess);
	if (!this->imagePathName.compare(L"(none)"))
		return;
	this->processName.assign(imagePathName.substr(imagePathName.find_last_of('\\') + 1));
	
	// (3)
	this->peParser.Reload(this->imagePathName);
}

// (1). Function that returns a handle to a process object
// using the process identifier (pid).
HANDLE ProcessManager::getHandleFromPid(_In_ const DWORD pid) {
	// (1)
	HANDLE hTemp = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (!hTemp) {
		::wprintf(L"[-] Failed acquiring handle to process\n");
		return nullptr;
	}
	return hTemp;
}

// (1). We obtain the full path of the executable file associated with the
// loaded process image.
const std::wstring ProcessManager::getImagePathName(_In_ const HANDLE _hProcess) {
	// (1)
	DWORD pathSize{ MAX_PATH };
	std::wstring _imagePathName;
	_imagePathName.resize(static_cast<std::size_t>(pathSize) + 1);

	if (!::QueryFullProcessImageName(_hProcess, 0, const_cast<PWSTR>(_imagePathName.data()), &pathSize)) {
		::wprintf(L"[-] Failed acquiring process image\n");
		::CloseHandle(_hProcess);
		return L"(none)";
	}

	return _imagePathName;
}

// (1). We close the process handle IF IT IS OPEN.
// We set variables to zero, nullptr or "(none)" depending on their types
// We also call a cleaning function from the PEParser class
void ProcessManager::Cleanup(void) {
	// (1)
	this->processId = 0;
	if (this->hProcess)
		::CloseHandle(this->hProcess);
	this->hProcess = nullptr;
	this->imagePathName = L"(none)";
	this->processName = L"(none)";
	this->peParser.clean();
}

// (1). This is probably the most important function for the MinimalProcessManager solution.
// I first start initializing some variables according to the microsoft documentation.
// Here, we are using th32help.h library to list all existing process on the system.
// This function can be used for the following purposes: display process list and
// search for a particular process (based on its name) returing its PID.
// (2). Stores a handle to an object that represents a snapshot on the system.
// Also, the program does some error handling.
// (3). Finally, all the logic is implemented in this third section. The function receives a boolean variable
// that tells if the function is to be used as a filter or should simply list all processes.
// In case the filter is off, we just print all the process information in a formatted way.
// In case the filter is on AND the user has specified a valid process name (we never know),
// then we compare with the current process name being displayed and, if they match, we return
// the process id of that particular process beign searched.
DWORD ProcessManager::DisplayProcessList(_In_ const bool filter, _In_ const std::wstring& targetProcName) const {
	// (1)
	PROCESS_INFORMATION procInfo{ 0 };
	PROCESSENTRY32W procEntry{ 0 };
	HANDLE hSnapshot{ nullptr };

	::ZeroMemory(&procInfo, sizeof(PROCESS_INFORMATION));
	::ZeroMemory(&procEntry, sizeof(PROCESSENTRY32W));
	procEntry.dwSize = sizeof(PROCESSENTRY32W);

	// (2)
	hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (!hSnapshot) {
		::wprintf(L"[-] Failed acquiring snapshot for process list\n");
		return 0;
	}

	// (3)
	::Process32First(hSnapshot, &procEntry);
	
	if (!filter)
		::wprintf(L"\t%-20ws%-20ws\n", L"Process Id", L"Image Name");
	
	do {
		if (!filter)
			::wprintf(L"\t%-20u%-20ws\n", procEntry.th32ProcessID, procEntry.szExeFile);
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

// (1). We display some process information. The use of all the windows API functions used here
// should be very straightforward to learn with the microsoft documentation.
void ProcessManager::DisplayProcessInfo(void) const {
	// (1)
	std::wstring priorityClass{ L"(none)"};
	DWORD handleCount{ 0 };
	BOOL priorityBoost{ FALSE };
	bool is64bit{ this->peParser.is64bit() };
	SIZE_T minWorkSize{ 0 };
	SIZE_T maxWorkSize{ 0 };

	if (this->hProcess) {
		::GetProcessPriorityBoost(this->hProcess, &priorityBoost);
		::GetProcessHandleCount(this->hProcess, &handleCount);
		::GetProcessWorkingSetSize(this->hProcess, &minWorkSize, &maxWorkSize);
		priorityClass = this->getPriorityText(::GetPriorityClass(this->hProcess));
	}

	::wprintf(L"- - - - - PROCESS INFORMATION - - - - -\n");
	::wprintf(L"Process Id: %u\nProcess Name: %ws\nImage Full Path: %ws\nPriority Class: %ws\nHandle Count: %u\nMinimum Working Set Size: %I64u\nMaximum Working Set Size: %I64u\n64-bit: %u\n", 
			this->processId, this->processName.data(), this->imagePathName.data(), priorityClass.data(), handleCount, minWorkSize, maxWorkSize, is64bit);
	::wprintf(L"- - - - - - - - - - - - - - - - - - - -");
}

// (1). This is a wrapper function that calls DisplayProcessList with the filtering
// mode enabled. It allows using this member function to obtain process id based
// only on a process name.
DWORD ProcessManager::GetProcessIdByName(_In_ const std::wstring& procName) const {
	return this->DisplayProcessList(true, procName);
}

// (1). Nothing special here. We just call the documented TerminateFunction passing
// the process handle that the class currently stores.
void ProcessManager::TerminateProcess(_In_ const DWORD ExitCode) {
	::TerminateProcess(this->hProcess, ExitCode);
	this->Cleanup();
}

// (1). Now, in this function and the next one (ResumeProcess) I do something that may seem
// weird for those who are new to this type of programming. To hide the function prototypes from
// the main program, I define VOID function pointers in the class declaration. Obviously, we need
// to cast those two pointers (ptrSuspendProcess and ptrResumeProcess) to the apropriate prototype
// so we can call the function from memory. Because the prototypes of those two functions are
// in this .cpp file, the main.cpp cannot see them.
void ProcessManager::SuspendProcess(void) {
	if (this->hProcess)
		static_cast<ptrNtSuspendProcess>(ProcessManager::ptrSuspendProcess)(this->hProcess);
}

// (1). Read SuspendProcess explanation
void ProcessManager::ResumeProcess(void) {
	if (this->hProcess)
		static_cast<ptrNtResumeProcess>(ProcessManager::ptrResumeProcess)(this->hProcess);
}


// (1). Here we will have a large discussion.
// Basically, consider that you have a CPU chip with 4 cores, 8 threads.
// You probaly heard that before. Lets just rename "8 threads" to "8 logical processors".
// Now, threads are the units of execution. They are the entities that actually RUN code.
// It is a common misconception to beleive that processes "run". They dont, they are just
// a container whose purpose is to MANAGE a group of threads.
// Now, suppose that you have 8 logical processors. So you can run 8 threads simultaneasly.
// But what if you have 20,40 or 2000 threads? (check your task manager) In that case we have to discuss something called
// "thread states" and "priorities".
// Begining with thread states, they just describe what that thread is doing at a particular moment.
// Ready State -> Means that the thread is READY to execute and is probably queued for a logical processor already.
// Wait State -> Thread IS NOT READY TO RUN because it is waiting for some kind of event to occur (i.g user input).
// Running State -> Is actually executing code.
// So, based on that, we can correctly conclude that MOST of the threads are in the WAIT state.
// Now, every thread has a priority which is defined by a combination of: thread relative priority (relative to the thread) + priority class (relative to the process)
// By default, the task manager shows the Priority class ONLY and so doesn't tell us everything about the threads priority.
// Points to note:
// 1 - Threads Priority Range from 0 to 31 (considering special threads in kernel). In practice, for user mode programming: [1, 31]
// 2 - Threads Priority = Threads Relative Priority + Priority Class
// -----------------------------------------------------------------------
// PRIORITY CLASS |		VALUE RANGE			THREADS RELATIVE PRIORITY | VALUE
//		IDLE		  4	[1-15]					 IDLE		             -15
//	BELOW NORMAL	  6	[1-15]   				LOWEST			         -02
//	   NORMAL		  8 [1-15]                BELOW NORMAL			     -01
//	ABOVE NORMAL      10 [1-15]                  NORMAL					 +00
//     HIGH           13 [1-15]               ABOVE NORMAL			     +01
//   REAL TIME        24 [16-31]                HIGHEST					 +02
// --------------------------------			  TIME CRITICAL				 +15
//
// 3 - Priority Class also defines the range of the final threads priority. For every priority class, with the exception of
// the REAL TIME, the final threads priority is in the range [1,15]. The REAL TIME one defines the range [16,31].
//
// So, we basically sum the two numbers and if the value exceeds one of the extreme points (1 and (31 or 15)) then the value
// considered is the closest extreme point. For example, if we have BELOW_NORMAL (6) priority class and IDLE relative priority (-15), we have a
// FINAL thread priority of 1 and NOT -9.
// TODO: [Discuss Round Robin Example]
// TODO: [Discuss Thread Ideal Processor]
// TODO: [Discuss Affinity and Processor Groups]
//
//
void ProcessManager::SetPriorityClass(_In_ const DWORD newPriority) {
	if (this->hProcess)
		::SetPriorityClass(this->hProcess, newPriority);
}

// (1). Function that creates a process with a fake parent process id (you can see the process three with ProcessExplorer.exe)
// This function is just a wrapper that allows executing with two string parameters.
void ProcessManager::CreateSpoofedPProcess(_In_ const std::wstring& processPath, _In_ const std::wstring& parentProcessPath) {
	this->CreateSpoofedPProcess(processPath, this->GetProcessIdByName(parentProcessPath));
}

// (1). Initialize all the variables properly.
// (2). Initialize LPPROC_THREAD_ATTRIBUTE_LIST and allocate memory
// (3). Update the process and thread attribute list with PROC_THREAD_ATTRIBUTE_PARENT_PROCESS
// passing the parent process handle as parameter. Finally, it creates the process with the
// extended version of startup info.
void ProcessManager::CreateSpoofedPProcess(_In_ const std::wstring& exePath, _In_ const DWORD pid) {
	
	// (1)
	// These variables need to be nonconst due to CreateProcess documentation. So we pass them to the stack without const modifier.
	std::wstring procPathCpy(exePath);
	SIZE_T procThreadAttrListSize{ 0 };
	LPPROC_THREAD_ATTRIBUTE_LIST procThreadAttr{ nullptr };
	STARTUPINFOEXW procStartupInfoEx{ 0 };
	PROCESS_INFORMATION procInfo{ 0 };
	HANDLE hParent{ nullptr };
	DWORD parentPid{ pid };

	::ZeroMemory(&procStartupInfoEx, sizeof(STARTUPINFOEXW));
	::ZeroMemory(&procInfo, sizeof(PROCESS_INFORMATION));

	hParent = ::OpenProcess(PROCESS_CREATE_PROCESS, FALSE, parentPid);
	if (!hParent) {
		::wprintf(L"[-] Unable to acquire handle for parent process\n");
		return;
	}

	// (2)
	::InitializeProcThreadAttributeList(nullptr, 1, 0, &procThreadAttrListSize);

	procThreadAttr = (LPPROC_THREAD_ATTRIBUTE_LIST)(new byte[procThreadAttrListSize]);

	if (!::InitializeProcThreadAttributeList(procThreadAttr, 1, 0, &procThreadAttrListSize)) {
		::wprintf(L"[-] Failed to initialize process_thread_attribute list\n");
		goto Cleanup;
	}

	// (3)
	if (!::UpdateProcThreadAttribute(procThreadAttr, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &hParent, sizeof(HANDLE), nullptr, nullptr)) {
		::wprintf(L"[-] Failed initializing proc_thread_attr values\n");
		goto Cleanup;
	}


	procStartupInfoEx.StartupInfo.cb = sizeof(STARTUPINFOEXW);
	procStartupInfoEx.lpAttributeList = procThreadAttr;
	if (!::CreateProcess(nullptr, const_cast<PWSTR>(procPathCpy.data()), nullptr, nullptr, FALSE, EXTENDED_STARTUPINFO_PRESENT, nullptr, nullptr, reinterpret_cast<LPSTARTUPINFO>(&procStartupInfoEx), &procInfo)) {
		::wprintf(L"[-] Failed creating child process with modified parent process: %u\n", ::GetLastError());
		goto Cleanup;
	}


	::wprintf(L"[+] Child process created with pid=%u\n", procInfo.dwProcessId);

	Cleanup:
		if (hParent) {
			::CloseHandle(hParent);
		}
		if (procThreadAttr) {
			::DeleteProcThreadAttributeList(procThreadAttr);
			// delete[] procThreadAttr;
		}
		if (procInfo.hProcess) {
			::CloseHandle(procInfo.hProcess);
		}
		if (procInfo.hThread) {
			::CloseHandle(procInfo.hThread);
		}

}

// (1). Returns the corresponding string for the particular priority value.
const wchar_t* ProcessManager::getPriorityText(_In_ const DWORD priorityValue) const {
	switch (priorityValue) {
	case IDLE_PRIORITY_CLASS:
		return L"Idle Priority Class (4)";
		break;
	case BELOW_NORMAL_PRIORITY_CLASS:
		return L"Below Normal Priority Class (6)";
		break;
	case NORMAL_PRIORITY_CLASS:
		return L"Normal Priority Class (8)";
		break;
	case ABOVE_NORMAL_PRIORITY_CLASS:
		return L"Above Normal Priority Class (10)";
		break;
	case HIGH_PRIORITY_CLASS:
		return L"High Priority Class (13)";
		break;
	case REALTIME_PRIORITY_CLASS:
		return L"Real Time Priority Class (24)";
		break;
	case THREAD_PRIORITY_IDLE:
		return L"Thread Relative Idle Priority (-15)";
		break;
	case THREAD_PRIORITY_LOWEST:
		return L"Thread Relative Lowset Priority (-2)";
		break;
	case THREAD_PRIORITY_BELOW_NORMAL:
		return L"Thread Relative Priority Below Normal (-1)";
		break;
	case THREAD_PRIORITY_NORMAL:
		return L"Thread Relative Idle Priority (0)";
		break;
	case THREAD_PRIORITY_ABOVE_NORMAL:
		return L"Thread Relative Lowset Priority (+1)";
		break;
	case THREAD_PRIORITY_HIGHEST:
		return L"Thread Relative Priority Below Normal (+2)";
		break;
	case THREAD_PRIORITY_TIME_CRITICAL:
		return L"Thread Relative Priority Below Normal (+15)";
		break;
	default:
		return L"";
	}
}