// hello-world.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <windows.h>

int main(int argc, char* argv[])
{
	wchar_t* mName = L"mMutex";
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
			ReleaseMutex(hMutex);
		}
	} else {
		printf("%s\n", "Hello world");
	}
	CloseHandle(hMutex);
	getchar();
        return 0;
}

