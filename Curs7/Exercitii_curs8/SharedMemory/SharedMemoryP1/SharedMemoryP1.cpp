// SharedMemoryP1.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include "Windows.h"
#include "Message.h"

LPCWSTR sharedMemName = L"Local\\P1P2SharedMemory";
LPCWSTR writeEventName = L"Local\\P1P2writeEvent";
LPCWSTR readEventName = L"Local\\P1P2readEvent";
int intMessages[5] = { 2, 3, 12, 100, 3 };
wchar_t *stringMessages[5] = { L"ANA", L"MARIA", L"ALIN", L"MIHAI", L"SILVIU" };

int main()
{
	LPVOID buf;

	SetEnvironmentVariable(L"SharedMemoryPageName", sharedMemName);
	SetEnvironmentVariable(L"WriteEventName", writeEventName);
	SetEnvironmentVariable(L"ReadEventName", readEventName);

	
	HANDLE fileMapHandle = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Message), sharedMemName);
	if (fileMapHandle == NULL)
	{
		int err = GetLastError();
		wprintf(L"P1: Cannot create file mapping. Error %d\n", err);
		return 0;
	}

	buf = MapViewOfFile(fileMapHandle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Message));

	if (buf == NULL) 
	{
		int err = GetLastError();
		wprintf(L"P1: Cannot create map view of file. Error %d\n", err);
		CloseHandle(fileMapHandle);
		return 0;
	}

	HANDLE writeEvent = CreateEvent(NULL, FALSE, FALSE, writeEventName);
	HANDLE readEvent = CreateEvent(NULL, FALSE, FALSE, readEventName);

	wchar_t p2Name[100] = L"SharedMemoryP2.exe";
	STARTUPINFO si = { 0 };
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi = { 0 };
	BOOL p2Created = CreateProcess(NULL, p2Name, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (!p2Created)
	{
		int err = GetLastError();
		wprintf(L"P1: Cannot create process SharedMemoryP2.exe. Error %d\n", err);
		CloseHandle(fileMapHandle);
		return 0;
	}

	for (int i = 0; i < 10; i++)
	{
		Message msg = { 0 };

		if (i % 2 == 0) // send int message
		{
			msg.messageType = MESSAGE_TYPE_INT;
			msg.intValue = intMessages[i / 2];
		}
		else
		{
			msg.messageType = MESSAGE_TYPE_STRING;
			wcscpy_s(msg.stringValue, stringMessages[i / 2]);
		}

		CopyMemory(buf, &msg, sizeof(msg));
		SetEvent(writeEvent);
		WaitForSingleObject(readEvent, INFINITE);
	}

	// Send stop message
	Message msg = { 0 };

	msg.messageType = MESSAGE_TYPE_STRING;
	wcscpy_s(msg.stringValue, L"STOP");

	CopyMemory(buf, &msg, sizeof(msg));
	SetEvent(writeEvent);

	WaitForSingleObject(pi.hProcess, INFINITE);

	CloseHandle(readEvent);
	CloseHandle(writeEvent);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	UnmapViewOfFile(buf);
	CloseHandle(fileMapHandle);
    return 0;
}

