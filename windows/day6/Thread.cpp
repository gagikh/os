#include "stdafx.h"
#include <Windows.h>
#include <assert.h>
#include <stdint.h>
#include <iostream>

int f(int i) {
	if (i < 2) {
		return 1;
	}
	return f(i - 1) + f(i - 2);
}

DWORD WINAPI ThreadFunction(void* params)
{
	assert(NULL != params);
	int id = *((int*)params);

	HANDLE h = GetCurrentThread();

	int c = GetThreadPriority(h);
	std::cout << "Pririty class: " << c << std::endl;

	return f(id);
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

	DWORD ec = 0;
	if (GetExitCodeThread(h, &ec)) {
		std::cout << "probed exit code = " << ec << std::endl;
	}
	CloseHandle(h);
	
	getchar();
  return 0;
}
