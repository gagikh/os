#include <Windows.h>
#include <cstdlib>
#include <iostream>
using namespace std;

int main()
{
	HANDLE x0 = OpenMutex(0, 0, L"left");
	HANDLE x1 = OpenMutex(0, 0, L"right");

	if (x0 == NULL) {
		x0 = CreateMutex(0, 0, L"left");
	}
	if (NULL == x1) {
		x1 = CreateMutex(0, 0, L"right");
	}

	while (true) {
		cout << "Waiting for forks" << endl;
		WaitForSingleObject(x1, INFINITE); // take left fork
		WaitForSingleObject(x0, INFINITE); // take right fork
		std::cout << "0 - start eating" << std::endl;

		Sleep(10); // eat


		ReleaseMutex(x1); // take down
		ReleaseMutex(x0); // take down
		std::cout << "1 - start thinking" << std::endl;
		Sleep(100); // think
	}
	return 0;
}
