#include "MyHelper.hpp"


std::vector<THREAD_INFO> EnumThreads(const int pid) {
	std::vector<THREAD_INFO> threads;
	std::unordered_map<DWORD, PROCESSENTRY32> procs;
	PROCESSENTRY32 pe{ sizeof(PROCESSENTRY32) };
	THREADENTRY32 te{ sizeof(THREADENTRY32) };
	HANDLE hSnapshot{ nullptr };
	HANDLE hThread{ nullptr };

	hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPTHREAD, 0);
	if (!hSnapshot) {
		return threads;
	}

	::Process32First(hSnapshot, &pe);

	do {
		procs.insert({pe.th32ProcessID, pe});
	} while (::Process32Next(hSnapshot, &pe));

	::Thread32First(hSnapshot, &te);

	do {

		if (pid > 0 && te.th32OwnerProcessID != pid)
			continue;

		THREAD_INFO tinfo;
		tinfo.Id = te.th32ThreadID;
		tinfo.Pid = te.th32OwnerProcessID;
		tinfo.Priority = te.tpBasePri;
		tinfo.processName = procs[tinfo.Pid].szExeFile;

		hThread = ::OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, tinfo.Id);
		if (hThread) {
			FILETIME user, kernel, exit;
			::GetThreadTimes(hThread, &tinfo.CreateTime, &exit, &kernel, &user);
			tinfo.CPUTime = ((*reinterpret_cast<PULONGLONG>(&kernel) + *reinterpret_cast<PULONGLONG>(&user)) / 1e7);
			::CloseHandle(hThread);
		}
		else {
			tinfo.CPUTime = 0;
			tinfo.CreateTime.dwHighDateTime = tinfo.CreateTime.dwLowDateTime = 0;
		}

		threads.push_back(std::move(tinfo));

	} while (::Thread32Next(hSnapshot, &te));

	::CloseHandle(hSnapshot);
	
	return threads;
}