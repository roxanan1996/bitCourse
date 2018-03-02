#include <Windows.h>
#include <stdio.h>

HANDLE tutunSemaphore;
HANDLE chibrituriSemaphore;

int main() {
	printf("am intrat in hartie\n");
	tutunSemaphore = OpenSemaphore(SYNCHRONIZE,
		FALSE,
		L"Local\\tutunSemaphore");

	if (tutunSemaphore == NULL) {
		printf("cannot open tutunSemaphore %d \n", GetLastError());
	}

	chibrituriSemaphore = OpenSemaphore(SYNCHRONIZE,
		FALSE,
		L"Local\\chibrituriSemaphore");

	if (chibrituriSemaphore == NULL) {
		printf("cannot open chibrituriSemaphore si sunt in %d \n", GetLastError());
	}

	while (1) {
		WaitForSingleObject(chibrituriSemaphore, INFINITE);

		DWORD res = WaitForSingleObject(tutunSemaphore, 0);

		if (res == WAIT_TIMEOUT) {
			//printf("am hartie si astept dupa chibrituri\n");
			ReleaseSemaphore(chibrituriSemaphore, 1, NULL);
		}
		else if (res == WAIT_OBJECT_0) {
			printf("am hartie si tigara iee\n");
			Sleep(1000);
			ReleaseSemaphore(chibrituriSemaphore, 1, NULL);
			ReleaseSemaphore(tutunSemaphore, 1, NULL);
		}
	}

}
