#include <Windows.h>
#include <stdio.h>
#include <queue>

CRITICAL_SECTION csSync;
CRITICAL_SECTION priSync;
std::queue<int> codita;

CONDITION_VARIABLE QueueNotEmpty;
HANDLE hEvent;

DWORD WINAPI MyFunc(LPVOID lpParam) {
	//ceva
	while (1) {
		//Sleep(1000);

		EnterCriticalSection(&csSync);
		while (codita.empty()) {
			//EnterCriticalSection(&priSync);
			//printf("waiting thread nb %d\n", GetCurrentThreadId());
			//LeaveCriticalSection(&priSync);
			DWORD wait = WaitForSingleObject(hEvent, 0);
			printf("astept\n");
			if (wait == WAIT_OBJECT_0) {
				//SetEvent(hEvent);
				printf("ies\n");
				break;
			}
			SleepConditionVariableCS(&QueueNotEmpty, &csSync, INFINITE);
			//break;
		}

		if (!codita.empty()) {
			int x = codita.front();
			codita.pop();

			LeaveCriticalSection(&csSync);

			//Sleep(10);

			EnterCriticalSection(&priSync);
			printf("current ThreadId: %d and number: %d\n", GetCurrentThreadId(), x);
			LeaveCriticalSection(&priSync);
		}
		else {
			LeaveCriticalSection(&csSync);
			break;
		}
	}

	return 1;
}


int main() {

	HANDLE dwHandles[2];
	DWORD dwThreadId[2];

	InitializeCriticalSection(&csSync);
	InitializeCriticalSection(&priSync);
	hEvent = CreateEvent(
		NULL,
		TRUE,
		FALSE,
		NULL);

	if (hEvent == NULL) {
		printf("CreateEvent failed (%d)\n", GetLastError());
		return -1;
	}

	for (int i = 0; i < 2; ++i) {
		dwHandles[i] = CreateThread(NULL, 0, MyFunc, NULL, 0, &dwThreadId[i]);
	}

	for (int i = 0; i < 30; ++i) {
		codita.push(i);
		WakeConditionVariable(&QueueNotEmpty);
	}

	SetEvent(hEvent);

	printf("Asteptam\n");
	WaitForSingleObject(dwHandles[0], INFINITE);
	printf("primul  %d\n", dwThreadId[0]);
	WaitForSingleObject(dwHandles[1], INFINITE);
	printf("s a terminat\n");

	DeleteCriticalSection(&csSync);
	DeleteCriticalSection(&priSync);

	return 0;
}
