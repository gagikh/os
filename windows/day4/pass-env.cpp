#include <windows.h>
#include <stdio.h>
#include <tchar.h>

wchar_t* environx = { L"CDX=DDDD\0EEEE=BBBB\0" };

void _tmain()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	auto* path = L"C:\\Users\\...\\test.exe";
	wchar_t* params = L"name param1 param2";

	int status = CreateProcess(path, params, NULL, NULL, 0, CREATE_UNICODE_ENVIRONMENT, environx, NULL, &si, &pi);

	if (!status) {
		printf("CreateProcess failed (%d).\n", GetLastError());
		return;
	}
	WaitForSingleObject(pi.hProcess, 5000);

	TerminateThread(pi.hThread, 0);
	TerminateProcess(pi.hProcess, 0);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
}

// test.cpp

/**
int main(int argc, char **argv, char **envp)
{
	for (char **env = envp; *env != 0; env++)
	{
		char *thisEnv = *env;
		printf("%s\n", thisEnv);
	}
	return 0;
}
*/
