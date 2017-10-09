#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;
// @author - Sano Madoyan, madoyan1996@gmail.com, 52-2

// Use command line to open google chrome and make a search
// Search text is passed as an input argument
int main(int argc, char* argv[])
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	wchar_t* path = L"C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe";

	//Add from Sano in 09/10/2017
	string text = "chrome https://www.google.ru/search?q=";
	text = text.append(argv[1]);
	cout << "Searching text with the command: \"" << text << "\"" << endl;

	wstring wstr = wstring(text.begin(), text.end());
	const wchar_t* wcrt = wstr.c_str();

	wchar_t* params = const_cast<wchar_t*>(wcrt);

	int status = CreateProcess(path, params, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);

	if (! status) {
		printf("Faild to start process - %d\n", status);
		return 0;
	}

	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);

	getchar();
	return 0;
}
