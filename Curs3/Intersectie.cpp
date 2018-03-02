#include <windows.h>
#include <stdio.h>

HANDLE hEventNS;
HANDLE hEventEW;
HANDLE hEventPlasator;

CRITICAL_SECTION csSync;

int nrNS = 0; //cate masini asteapta pe directia nord sud
int nrEW = 0; //cate masini asteapta pe directia est west
int nCrtDirection = 0; //0 - NS, 1, EW
int nChangeDirectionStarted = 0;
#define MAX_THRESHOLD    3
#define DIRECTION_NS     0
#define DIRECTION_EW     1

DWORD WINAPI MasinaNordSud(LPVOID param)
{
	printf("Incerc sa ma deplasez pe NS\n");
	EnterCriticalSection(&csSync);
	nrNS = nrNS + 1;
	if (nrNS > MAX_THRESHOLD && (nCrtDirection == DIRECTION_EW) && !nChangeDirectionStarted)
	{
		printf("Trezim planificator for NS- changing direction\n");
		nChangeDirectionStarted = 1;
		SetEvent(hEventPlasator);
	}
	LeaveCriticalSection(&csSync);
	WaitForSingleObject(hEventNS, INFINITE);
	printf("Am intrat in intersectia pe directia NS si am plecat\n");
	EnterCriticalSection(&csSync);
	nrNS = nrNS - 1;
	LeaveCriticalSection(&csSync);
	return 0;
}

DWORD WINAPI MasinaEstVest(LPVOID param)
{
	printf("Incerc sa ma deplasez pe EW\n");
	EnterCriticalSection(&csSync);
	nrEW = nrEW + 1;
	if (nrEW > MAX_THRESHOLD && (nCrtDirection == DIRECTION_NS) && !nChangeDirectionStarted)
	{
		printf("Trezim planificator for EW- changing direction\n");
		nChangeDirectionStarted = 1;
		SetEvent(hEventPlasator);
	}
	LeaveCriticalSection(&csSync);

	WaitForSingleObject(hEventEW, INFINITE);
	printf("Am intrat in intersectia pe directia EW si am plecat\n");
	EnterCriticalSection(&csSync);
	nrEW = nrEW - 1;
	LeaveCriticalSection(&csSync);
	return 0;
}


DWORD WINAPI Planificator(LPVOID param)
{

	while (1)
	{

		DWORD dwResult = 0;
		dwResult = WaitForSingleObject(hEventPlasator, 100000);

		switch (dwResult)
		{
		case WAIT_OBJECT_0:
		case WAIT_TIMEOUT:
			EnterCriticalSection(&csSync);
			if (!nCrtDirection)
				ResetEvent(hEventNS);
			else
				ResetEvent(hEventEW);
			LeaveCriticalSection(&csSync);
			Sleep(2000);

			EnterCriticalSection(&csSync);
			if (nCrtDirection)
				SetEvent(hEventNS);
			else
				SetEvent(hEventEW);
			nCrtDirection = 1 - nCrtDirection;
			nChangeDirectionStarted = 0;
			LeaveCriticalSection(&csSync);
			break;
		}
	}
}

int main()
{
	HANDLE hThreads[100];
	HANDLE hThread;
	DWORD dwThreadPlanificator;
	srand(GetTickCount());

	InitializeCriticalSection(&csSync);
	hEventNS = CreateEvent(NULL, TRUE, TRUE, NULL);
	hEventEW = CreateEvent(NULL, TRUE, FALSE, NULL);
	hEventPlasator = CreateEvent(NULL, FALSE, FALSE, NULL);
	hThread = CreateThread(NULL, 0, Planificator, NULL, 0, &dwThreadPlanificator);

	for (int i = 0; i < 10; i++)
	{
		if (rand() % 2)
		{
			DWORD dwThreadId = 0;
			for (int j = 0; j < 5; j++)
			{
				CreateThread(NULL, 0, MasinaNordSud, NULL, 0, &dwThreadId);
				Sleep(1000);
			}

		}
		else
		{
			DWORD dwThreadId = 0;
			for (int j = 0; j < 5; j++)
			{
				CreateThread(NULL, 0, MasinaEstVest, NULL, 0, &dwThreadId);
				Sleep(1000);
			}
		}
		Sleep(1000);
	}
	WaitForSingleObject(hThread, INFINITE);
	DeleteCriticalSection(&csSync);
	return 0;
}