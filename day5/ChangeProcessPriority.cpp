// ChangeProcessPriority.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include <Windows.h>

int main()
{
	HANDLE h = GetCurrentProcess(); // actually, its equal to -1
	SetPriorityClass(h, HIGH_PRIORITY_CLASS);
	// SetPriorityClass(h, IDLE_PRIORITY_CLASS);

	getchar();
  return 0;
}
