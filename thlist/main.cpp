#include <iostream>
#include "MyHelper.hpp"



int wmain(const int argc, const wchar_t* const argv[]) {


	auto threads = EnumThreads(0);

	::wprintf_s(L"%30ws %12s %12s %12s %12s %12s\n",
		L"PROCESS NAME", L"PID", L"TID", L"PRIORITY", L"STARTED", L"CPU TIME");

	// TODO: Format time
	for (auto tinfo : threads) {
		::wprintf_s(L"%30ws %12u %12u %12u %12u %12u\n",
					  tinfo.processName.data(),
					  tinfo.Pid,
					  tinfo.Id,
					  tinfo.Priority,
					  tinfo.CreateTime,
					  tinfo.CPUTime);
		
	}


	return 0;
}