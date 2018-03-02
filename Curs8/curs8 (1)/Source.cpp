#include <stdio.h>
#include <Windows.h>
#define MAX_SEM_COUNT 5

HANDLE barbierMutex;
HANDLE waitSemaphore;

int main() {

	waitSemaphore = CreateSemaphore(
		NULL,           // default security attributes
		MAX_SEM_COUNT,  // initial count
		MAX_SEM_COUNT,  // maximum count
		L"Global\\waitSemaphore");          // unnamed semaphore

	if (waitSemaphore == NULL) {
		printf("CreateSemaphore error: %d\n", GetLastError());
		return 1;
	}

	barbierMutex = CreateMutex(NULL, FALSE, L"Global\\barbierMutex");

	while (1) {
		Sleep(3000);

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		if (!CreateProcess(L"E:\\Bitdefender\\Curs8\\problemaBarbierului\\Debug\\client.exe",   // No module name (use command line)
			NULL,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi))           // Pointer to PROCESS_INFORMATION structure
			
		{
			printf("CreateProcess failed (%d).\n", GetLastError());
			return 1;
		}
	}

	return 0;

}