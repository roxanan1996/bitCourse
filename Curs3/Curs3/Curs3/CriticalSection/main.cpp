
#include <Windows.h>
#include <stdio.h>
int contor = 0;

#define MAXIMUM_INCREASE 10
CRITICAL_SECTION csSync;
CRITICAL_SECTION print;
HANDLE hMutex;

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	for (int i = 0; i < MAXIMUM_INCREASE; i++)
	{
		EnterCriticalSection(&csSync);
		//WaitForSingleObject(hMutex, INFINITE);
		contor++;
		//ReleaseMutex(hMutex);
		LeaveCriticalSection(&csSync);
		EnterCriticalSection(&print);
		printf("a intrat %d\n", *(int*)lpParam);
		LeaveCriticalSection(&print);
	}
	return 0;
}

int main()
{
	HANDLE hThr[2];
	DWORD dwThreadId[2];

	InitializeCriticalSectionAndSpinCount(&csSync, 0x1000);
	InitializeCriticalSection(&print);

	//hMutex = CreateMutex(NULL, 0, NULL);

	for (int i = 0; i < 2; i++)
	{
		int *ceva = new int();
		*ceva = i;
		hThr[i] = CreateThread(NULL, 0, ThreadProc, ceva, 0, &dwThreadId[i]);
	}

	WaitForMultipleObjects(2, hThr, TRUE, INFINITE);
	DeleteCriticalSection(&csSync);
	DeleteCriticalSection(&print);
	//CloseHandle(hMutex);
	printf("contor = %d\n", contor);
	return 0;
}