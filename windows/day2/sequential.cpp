// @Artur Tevosyan <tevosyanartur717@gmail.com>
// Sequentially open links with 2 process.

#include <stdlib.h>
#include <windows.h>
#include <stdio.h>


// Child process
int main(int argc, char* argv[])
{
	char* mName = "mMutex";
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, mName);

	if (NULL == hMutex) {
		printf("Unable to get access to the mutex\n");
		return 0;
	}
	if (1 < argc) {
		for (int i = 0; i < argc; ++i) {
			// Take the ownership
			WaitForSingleObject(hMutex, INFINITE);
			printf("	->>>child process - arg[%d] = %s\n", i, argv[i]);

			//Sleep(5);

			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			memset(&si, 0, sizeof(si));
			si.cb = sizeof(si);

			char* path = "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe";
			char* params = "ya.ru yahoo.ru";
			int status = CreateProcess(path, params, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);

			if (!status) {
				printf("Faild to start process - %d\n", status);
				return 0;

			}
			WaitForSingleObject(pi.hProcess, 5000);

			TerminateThread(pi.hThread, 0);
			TerminateProcess(pi.hProcess, 0);

			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);

			printf("	->>>child process releases - arg[%d] = %s\n", i, argv[i]);
			ReleaseMutex(hMutex);
		}
	}
	else {
		printf("%s\n", "Hello world");
	}
	CloseHandle(hMutex);
	getchar();
	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

// Parent process
#include <stdio.h>
#include <windows.h>

int main()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	char* path = "child.exe";
	char* params = "child google.com";
	char* mName = "mMutex";

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

	for (int i = 0; i < 3; ++i) {
		// wait to run
		int dwWaitResult = WaitForSingleObject(
			hMutex,		// handle to mutex
			INFINITE);  //
		printf("Parent gets the owner ship %d\n", i);

		//Sleep(5);
		char* path = "C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe";
		char* params = "ya.ru google.com";
		int status = CreateProcess(path, params, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);

		if (!status) {
			printf("Faild to start process - %d\n", status);
			return 0;

		}
		WaitForSingleObject(pi.hProcess, 5000);
		TerminateThread(pi.hThread, 0);
		TerminateProcess(pi.hProcess, 0);
		

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);

		printf("Parent releases the owner ship %d\n", i);
		ReleaseMutex(hMutex);
	}

	WaitForSingleObject(pi.hProcess, 5000);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(hMutex);

	return 0;
}
