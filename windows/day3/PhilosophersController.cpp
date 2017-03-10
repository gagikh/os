#include "stdafx.h"
#include <windows.h>

int main()
{
	STARTUPINFO si[2];
	PROCESS_INFORMATION pi[2];
	
	wchar_t* path = L"Philosopher.exe";
	wchar_t* params[] = { L"Process 0", L"Process 1" };

	for (int i = 0; i < 2; ++i) {
		memset(&si[i], 0, sizeof(si[i]));
		si[i].cb = sizeof(si[i]);

		int status = CreateProcess(path, params[i], NULL, NULL, 0, 0, NULL, NULL, &si[i], &pi[i]);

		if (!status) {
			printf("Faild to start %d process - %d\n", i, status);
			return 0;
		}
	}
	for (int i = 0; i < 2; ++i) {
		WaitForSingleObject(pi[i].hProcess, INFINITE);
		CloseHandle(pi[i].hThread);
		CloseHandle(pi[i].hProcess);
	}

	getchar();

	return 0;
}
