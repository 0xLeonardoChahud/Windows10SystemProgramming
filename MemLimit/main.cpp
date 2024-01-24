// TODO
#include <iostream>
#include <Windows.h>
#include <wil/resource.h>

int wmain(const int argc, const wchar_t* argv[]) {

	if (argc < 2) {
		printf("Usage: MemLimit.exe <process id> <max commit memory>\n");
		return 0;
	}

	DWORD pid{ static_cast<DWORD>(::_wtoi(argv[1])) };
	DWORD maxCommitMemory{ static_cast<DWORD>(::_wtoi(argv[2])) };
	JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobLimitInfoEx{ 0 };

	::ZeroMemory(&jobLimitInfoEx, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION));

	wil::unique_handle hProcess(::OpenProcess(PROCESS_SET_QUOTA | PROCESS_TERMINATE, FALSE, pid));
	if (!hProcess) {
		printf("[-] Failed acquiring handle to remote process\n");
		return 1;
	}

	wil::unique_handle hJob(::CreateJobObject(nullptr, L"MyCustomJobObject"));
	if (!hJob) {
		printf("[-] Failed creating job object\n");
		return 1;
	}

	if (!::AssignProcessToJobObject(hJob.get(), hProcess.get())) {
		printf("[-] Failed assigning process %u to job object\n", pid);
		return 1;
	}


	jobLimitInfoEx.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_MEMORY;
	jobLimitInfoEx.ProcessMemoryLimit = static_cast<std::size_t>(maxCommitMemory) * (1024);
	if (!::SetInformationJobObject(hJob.get(),JobObjectExtendedLimitInformation, &jobLimitInfoEx, sizeof(JOBOBJECT_EXTENDED_LIMIT_INFORMATION))) {
		printf("[-] Failed setting limits to job object\n");
		return 1;
	}


	
	return 0;
}