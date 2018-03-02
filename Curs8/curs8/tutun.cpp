#include <Windows.h>
#include <stdio.h>

HANDLE hartieSemaphore;
HANDLE chibrituriSemaphore;

int main() {
	
	printf("am intrat in tutun\n");
	hartieSemaphore = OpenSemaphore(SYNCHRONIZE,
		FALSE,
		L"Local\\hartieSemaphore");

	if (hartieSemaphore == NULL) {
		printf("cannot open hartieSemaphore %d \n", GetLastError());
	}

	chibrituriSemaphore = OpenSemaphore(SYNCHRONIZE,
		FALSE,
		L"Local\\chibrituriSemaphore");

	if (chibrituriSemaphore == NULL) {
		printf("cannot open chibrituriSemaphore %d \n", GetLastError());
	}

	while (1) {
		WaitForSingleObject(chibrituriSemaphore, INFINITE);

		DWORD res = WaitForSingleObject(hartieSemaphore, 0);

		if (res == WAIT_TIMEOUT) {
			//printf("am tutun si astept dupa chibrituri\n");
			ReleaseSemaphore(chibrituriSemaphore, 1, NULL);
		}
		else if (res == WAIT_OBJECT_0) {
			printf("am tutun si tigara iee\n");
			Sleep(1000);
			ReleaseSemaphore(chibrituriSemaphore, 1, NULL);
			ReleaseSemaphore(hartieSemaphore, 1, NULL);
		}
	}

}
