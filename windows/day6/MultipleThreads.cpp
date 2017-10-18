#include <stdio.h>
#include <Windows.h>
#include <assert.h>
#include <stdint.h>
#include <iostream>


// Create multiple threads to experiment increment operator
// @author Hayk Martirosyan <haykmartirosyan21@gmail.com>
int x = 0;

int f(int N) {
	for (auto i = 0; i < N; i++) {
		x++;
	}
	return x;
}

DWORD WINAPI ThreadFunction(void* params)
{
	assert(NULL != params);
	int n = *((int*)params);
	
	return f(n);	
}

#define L 1000

int main()
{
	DWORD id;

	int param = 1000;
	int N = 2;
  
	HANDLE h[L];
	for (int i = 0; i < L; i++) {
		h[i] = CreateThread(NULL, N, ThreadFunction, &param, 0, &id);
		if (NULL == h[i]) {
			std::cout << "Faild to create thread" << std::endl;
			return 1;
		}
	}
	// Wait for thread to exit
	::WaitForMultipleObjects(L, h, true, INFINITE);
	for (int i = 0; i < L; i++) {
		DWORD ec = 0;
		if (GetExitCodeThread(h[i], &ec)) {
			std::cout << "probed exit code = " << ec << std::endl;
		}

		CloseHandle(h[i]);
	}

	std::cout << x << std::endl;
	getchar();
	return 0;

}
