#include <Windows.h>
#include <stdio.h>

HANDLE hartieSemaphore;
HANDLE tutunSemaphore;

int main() {
	printf("am intrat in chibrituri\n");

	hartieSemaphore = OpenSemaphore(SYNCHRONIZE,
		FALSE,
		L"Local\\hartieSemaphore");

	if (hartieSemaphore == NULL) {
		printf("cannot open hartieSemaphore %d \n", GetLastError());
	}

	tutunSemaphore = OpenSemaphore(SYNCHRONIZE,
		FALSE,
		L"Local\\tutunSemaphore");

	if (tutunSemaphore == NULL) {
		printf("cannot open tutunSemaphore %d \n", GetLastError());
	}

	while (1) {
		WaitForSingleObject(hartieSemaphore, INFINITE);

		DWORD res = WaitForSingleObject(tutunSemaphore, 0);

		if (res == WAIT_TIMEOUT) {
			//printf("am chibrituri si astept dupa hartie\n");
			ReleaseSemaphore(hartieSemaphore, 1, NULL);
		}
		else if (res == WAIT_OBJECT_0) {

			//sprintf("am tutun si tigara ieee\n");
			Sleep(1000);
			ReleaseSemaphore(tutunSemaphore, 1, NULL);
			ReleaseSemaphore(hartieSemaphore, 1, NULL);
		}
	}

}
