// SharedMemoryP2.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include "windows.h"
#include "..\SharedMemoryP1\Message.h"

int main()
{
	wchar_t sharedMemName[MAX_PATH] = { 0 };
	wchar_t writeEventName[MAX_PATH] = { 0 };
	wchar_t readEventName[MAX_PATH] = { 0 };
	GetEnvironmentVariable(L"SharedMemoryPageName", sharedMemName, MAX_PATH);
	GetEnvironmentVariable(L"WriteEventName", writeEventName, MAX_PATH);
	GetEnvironmentVariable(L"ReadEventName", readEventName, MAX_PATH);

	wprintf(L"P2: Shared Memory page name: %s\n", sharedMemName);
	wprintf(L"P2: Write Event Object name: %s\n", writeEventName);
	wprintf(L"P2: Read Event Object name: %s\n", writeEventName);

	HANDLE fileMapHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS, NULL, sharedMemName);

	if (fileMapHandle == NULL)
	{
		int err = GetLastError();
		wprintf(L"P2: Cannot open file mapping. Error %d\n", err);
		return 0;
	}

	LPVOID buf;

	buf = MapViewOfFile(fileMapHandle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Message));

	if (buf == NULL)
	{
		int err = GetLastError();
		wprintf(L"P2: Cannot create map view of file. Error %d\n", err);
		CloseHandle(fileMapHandle);
		return 0;
	}

	HANDLE readEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, readEventName);
	HANDLE writeEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, writeEventName);

	int sum = 0;
	wchar_t destinationStr[1000] = { 0 };
	
	while (1)
	{
		WaitForSingleObject(writeEvent, INFINITE);
		Message msg = { 0 };
		CopyMemory(&msg, buf, sizeof(msg));
		SetEvent(readEvent);
		if (msg.messageType == MESSAGE_TYPE_INT)
		{
			wprintf(L"P2: Received int message %d\n", msg.intValue);
			sum += msg.intValue;
		}
		else if (msg.messageType == MESSAGE_TYPE_STRING)
		{
			wprintf(L"P2: Received string message: %s\n", msg.stringValue);
			if (wcscmp(msg.stringValue, L"STOP") == 0)
			{
				wprintf(L"P2: Received STOP message!\n");
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

	UnmapViewOfFile(buf);

	CloseHandle(readEvent);
	CloseHandle(writeEvent);
	CloseHandle(fileMapHandle);
    return 0;
}

