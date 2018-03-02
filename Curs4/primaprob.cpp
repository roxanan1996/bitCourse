#include <Windows.h>
#include <stdio.h>

#define MAX_SEM_COUNT 1
HANDLE dwSemPar, dwSemImpar;
CRITICAL_SECTION csSync;

DWORD WINAPI MyFuncPar(LPVOID lpParam) {
	DWORD dwWaitResult;
	for (int i = 0; i < 100; i += 2) {
		dwWaitResult = WaitForSingleObject(dwSemPar, INFINITE);
		switch (dwWaitResult) {
			case WAIT_OBJECT_0:
				EnterCriticalSection(&csSync);
				printf("%d\n", i);
				LeaveCriticalSection(&csSync);
				Sleep(1000);
				if (!ReleaseSemaphore(dwSemImpar, 1, NULL)) {
					printf("Error\n");
				}
				break;
			case WAIT_TIMEOUT:
				printf("error\n");
				break;
		}
	}
	return 0;
}

DWORD WINAPI MyFuncImpar(LPVOID lpParam) {
	DWORD dwWaitResult;
	for (int i = 1; i < 100; i += 2) {
		dwWaitResult = WaitForSingleObject(dwSemImpar, INFINITE);
		switch (dwWaitResult) {
			case WAIT_OBJECT_0:
				EnterCriticalSection(&csSync);
				printf("%d\n", i);
				LeaveCriticalSection(&csSync);
				Sleep(1000);
				if (!ReleaseSemaphore(dwSemPar, 1, NULL)) {
					printf("Error\n");
				}
				break;
			case WAIT_TIMEOUT:
				printf("error\n");
				break;
		}
	}
	return 0;
}
int main() {

	HANDLE dwHandles[2];
	DWORD dwThreadId[2];

	InitializeCriticalSection(&csSync);
	dwSemPar = CreateSemaphore(NULL, 1, 1, NULL);
	dwSemImpar = CreateSemaphore(NULL, 0, 1, NULL);

	if (dwSemPar == NULL) {
		printf("CreateSemaphore error: %d\n", GetLastError());
		return - 1;
	}

	dwHandles[0] = CreateThread(NULL, 0, MyFuncPar, NULL, 0, &dwThreadId[0]);
	dwHandles[1] = CreateThread(NULL, 0, MyFuncImpar, NULL, 0, &dwThreadId[1]);

	WaitForMultipleObjects(2, dwHandles, TRUE, INFINITE);
	DeleteCriticalSection(&csSync);
	CloseHandle(dwSemPar);
	CloseHandle(dwSemImpar);

}