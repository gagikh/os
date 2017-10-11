#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

// Experiment process synchornization with a mutex
// Write file in parallel
// @author: Edgar Pachisnky <www.edgar.pachinsky@gmail.com>, 52-1
int main(int argc, char* argv[])
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	wchar_t* mName = L"mMutex";
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, mName);

	if (NULL == hMutex) {
		cout << ("Unable to get access to the mutex\n");
	  // Try to create new one if not accessible
		hMutex = CreateMutex(
			NULL,     // no security descriptor
			FALSE,    // mutex not owned
			mName);   // object name
		
	}
	if (NULL == hMutex) {
		cout << ("Error creating mutex\n");
		return 0;
	}

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	fstream out;
	out.open("notepad.txt",ofstream::app);

	wchar_t* path = L"C:\\Windows\\notepad.exe";
	wchar_t* params = L"notepad notepad.txt";

	for (int i = 0; i < 10; i++)
	{
		int dwWaitResult = WaitForSingleObject(
			hMutex,		// handle to mutex
			INFINITE);  //
			printf("Parent gets the owner ship %d\n", i);

		Sleep(1);
		out << "hello:";
		ReleaseMutex(hMutex);
	}

  // TO view the text
	int status = CreateProcess(path, params, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);

	if (!status) {
		printf("Faild to start process - %d\n", status);
		return 0;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	getchar();
	return 0;
}
