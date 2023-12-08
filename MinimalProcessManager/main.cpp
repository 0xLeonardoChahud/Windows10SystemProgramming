#include "ProcessManager.h"
#include <limits>


int wmain(const int argc, const wchar_t* argv[]) {
	
	ProcessManager procMg;
	
	unsigned short int option{ 0 };
	std::wstring menu = L"\t[ MENU ]\n\t(1) - Display Process List\n\t(2) - Set current process\n\t(3) - Display Process Information\n\t(4) - Kill Process\n\t(5) - Suspend Process\n\t(6) - Resume Process\n\t(7) - Exit\n\n";
	wchar_t procName[MAX_PATH]{ 0 };
	DWORD procId{ 0 };

	while (option != 7) {
		printf("%ws\n[ Input ]: ", menu.data());
		
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
			break;
		case 3:
			procMg.DisplayProcessInfo();
			break;
		case 4:
			procMg.TerminateProcess();
			break;
		case 5:
			procMg.SuspendProcess();
			break;
		case 6:
			procMg.ResumeProcess();
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