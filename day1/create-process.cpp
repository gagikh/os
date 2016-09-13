// create-process.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

int main()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	wchar_t* path = L"hello-world.exe";
	wchar_t* params = L"test1 test2 test3";
	wchar_t* mName = L"mMutex";

	HANDLE hMutex = CreateMutex(
		NULL,     // no security descriptor
		FALSE,    // mutex not owned
		mName);   // object name

	if (NULL == hMutex) {
		printf("Error creating mutex\n");
		return 0;
	}

	int status = CreateProcess(path, params, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);

	if (!status) {
		printf("Faild to start process - %d\n", status);
		return 0;
	}

	for (int i = 0; i < 10; ++i) {
		// wait to run
		int dwWaitResult = WaitForSingleObject(
			hMutex,		// handle to mutex
			INFINITE);  //
		printf("Parent gets the owner ship %d\n", i);

		Sleep(1);
		printf("Parent releases the owner ship %d\n", i);
		ReleaseMutex(hMutex);
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(hMutex);

	getchar();

        return 0;
}
