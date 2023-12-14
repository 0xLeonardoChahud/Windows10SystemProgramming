#include "ProcessManager.h"
#include <limits>
#include <cctype>

void getInput(const std::wstring& prefix, std::wstring& s) {
	while (true) {
		printf("%ws", prefix.data());
		if (!(std::wcin >> s)) {
			std::cin.clear();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			printf("[-] Invalid option selected. Try Again.\n");
			continue;
		}
		break;
	}
}

template <typename T>
void getInput(const std::wstring& prefix, T& p) {
	while (true) {
		printf("%ws", prefix.data());
		if (!(std::cin >> p)) {
			system("cls");
			std::cin.clear();
			std::cin.ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
			printf("[-] Invalid option selected. Try Again.\n");
			continue;
		}
		break;
	}
}



int wmain(const int argc, const wchar_t* argv[]) {
	
	ProcessManager procMg;
	
	unsigned short int option{ 0 };
	std::wstring menu = L"\t[ MENU ]\n\t(1) - Display Process List\n\t(2) - Set current process\n\t(3) - Display Process Information\n\t(4) - Kill Process\n\t(5) - Suspend Process\n\t(6) - Resume Process\n\t(7) - Set Process Priority\n\t(8) - Create process with spoofed parent id\n\t(9) - Exit\n";
	std::wstring lastOperationDone = L"(none)";
	std::wstring aux01;
	std::wstring aux02;
	std::wstring aux03;
	DWORD procId{ 0 };
	DWORD tempBasePriority{ NORMAL_PRIORITY_CLASS };
		
	while (option != 9) {
		
		aux01 = menu + L"\n\t[info] Last operation done : " + lastOperationDone + L"\n\n[Input]: ";
		getInput(aux01, option);

		system("cls");

		switch (option){
		case 1:
			procMg.DisplayProcessList();
			lastOperationDone = L"Process list displayed";
			break;
		case 2:
			aux01 = L"\n\t(1) - Specify process name\n\t(2) - Specify process id\n[ Input ]: ";
			getInput(aux01, option);

			if (option == 1) {
				aux01 = L"[ Type process name ]: ";
				getInput(aux01, aux02);
				procMg.UpdateProcess(aux02);
			}
			else {
				aux01 = L"[ Type process id ]: ";
				getInput(aux01, procId);
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
			aux01 = L"\t (0) - IDLE\n\t(1) - BELOW NORMAL\n\t(2) - NORMAL\n\t(3) - ABOVE NORMAL\n\t(4) - HIGH\n\t(5) - REAL TIME\n\n[ New Base Priority Class ]: ";
			getInput(aux01, option);

			switch (option)
			{
			case 0:
				tempBasePriority = IDLE_PRIORITY_CLASS;
				break;
			case 1:
				tempBasePriority = BELOW_NORMAL_PRIORITY_CLASS;
				break;
			case 2:
				tempBasePriority = NORMAL_PRIORITY_CLASS;
				break;
			case 3:
				tempBasePriority = ABOVE_NORMAL_PRIORITY_CLASS;
				break;
			case 4:
				tempBasePriority = HIGH_PRIORITY_CLASS;
				break;
			case 5:
				tempBasePriority = REALTIME_PRIORITY_CLASS;
				break;
			default:
				break;
			}
			procMg.SetPriorityClass(tempBasePriority);
			break;
		case 8:
			aux01 = L"\n\t[ Child process exe path ]: ";
			getInput(aux01, aux02);;

			aux01 = L"\n\t[ Parent process (name/id) ]: ";
			getInput(aux01, aux03);

			if (std::isdigit(aux03[0])) {
				procMg.CreateSpoofedPProcess(aux02, ::_wtoi(aux03.data()));
			}else{
				procMg.CreateSpoofedPProcess(aux02, aux03);
			}

		case 9:
			break;
		default:
			printf("[-] Invalid option specified\n");
			break;
		}

		printf("\n\n\n");
		procId = 0;
	}
	
	return 0;
}