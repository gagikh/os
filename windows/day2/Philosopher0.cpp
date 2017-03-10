// Philosopher0.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>

#include <iostream>

inline void think()
{
	int s = rand() / 10;
	std::cout << "0 - Philosopher thinks: " << s << " secs" << std::endl;
	Sleep(s);
}

inline void eat()
{
	int s = rand() / 10;
	std::cout << "0 - Philosopher eats: " << s << " secs" << std::endl;
	Sleep(s);
}

int main()
{
	HANDLE x0 = OpenMutex(NULL, 0, L"left");
	HANDLE x1 = OpenMutex(NULL, 0, L"right");

	if (NULL == x0) {
		x0 = CreateMutex(NULL, 0, L"left");
	}

	if (NULL == x1) {
		x1 = CreateMutex(NULL, 0, L"right");
	}

	while (true) {
		std::cout << "0 - Philophper takes left fork" << std::endl;
		if (WAIT_OBJECT_0 != WaitForSingleObject(x0, INFINITE)) {
			continue;
		}
		std::cout << "0 - Philophper takes right fork" << std::endl;
		if (WAIT_OBJECT_0 != WaitForSingleObject(x1, INFINITE)) {
			ReleaseMutex(x0);
			continue;
		}
		eat();
		ReleaseMutex(x0);
		ReleaseMutex(x1);
		think();
	}
	return 0;
}
