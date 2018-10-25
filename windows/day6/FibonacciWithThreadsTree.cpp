// tevosyanartur717@gmail.com

//#include "stdafx.h"
#include <Windows.h>
#include <assert.h>
#include <stdint.h>
#include <iostream>

DWORD WINAPI ThreadFunction(void* params)
{
	assert(NULL != params);
	int id = *((int*)params);

	if (id < 2) {
		return 1;
	} else {
		int param1 = id - 1;
		int param2 = id - 2;
		DWORD id1, id2;

		HANDLE h1 = CreateThread(NULL, 8, ThreadFunction, &param1, 0, &id1);
		HANDLE h2 = CreateThread(NULL, 8, ThreadFunction, &param2, 0, &id2);
		
		if (NULL == h1 || NULL == h2) {
			std::cout << "Faild to create thread" << std::endl;
			return -1;
		}


		DWORD ec1 = 0;
		DWORD ec2 = 0;

		// Wait for thread to exit
		WaitForSingleObject(h2, INFINITE);
		WaitForSingleObject(h1, INFINITE);
		if (GetExitCodeThread(h2, &ec2) && GetExitCodeThread(h1, &ec1)) {

		}
	
		CloseHandle(h1);
		CloseHandle(h2);

		return ec1 + ec2;
	}
}

int main()
{
	DWORD id;

	int param = 20;
	int N = 2; // Too small stack size, everything can be happened here!!!

	HANDLE h = CreateThread(NULL, N, ThreadFunction, &param, 0, &id);
	if (NULL == h) {
		std::cout << "Faild to create thread" << std::endl;
		return 1;
	}

	// Wait for thread to exit
	WaitForSingleObject(h, INFINITE);

	DWORD x = 0;
	if (GetExitCodeThread(h, &x)) {
		std::cout << "probed exit code = " << x << std::endl;
	}
	CloseHandle(h);

	getchar();
	return 0;
}
