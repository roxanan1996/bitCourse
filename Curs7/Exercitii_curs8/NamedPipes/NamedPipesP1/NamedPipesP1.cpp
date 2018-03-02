// NamedPipesP1.cpp : Defines the entry point for the console application.
//

#include "windows.h"
#include "Message.h"
#include <stdio.h>

LPCWSTR pipeName = L"\\\\.\\pipe\\P1P2Pipe";
int intMessages[5] = { 2, 3, 12, 100, 3 };
wchar_t *stringMessages[5] = { L"ANA", L"MARIA", L"ALIN", L"MIHAI", L"SILVIU" };

int main()
{
	SetEnvironmentVariable(L"PipeName", pipeName);


	printf("pulaaa\n");
	HANDLE pipeHandle = CreateNamedPipe(pipeName, 
		PIPE_ACCESS_OUTBOUND, 
		PIPE_TYPE_MESSAGE | PIPE_WAIT, 
		PIPE_UNLIMITED_INSTANCES, 
		256, 
		256, 
		0, 
		NULL);
	
	wchar_t p2Name[100] = L"NamedPipesP2.exe";
	STARTUPINFO si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi = { 0 };
	BOOL p2Created = CreateProcess(NULL, p2Name, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (!p2Created)
	{
		printf("proces necreat\n");
		CloseHandle(pipeHandle);
		return 0;
	}

	printf("asteapta clienti\n");
	ConnectNamedPipe(pipeHandle, NULL);

	for (int i = 0; i < 10; i++)
	{
		Message msg = { 0 };

		if (i % 2 == 0) // send int message
		{
			msg.messageType = MESSAGE_TYPE_INT;
			msg.intValue = intMessages[i/2];
		}
		else
		{
			msg.messageType = MESSAGE_TYPE_STRING;
			wcscpy_s(msg.stringValue, stringMessages[i / 2]);
		}

		DWORD bytesWritten = 0;

		//printf("scrie scrie cu id = %d\n", i);
		WriteFile(pipeHandle, &msg, sizeof(msg), &bytesWritten, NULL);
	}

	// send stop message

	Message msg = { 0 };
	msg.messageType = MESSAGE_TYPE_STRING;
	wcscpy_s(msg.stringValue, L"STOP");

	DWORD bytesWritten = 0;
	WriteFile(pipeHandle, &msg, sizeof(msg), &bytesWritten, NULL);

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	CloseHandle(pipeHandle);
    return 0;
}

