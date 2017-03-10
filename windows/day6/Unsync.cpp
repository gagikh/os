#include "stdafx.h"
#include <Windows.h>
#include <assert.h>
#include <stdint.h>
#include <iostream>

int accumulator = 0;

int f(int i) {
	Sleep(25);
	return ++i;
}

DWORD WINAPI ThreadFunction(void* params)
{
	assert(NULL != params);
	int id = *((int*)params);

	std::cout << "Running thread: " << id << std::endl;
	accumulator = f(accumulator);

	return 0;
}

#define N 50
#define S 10

int params[N];

int main()
{
	DWORD id[N];
	HANDLE h[N];

	for (int i = 0; i < N; ++i) {
		params[i] = i;
		h[i] = CreateThread(NULL, S, ThreadFunction, &params[i], 0, &id[i]);
		if (NULL == h[i]) {
			std::cout << "Faild to create thread: " << i << std::endl;
		  return 1; // is this ok?
		}
	}

	// Wait for threads to exit
	WaitForMultipleObjects(N, h, true, INFINITE);

	std::cout << "Accumulator value = " << accumulator << std::endl;

	getchar();
	return 0;
}
