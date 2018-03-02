#include <Windows.h>
#include <stdio.h>

HANDLE hThr[2];
HANDLE hMutex;

DWORD WINAPI ThreadFunc(LPVOID param)
{
	int i = *((int *)param);
	WaitForSingleObject(hMutex, INFINITE);

	return 0;
}

int main()
{
	int params[2];
	DWORD dwId = 0;

	hMutex = CreateMutex(NULL, TRUE, NULL);
	//HANDLE h = CreateFile(L"d:\\out2.txt", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, 0, NULL);

	// initialization bla bla
	for (int i = 0; i < 2; i++)
	{
		params[i] = i;
	}

	for (int i = 0; i < 1; i++)
	{
		hThr[i] = CreateThread(NULL, 0, ThreadFunc, &params[i], 0, &dwId);
	}

	WaitForSingleObject(hThr[0] ,INFINITE);	
	return 0;
}