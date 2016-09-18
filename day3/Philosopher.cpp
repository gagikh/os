// Philosopher.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <Windows.h>

#include <iostream>

static const wchar_t* mutexes[2][2] = {
	{L"left", L"right"},
	{L"right", L"left"}
};

#define GET_MUTEX_NAME(philosopher_id, fork_id) (mutexes[philosopher_id][fork_id])

inline void think(int id)
{
	int s = rand() / 10;
	std::cout << id << "-th Philosopher thinks: " << s << " secs" << std::endl;
	Sleep(s);
}

inline void eat(int id)
{
	int s = rand();
	std::cout << id << "-th Philosopher eats: " << s << " secs" << std::endl;
	Sleep(s);
}

inline HANDLE get_mutex(const wchar_t* mutex_name)
{
	HANDLE x = OpenMutex(NULL, 0, mutex_name);
	if (NULL == x) {
		x = CreateMutex(NULL, 0, mutex_name);
	}
	return x;
}

int main(int argc, char** argv)
{
	if (2 > argc) {
		std::cout << "Philosopher id is not specified" << std::endl;
		return 1;
	}
	int id = atoi(argv[1]);
	if (0 != id && 1 != id) {
		std::cout << "Invalid Philosopher id" << std::endl;
		return 1;
	}

	const wchar_t* left_mutex_name = GET_MUTEX_NAME(id, 0);
	const wchar_t* right_mutex_name = GET_MUTEX_NAME(id, 1);

	std::cout << "-->first mutex name: " << left_mutex_name << std::endl;
	std::cout << "-->second mutex name: " << right_mutex_name << std::endl;

	HANDLE x0 = get_mutex(left_mutex_name);
	HANDLE x1 = get_mutex(right_mutex_name);

	while (true) {
		std::cout << id << "-th Philophper takes left fork" << std::endl;
		if (WAIT_OBJECT_0 != WaitForSingleObject(x0, INFINITE)) {
			continue;
		}
		std::cout << id << "-th Philophper takes right fork" << std::endl;
		if (WAIT_OBJECT_0 != WaitForSingleObject(x1, INFINITE)) {
			ReleaseMutex(x0);
			continue;
		}
		eat(id);
		ReleaseMutex(x0);
		ReleaseMutex(x1);
		think(id);
	}
	return 0;
}
