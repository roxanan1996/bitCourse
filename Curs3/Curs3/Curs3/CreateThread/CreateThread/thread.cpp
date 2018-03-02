
#include <Windows.h>
#include <stdio.h>

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	printf("Thread\n");
	return 0;
}

int main()
{
	HANDLE hThr[2];
	DWORD dwThreadId[2];
	for (int i = 0 ; i < 2 ; i++)
		hThr[i] = CreateThread(NULL, 0, ThreadProc, NULL, 0, &dwThreadId[i]);
	WaitForMultipleObjects(2, hThr, TRUE, INFINITE);
	printf("main\n");

	return 0;
}