// NamedPipesP2.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include "windows.h"
#include "..\NamedPipesP1\Message.h"

int main()
{
	wchar_t pipeName[MAX_PATH] = { 0 };
	GetEnvironmentVariable(L"PipeName", pipeName, MAX_PATH);
	wprintf(L"Pipe Name is: %s\n", pipeName);
	HANDLE pipeHandle = CreateFile(pipeName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (pipeHandle == INVALID_HANDLE_VALUE || pipeHandle == NULL) {
		wprintf(L"Cannot connect to pipe name: %s\n", pipeName);
		return 0;
	}
	wprintf(L"Connected to pipe name: %s\n", pipeName);

	int sum = 0;
	wchar_t destinationStr[1000] = { 0 };

	while (1)
	{
		Message msg = { 0 };
		DWORD bytesRead = 0;
		ReadFile(pipeHandle, &msg, sizeof(msg), &bytesRead, NULL);
		if (msg.messageType == MESSAGE_TYPE_INT)
		{
			wprintf(L"Received int message %d\n", msg.intValue);
			sum += msg.intValue;
		}
		else if (msg.messageType == MESSAGE_TYPE_STRING)
		{
			wprintf(L"Received string message: %s\n", msg.stringValue);
			if (wcscmp(msg.stringValue, L"STOP") == 0) 
			{
				wprintf(L"Received STOP message!\n");
				break;
			}
			else 
			{
				wcscat_s(destinationStr, msg.stringValue);
			}
		}
	}
	
	wprintf(L"Sum is %d\n", sum);
	wprintf(L"Destination string is: %s\n", destinationStr);
	CloseHandle(pipeHandle);
    return 0;
}

