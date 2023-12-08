#include "ProcessManager.h"
#include <limits>


int wmain(const int argc, const wchar_t* argv[]) {
	
	ProcessManager procMg;
	
	unsigned short int option{ 0 };
	std::wstring menu = L"\t[ MENU ]\n\t(1) - Display Process List\n\t(2) - Set current process\n\t(3) - Display Process Information\n\t(4) - Kill Process\n\t(5) - Suspend Process\n\t(6) - Resume Process\n\t(7) - Exit\n";
	wchar_t procName[MAX_PATH]{ 0 };
	std::wstring lastOperationDone = L"(none)";
	DWORD procId{ 0 };
		
	while (option != 7) {
		printf("%ws\n\t[ info ] Last operation done: %ws\n\n[ Input ]: ", menu.data(), lastOperationDone.data());
		
		if (!(std::cin >> option)) {
			std::cin.clear();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			printf("[-] Invalid option selected\n");
			continue;
		}

		system("cls");

		switch (option){
		case 1:
			procMg.DisplayProcessList();
			lastOperationDone = L"Process list displayed";
			break;
		case 2:
			printf("\n\t(1) - Specify process name\n\t(2) - Specify process id\n");
			printf("[ Input ]: ");
			
			if (!(std::cin >> option)) {
				std::cin.clear();
				std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
				printf("[-] Invalid option selected\n");
				break;
			}

			if (option == 1) {
				printf("[ Type process name ]: ");
				std::wcin >> procName;
				procMg.UpdateProcess(procName);
			}
			else {
				printf("[ Type process id ]: ");
				std::cin >> procId;
				procMg.UpdateProcess(procId);
			}
			lastOperationDone = L"Process Updated";
			break;
		case 3:
			procMg.DisplayProcessInfo();
			lastOperationDone = L"Process Info Displayed";
			break;
		case 4:
			procMg.TerminateProcess();
			lastOperationDone = L"Process Terminated";
			break;
		case 5:
			procMg.SuspendProcess();
			lastOperationDone = L"Process Suspended";
			break;
		case 6:
			procMg.ResumeProcess();
			lastOperationDone = L"Process Resumed";
			break;
		case 7:
			break;
		default:
			printf("[-] Invalid option specified\n");
			break;
		}

		printf("\n\n\n");
		::memset(procName, 0, sizeof(wchar_t) * MAX_PATH);
		procId = 0;
	}
	
	return 0;
}