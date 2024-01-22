#include <iostream>
#include <memory>
#include <Windows.h>
#include <wil/resource.h>

#pragma comment(lib, "Synchronization.lib")

// Waiter
DWORD WINAPI Thread1(LPVOID lpParam);

// Modifier
DWORD WINAPI Thread2(LPVOID lpParam);


int wmain(const int argc, const wchar_t* const argv[]) {

	uint8_t monitoredValue{ 0 };

	wil::unique_handle hThread1(::CreateThread(nullptr, 0, Thread1, &monitoredValue, 0, nullptr));
	::Sleep(500);
	wil::unique_handle hThread2(::CreateThread(nullptr, 0, Thread2, &monitoredValue, 0, nullptr));

	if (!hThread1 || !hThread2) {
		std::wcerr << L"[-] Failed creating threads" << std::endl;
		return 1;
	}
	
	::WaitForSingleObject(hThread1.get(), INFINITE);

	return 0;
}


// Waiter
DWORD WINAPI Thread1(LPVOID lpParam) {
	uint8_t& value = *static_cast<uint8_t*>(lpParam);
	uint8_t undesired{ value };
	BOOL ret{ FALSE };
	const DWORD ctid{ ::GetCurrentThreadId() };

	std::wcout << L"[ tid:" << ctid << " ] " << "Checking if values are the same and waiting" << std::endl;
	ret = ::WaitOnAddress(&value, &undesired, sizeof(uint8_t), INFINITE);
	std::wcout << L"[ tid:" << ctid << " ] " << L"Wait returned: " << ret << L", Value: " << value << L", Undesired: " << undesired << std::endl;

	return 0;
}

// Modifier
DWORD WINAPI Thread2(LPVOID lpParam) {

	uint8_t& value = *static_cast<uint8_t*>(lpParam);
	const DWORD ctid{ ::GetCurrentThreadId() };
	std::wcout << L"[ tid:" << ctid << " ] " << "Changing value in 5 sec and trying to wake the thread" << std::endl;
	::Sleep(5000);
	value++;
	::WakeByAddressSingle(&value);

	return 0;
}
