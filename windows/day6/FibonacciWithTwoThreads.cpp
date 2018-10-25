// susanna.beglaryan@gmail.com

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

DWORD getFibonacci(int param);

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

	int param = 20;

	DWORD ec = getFibonacci(param);

	std::cout << "probed exit code = " << ec << std::endl;

	getchar();
	return 0;
}

DWORD getFibonacci(int param) {
	DWORD id;

	int p1 = param - 1;
	int p2 = param - 2;
	int N = 2; // Too small stack size, everything can be happened here!!!

	HANDLE h1 = CreateThread(NULL, N, ThreadFunction, &p1, 0, &id);
	HANDLE h2 = CreateThread(NULL, N, ThreadFunction, &p2, 0, &id);
	if (NULL == h1 && NULL == h2) {
		std::cout << "Faild to create thread" << std::endl;
		return 1;
	}

	// Wait for thread to exit
	WaitForSingleObject(h1, INFINITE);
	WaitForSingleObject(h2, INFINITE);

	DWORD ec1 = 0;
	DWORD ec2 = 0;

	if (GetExitCodeThread(h1, &ec1) && GetExitCodeThread(h2, &ec2)) {
		std::cout << "Success!" <<  std::endl;
	}

	CloseHandle(h1);
	CloseHandle(h2);

	DWORD ec = ec1 + ec2;

	return ec;
}
