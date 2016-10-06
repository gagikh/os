#include <iostream>
#include <Windows.h>

using namespace std;

int main()
{
      STARTUPINFO si;
      PROCESS_INFORMATION pi;
      
      memset(&si, 0, sizeof(si));
      si.cb = sizeof(si);
      
      wchar_t* path = L"C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe";
      wchar_t* params = L"chrome.exe facebook.com";
      
      wchar_t arr[1024];
      GetEnvironmentVariable(L"PATH", arr, 1024);
      
      // Check if we can invoke the CreateProcess API using executable name only
      // - Get system path variable, split, append executable name and check file existenc
      // - execlp(), execvp(), and execvpe() APIs do those steps automatically
      
      int status = CreateProcess(path, params, NULL, NULL, 0, 0, NULL, NULL, &si, &pi);
      if (! status) {
        printf("Faild to start process - %d\n", status);
        return 0;
      }
      
      // Increase the priority
      SetPriorityClass(pi.hProcess, HIGH_PRIORITY_CLASS);
      return 0;
}
