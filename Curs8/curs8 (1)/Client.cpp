#include <stdio.h>
#include <Windows.h>


int main() {
	HANDLE barbierMutex;
	HANDLE waitSemaphore;

	waitSemaphore = OpenSemaphore(
		SYNCHRONIZE | SEMAPHORE_MODIFY_STATE,
		NULL,
		L"Global\\waitSemaphore");

	barbierMutex = OpenMutex(SYNCHRONIZE,
							NULL,
							L"Global\\barbierMutex");


	DWORD res = WaitForSingleObject(waitSemaphore, 0);

	if (res == WAIT_TIMEOUT) {
		printf("nu mai sunt locuri libere :( \n");
		exit(0);
	}

	printf(" astept\n");
	res = WaitForSingleObject(barbierMutex, INFINITE);

	ReleaseSemaphore(waitSemaphore, 1, NULL);

	printf("ma tund :D\n");
	Sleep(3000);
	ReleaseMutex(barbierMutex);

	return 0;
}