#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <strsafe.h>

#define BUFSIZE 4096
int _tmain() {
	/*STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	TCHAR block[100];
	LPTSTR var = (LPTSTR)block;
	if (FAILED(StringCchCopy(var, 100, TEXT("MySetting=A"))))
	{
		printf("String copy failed\n");
		return -1;
	}

	var += lstrlen(var) + 1;
	if (FAILED(StringCchCopy(var, 100, TEXT("MyVersion=2"))))
	{
		printf("String copy failed\n");
		return -1;
	}

	// Terminate the block with a NULL byte. 

	var += lstrlen(var) + 1;
	*var = (TCHAR)0;
	
	var = (LPTSTR)block;
	while (*var) {
		_tprintf(TEXT("var = %s\n"), var);
		var += lstrlen(var) + 1;
	}
	*/

	printf("ullaa\n");
	TCHAR chNewEnv[BUFSIZE];
	LPTSTR lpszCurrentVariable;
	DWORD dwFlags = 0;
	TCHAR szAppName[] = TEXT("ex3.exe");
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	BOOL fSuccess;

	// Copy environment strings into an environment block. 

	lpszCurrentVariable = (LPTSTR)chNewEnv;
	if (FAILED(StringCchCopy(lpszCurrentVariable, BUFSIZE, TEXT("MySetting=A"))))
	{
		printf("String copy failed\n");
		return FALSE;
	}

	lpszCurrentVariable += lstrlen(lpszCurrentVariable) + 1;
	if (FAILED(StringCchCopy(lpszCurrentVariable, BUFSIZE, TEXT("MyVersion=2"))))
	{
		printf("String copy failed\n");
		return FALSE;
	}

	// Terminate the block with a NULL byte. 

	lpszCurrentVariable += lstrlen(lpszCurrentVariable) + 1;
	*lpszCurrentVariable = (TCHAR)0;

	// Create the child process, specifying a new environment block. 

	SecureZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);

#ifdef UNICODE
	dwFlags = CREATE_UNICODE_ENVIRONMENT;
#endif
	printf("cream proces\n");
	bool isCreated = CreateProcess(L"E:\\Bitdefender\\Curs5\\curs 5\\Environment\\Debug\\Environment.exe",
		NULL,
		NULL,
		NULL,
		TRUE,
		dwFlags,
		(LPVOID)chNewEnv,
		//NULL,
		NULL,
		&si,
		&pi);
	
	if (isCreated)
		printf("Proces creat!\n");
	else
		printf("Eroare la crearea procesului!(%d)\n", GetLastError());

	WaitForSingleObject(pi.hProcess, INFINITE);
	return 0;
}