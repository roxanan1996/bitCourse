#include <tchar.h>
#include <strsafe.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <queue>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <cstring>
#include "sample.h"

#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "ws2_32.lib")

#define SVCNAME TEXT("RoxService")


std::queue<SOCKET> sockets;
CRITICAL_SECTION crQueue;
CONDITION_VARIABLE QueueNotEmpty;
SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;


/* */
VOID ReportSvcStatus(DWORD dwCurrentState,
	DWORD dwWin32ExitCode,
	DWORD dwWaitHint)
{
	static DWORD dwCheckPoint = 1;

	// Fill in the SERVICE_STATUS structure.

	gSvcStatus.dwCurrentState = dwCurrentState;
	gSvcStatus.dwWin32ExitCode = dwWin32ExitCode;
	gSvcStatus.dwWaitHint = dwWaitHint;

	if (dwCurrentState == SERVICE_START_PENDING)
		gSvcStatus.dwControlsAccepted = 0;
	else gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

	if ((dwCurrentState == SERVICE_RUNNING) ||
		(dwCurrentState == SERVICE_STOPPED))
		gSvcStatus.dwCheckPoint = 0;
	else gSvcStatus.dwCheckPoint = dwCheckPoint++;

	// Report the status of the service to the SCM.
	SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
}

/*
** Logs messages to the event log
*/

VOID SvcReportEvent(LPTSTR szFunction) {
	HANDLE hEventSource;
	LPCTSTR lpszStrings[2];
	TCHAR Buffer[80];

	hEventSource = RegisterEventSource(NULL, SVCNAME);

	if (NULL != hEventSource)
	{
		StringCchPrintf(Buffer, 80, TEXT("%s failed with %d"), szFunction, GetLastError());

		lpszStrings[0] = SVCNAME;
		lpszStrings[1] = Buffer;

		ReportEvent(hEventSource,        // event log handle
			EVENTLOG_ERROR_TYPE, // event type
			0,                   // event category
			SVC_ERROR,           // event identifier
			NULL,                // no security identifier
			2,                   // size of lpszStrings array
			0,                   // no binary data
			lpszStrings,         // array of strings
			NULL);               // no binary data

		DeregisterEventSource(hEventSource);
	}
}

/*
** Called by SCM whenever a control code is sent to the service
*/

VOID WINAPI SvcCtrlHandler(DWORD dwCtrl) {

	switch (dwCtrl) {
	case SERVICE_CONTROL_STOP:
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
		SetEvent(ghSvcStopEvent);
		ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
		return;
	case SERVICE_CONTROL_INTERROGATE:
		break;
	default:
		break;
	}
}

/*
** Installs a service in the SCM database
*/

VOID SvcInstall() {
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	TCHAR szPath[MAX_PATH];

	if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
		printf("Cannot install service (%d)\n", GetLastError());
		return;
	}

	/* Obtain a handle to the SCM database */

	schSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == schSCManager) {
		printf("Open SCMANAGER failed (%d)\n", GetLastError());
		return;
	}

	/* Create the service */

	schService = CreateService(
		schSCManager,
		SVCNAME,
		SVCNAME,
		SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		szPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	if (NULL == schService) {
		printf("Create Service Failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	printf("Service installed succesfully\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);

}

/*
** Delete a service from SCM database
*/

VOID _stdcall DoDeleteSvc() {

	SC_HANDLE schSCManager;
	SC_HANDLE schService;

	/* Get a handle to the SCM database */

	schSCManager = OpenSCManager(
		NULL,
		NULL,
		SC_MANAGER_ALL_ACCESS);

	if (NULL == schSCManager) {
		printf("Open SCMANAGER failed (%d)\n", GetLastError());
		return;
	}

	/* get a handle to the service */

	schService = OpenService(
		schSCManager,
		SVCNAME,
		DELETE);

	if (NULL == schService) {
		printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	/* delete the service :D */

	if (!DeleteService(schService)) {
		printf("DeleteService failed (%d)\n", GetLastError());
	}

	printf("Service deleted succesfully\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

//
// Purpose
// Threads Code
// Parameters: None
// Return Value: 0
DWORD WINAPI MyFunc(LPVOID lpParam) {
	while (1) {
		//Sleep(1000);

		EnterCriticalSection(&crQueue);
		while (sockets.empty()) {
			//EnterCriticalSection(&priSync);
			//printf("waiting thread nb %d\n", GetCurrentThreadId());
			//LeaveCriticalSection(&priSync);
			DWORD wait = WaitForSingleObject(ghSvcStopEvent, 0);
			printf("astept\n");
			if (wait == WAIT_OBJECT_0) {
				//SetEvent(hEvent);
				printf("ies\n");
				break;
			}
			SleepConditionVariableCS(&QueueNotEmpty, &crQueue, 5000);
			//break;
		}
		if (!sockets.empty()) {
			char url[100];
			SOCKET acceptSocket = sockets.front();
			sockets.pop();
			LeaveCriticalSection(&crQueue);

			int recvRes = recv(acceptSocket, url, 100, 0);

			if (recvRes == SOCKET_ERROR) {
				wprintf(L"ERROR AT RECEIVING URL MESSAGE  %d\n\n", WSAGetLastError());
				return -2;
			}

			//printf("%s\n\n\n\n", url);

			// aflare nume fisier html :)
			char *token;
			token = strtok(url, "/");
			token = strtok(NULL, "/");
			token = strtok(token, " ");

			if (token != NULL) {
				printf("s a trimis%slala\n", token);


				// deschidere fisier si citire tot in message[]
				HANDLE hFileConfig = CreateFile(L"E:\\Bitdefender\\Curs9\\LastHomework\\Debug\\httpservice.config", GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
					NULL, OPEN_EXISTING, 0, NULL);
				if (hFileConfig == INVALID_HANDLE_VALUE) {
					wprintf(L"Nu a putut fi deschis fisierul httpServiceConfig lasteroor :: %d\n", GetLastError());
					return -1;
				}

				BOOL bRead = FALSE;
				DWORD dwRead = 0;
				char pathToFile[100];

				bRead = ReadFile(hFileConfig, (LPVOID)pathToFile, 51, &dwRead, NULL);
				if (!bRead) {
					printf("EROARE LA CITIRE DIN CONFIG\n");
					return -1;
				}

				CloseHandle(hFileConfig);
				//printf("%d", dwRead);

				pathToFile[dwRead] = '\0';
				//strcat(pathToFile, '\0');
				//printf("lala %s\n", pathToFile);


				//wprintf(L"%s\n\n", confText);

				//std::wcout << L"confText pur: " << confText << L"\n";
				//wchar_t pathToFile[100];
				//wcscpy(pathToFile, confText);

				//wprintf(L"doar calea %s\n", pathToFile);
				//std::wcout << L"doar calea " << pathToFile << L"\n";
				strcat(pathToFile, token);
				strcat(pathToFile, ".txt");

				//wprintf(L"\n\ncalea cat\re html :D\n\n");
				//std::cout << "bla" << pathToFile << "caca\n\n";

				size_t size = strlen(pathToFile);

				//printf("%d", size);
				wchar_t* wArr = new wchar_t[size];
				for (size_t i = 0; i < size; ++i) {
					wArr[i] = pathToFile[i];
				}

				wArr[size] = '\0';
				//printf("%d\n", wcslen(wArr));
				//wprintf(L"%s\n", wArr);

				//std::wcout << L"a message" << wArr << L"\n";
				HANDLE hHtml = CreateFile(wArr, GENERIC_READ,
					FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
					NULL, OPEN_EXISTING, 0, NULL);

				if (hHtml == INVALID_HANDLE_VALUE) {
					wprintf(L"Nu a putut fi deschis html %d\n", GetLastError());
					return -1;
				}

				char message[1000];// = "HTTP/1.1 200 Okay\r\nContent-Type: text/html; charset=ISO-8859-4 \r\n\r\n<h1>Hello, client! Welcome to the Virtual Machine Web..</h1>";
				BOOL htmlRead = FALSE;
				htmlRead = ReadFile(hHtml, message, 1000, &dwRead, NULL);

				if (!htmlRead) {
					printf("EROARE LA CITIRE DIN html\n");
					return -1;
				}

				CloseHandle(hHtml);
				message[dwRead] = '\0';
				// printf("mesajul %s\n", message);
				int length = strlen(message) + 1; // Plus 1 for  null terminator
				int send_res = send(acceptSocket, message, length, 0); // Flag = 0
				Sleep(5000);
				if (send_res == SOCKET_ERROR) {
					wprintf(L"ERROR AT SENDIND MESSAGE  %d\n\n", WSAGetLastError());
					return -2;
				}

				/*
				char message[] = "HTTP / 1.1 200 Okay\r\nContent - Type: text / html; charset = ISO - 8859 - 4 \r\n\r\n<h1>Hello, client!Welcome to the Virtual Machine Web..< / h1>";
				int length = strlen(message) + 1; // Plus 1 for  null terminator
				int send_res = send(acceptSocket, message, length, 0); // Flag = 0
				Sleep(5000);
				if (send_res == SOCKET_ERROR) {
				wprintf(L"ERROR AT SENDIND MESSAGE  %d\n\n", WSAGetLastError());
				return -2;
				}
				*/
				printf("S A TRIIIIMIS \n");
			}
			closesocket(acceptSocket);

		}
		else {
			LeaveCriticalSection(&crQueue);
			break;
		}
	}
		
	return 0;
}

/*
** The heart of the service
*/

VOID SvcInit(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// TO_DO: Declare and set any required variables.
	//   Be sure to periodically call ReportSvcStatus() with 
	//   SERVICE_START_PENDING. If initialization fails, call
	//   ReportSvcStatus with SERVICE_STOPPED.

	InitializeCriticalSection(&crQueue);
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return;
	}

	/* Confirm that the WinSock DLL supports 2.2.*/
	/* Note that if the DLL supports versions greater    */
	/* than 2.2 in addition to 2.2, it will still return */
	/* 2.2 in wVersion since that is the version we      */
	/* requested.                                        */

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		printf("Could not find a usable version of Winsock.dll\n");
		WSACleanup();
		return;
	}
	else
		printf("The Winsock 2.2 dll was found okay\n");


	/* The Winsock DLL is acceptable. Proceed to use it. */

	/* Add network programming using Winsock here */


	/* Create a socket for server :D */
	SOCKET sock = INVALID_SOCKET;
	int iFamily = AF_INET;
	int iType = SOCK_STREAM;
	int iProtocol = IPPROTO_TCP;

	sock = socket(iFamily, iType, iProtocol);
	if (sock == INVALID_SOCKET) {
		wprintf(L"socket function failed with error = %d\n", WSAGetLastError());
		return;
	}
	else {
		wprintf(L"socket function succeeded\n");
	}

	/* the socket address to pe passed to bind */
	sockaddr_in service;
	// The sockaddr_in structure specifies the address family,
	// IP address, and port for the socket that is being bound.
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr("127.0.0.1");
	service.sin_port = htons(80);

	// bind the socket
	int bindResult = bind(sock, (SOCKADDR*)&service, sizeof(service));

	if (bindResult == SOCKET_ERROR) {
		wprintf(L"bind failed with error %u\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return;
	}
	else
		wprintf(L"bind returned success\n");

	/* listen */

	int listenRes = listen(sock, SOMAXCONN);

	if (listenRes) {
		wprintf(L"EROARE LA LISTENING  %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return;
	}

	SOCKET acceptSocket;

	printf("WAITING FOR A CLIENT TO CONNECT :D \n\n");

	HANDLE dwHandles[4];
	DWORD dwThreadId[4];

	for (int i = 0; i < 4; ++i) {
		dwHandles[i] = CreateThread(NULL, 0, MyFunc, NULL, 0, &dwThreadId[i]);
	}


	// Create an event. The control handler function, SvcCtrlHandler,
	// signals this event when it receives the stop control code.

	ghSvcStopEvent = CreateEvent(
		NULL,    // default security attributes
		TRUE,    // manual reset event
		FALSE,   // not signaled
		NULL);   // no name

	if (ghSvcStopEvent == NULL)
	{
		ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
		return;
	}

	// Report running status when initialization is complete.

	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

	// TO_DO: Perform work until service stops.

	while (1) {

		acceptSocket = accept(sock, NULL, NULL);

		if (acceptSocket == INVALID_SOCKET) {
			wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
			closesocket(sock);
			WSACleanup();
			return;
		}


		wprintf(L"Client connected.\n\n");

		EnterCriticalSection(&crQueue);
		sockets.push(acceptSocket);
		WakeConditionVariable(&QueueNotEmpty);
		LeaveCriticalSection(&crQueue);

		// Check whether to stop the service.

		DWORD wait = WaitForSingleObject(ghSvcStopEvent, 1000);
		if (wait == WAIT_OBJECT_0) {
			break;
		}
	}

	WaitForMultipleObjects(4, dwHandles, TRUE, 3000);
	ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

/*
** Entry point for the service
*/

VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR *lpszArgv) {
	gSvcStatusHandle = RegisterServiceCtrlHandler(
		SVCNAME,
		SvcCtrlHandler);

	if (!gSvcStatusHandle) {
		SvcReportEvent(TEXT("RegisterServiceCtrlHandler"));
		return;
	}

	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;

	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	SvcInit(dwArgc, lpszArgv);
}
//
// Purpose: 
//   Starts the service if possible.
//
// Parameters:
//   None
// 
// Return value:
//   None
//
VOID __stdcall DoStartSvc()
{
	SERVICE_STATUS_PROCESS ssStatus;
	DWORD dwOldCheckPoint;
	DWORD dwStartTickCount;
	DWORD dwWaitTime;
	DWORD dwBytesNeeded;

	// Get a handle to the SCM database. 

	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // servicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the service.

	SC_HANDLE schService = OpenService(
		schSCManager,         // SCM database 
		SVCNAME,            // name of service 
		SERVICE_ALL_ACCESS);  // full access 

	if (schService == NULL)
	{
		printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	// Check the status in case the service is not stopped. 

	if (!QueryServiceStatusEx(
		schService,                     // handle to service 
		SC_STATUS_PROCESS_INFO,         // information level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded))              // size needed if buffer is too small
	{
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}

	// Check if the service is already running. It would be possible 
	// to stop the service here, but for simplicity this example just returns. 

	if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING)
	{
		printf("Cannot start the service because it is already running\n");
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	// Wait for the service to stop before attempting to start it.

	while (ssStatus.dwCurrentState == SERVICE_STOP_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds. 

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status until the service is no longer stop pending. 

		if (!QueryServiceStatusEx(
			schService,                     // handle to service 
			SC_STATUS_PROCESS_INFO,         // information level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded))              // size needed if buffer is too small
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			CloseServiceHandle(schService);
			CloseServiceHandle(schSCManager);
			return;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// Continue to wait and check.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				printf("Timeout waiting for service to stop\n");
				CloseServiceHandle(schService);
				CloseServiceHandle(schSCManager);
				return;
			}
		}
	}

	// Attempt to start the service.

	if (!StartService(
		schService,  // handle to service 
		0,           // number of arguments 
		NULL))      // no arguments 
	{
		printf("StartService failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}
	else printf("Service start pending...\n");

	// Check the status until the service is no longer start pending. 

	if (!QueryServiceStatusEx(
		schService,                     // handle to service 
		SC_STATUS_PROCESS_INFO,         // info level
		(LPBYTE)&ssStatus,             // address of structure
		sizeof(SERVICE_STATUS_PROCESS), // size of structure
		&dwBytesNeeded))              // if buffer too small
	{
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return;
	}

	// Save the tick count and initial checkpoint.

	dwStartTickCount = GetTickCount();
	dwOldCheckPoint = ssStatus.dwCheckPoint;

	while (ssStatus.dwCurrentState == SERVICE_START_PENDING)
	{
		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth the wait hint, but no less than 1 second and no 
		// more than 10 seconds. 

		dwWaitTime = ssStatus.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		// Check the status again. 

		if (!QueryServiceStatusEx(
			schService,             // handle to service 
			SC_STATUS_PROCESS_INFO, // info level
			(LPBYTE)&ssStatus,             // address of structure
			sizeof(SERVICE_STATUS_PROCESS), // size of structure
			&dwBytesNeeded))              // if buffer too small
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			break;
		}

		if (ssStatus.dwCheckPoint > dwOldCheckPoint)
		{
			// Continue to wait and check.

			dwStartTickCount = GetTickCount();
			dwOldCheckPoint = ssStatus.dwCheckPoint;
		}
		else
		{
			if (GetTickCount() - dwStartTickCount > ssStatus.dwWaitHint)
			{
				// No progress made within the wait hint.
				break;
			}
		}
	}

	// Determine whether the service is running.

	if (ssStatus.dwCurrentState == SERVICE_RUNNING)
	{
		printf("Service started successfully.\n");
	}
	else
	{
		printf("Service not started. \n");
		printf("  Current State: %d\n", ssStatus.dwCurrentState);
		printf("  Exit Code: %d\n", ssStatus.dwWin32ExitCode);
		printf("  Check Point: %d\n", ssStatus.dwCheckPoint);
		printf("  Wait Hint: %d\n", ssStatus.dwWaitHint);
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}


BOOL __stdcall StopDependentServices(SC_HANDLE schService, SC_HANDLE schSCManager)
{
	DWORD i;
	DWORD dwBytesNeeded;
	DWORD dwCount;

	LPENUM_SERVICE_STATUS   lpDependencies = NULL;
	ENUM_SERVICE_STATUS     ess;
	SC_HANDLE               hDepService;
	SERVICE_STATUS_PROCESS  ssp;

	DWORD dwStartTime = GetTickCount();
	DWORD dwTimeout = 30000; // 30-second time-out

							 // Pass a zero-length buffer to get the required buffer size.
	if (EnumDependentServices(schService, SERVICE_ACTIVE,
		lpDependencies, 0, &dwBytesNeeded, &dwCount))
	{
		// If the Enum call succeeds, then there are no dependent
		// services, so do nothing.
		return TRUE;
	}
	else
	{
		if (GetLastError() != ERROR_MORE_DATA)
			return FALSE; // Unexpected error

						  // Allocate a buffer for the dependencies.
		lpDependencies = (LPENUM_SERVICE_STATUS)HeapAlloc(
			GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytesNeeded);

		if (!lpDependencies)
			return FALSE;

		__try {
			// Enumerate the dependencies.
			if (!EnumDependentServices(schService, SERVICE_ACTIVE,
				lpDependencies, dwBytesNeeded, &dwBytesNeeded,
				&dwCount))
				return FALSE;

			for (i = 0; i < dwCount; i++)
			{
				ess = *(lpDependencies + i);
				// Open the service.
				hDepService = OpenService(schSCManager,
					ess.lpServiceName,
					SERVICE_STOP | SERVICE_QUERY_STATUS);

				if (!hDepService)
					return FALSE;

				__try {
					// Send a stop code.
					if (!ControlService(hDepService,
						SERVICE_CONTROL_STOP,
						(LPSERVICE_STATUS)&ssp))
						return FALSE;

					// Wait for the service to stop.
					while (ssp.dwCurrentState != SERVICE_STOPPED)
					{
						Sleep(ssp.dwWaitHint);
						if (!QueryServiceStatusEx(
							hDepService,
							SC_STATUS_PROCESS_INFO,
							(LPBYTE)&ssp,
							sizeof(SERVICE_STATUS_PROCESS),
							&dwBytesNeeded))
							return FALSE;

						if (ssp.dwCurrentState == SERVICE_STOPPED)
							break;

						if (GetTickCount() - dwStartTime > dwTimeout)
							return FALSE;
					}
				}
				__finally
				{
					// Always release the service handle.
					CloseServiceHandle(hDepService);
				}
			}
		}
		__finally
		{
			// Always free the enumeration buffer.
			HeapFree(GetProcessHeap(), 0, lpDependencies);
		}
	}
	return TRUE;
}

//
// Purpose: 
//   Stops the service.
//
// Parameters:
//   None
// 
// Return value:
//   None
//
VOID __stdcall DoStopSvc()
{
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwStartTime = GetTickCount();
	DWORD dwBytesNeeded;
	DWORD dwTimeout = 30000; // 30-second time-out
	DWORD dwWaitTime;

	// Get a handle to the SCM database. 

	SetEvent(ghSvcStopEvent);

	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the service.

	SC_HANDLE schService = OpenService(
		schSCManager,         // SCM database 
		SVCNAME,            // name of service 
		SERVICE_STOP |
		SERVICE_QUERY_STATUS |
		SERVICE_ENUMERATE_DEPENDENTS);

	if (schService == NULL)
	{
		printf("OpenService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}

	// Make sure the service is not already stopped.

	if (!QueryServiceStatusEx(
		schService,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded))
	{
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		goto stop_cleanup;
	}

	if (ssp.dwCurrentState == SERVICE_STOPPED)
	{
		printf("Service is already stopped.\n");
		goto stop_cleanup;
	}

	// If a stop is pending, wait for it.

	while (ssp.dwCurrentState == SERVICE_STOP_PENDING)
	{
		printf("Service stop pending...\n");

		// Do not wait longer than the wait hint. A good interval is 
		// one-tenth of the wait hint but not less than 1 second  
		// and not more than 10 seconds. 

		dwWaitTime = ssp.dwWaitHint / 10;

		if (dwWaitTime < 1000)
			dwWaitTime = 1000;
		else if (dwWaitTime > 10000)
			dwWaitTime = 10000;

		Sleep(dwWaitTime);

		if (!QueryServiceStatusEx(
			schService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			goto stop_cleanup;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
		{
			printf("Service stopped successfully.\n");
			goto stop_cleanup;
		}

		if (GetTickCount() - dwStartTime > dwTimeout)
		{
			printf("Service stop timed out.\n");
			goto stop_cleanup;
		}
	}

	// If the service is running, dependencies must be stopped first.

	//StopDependentServices(schService, schSCManager);

	// Send a stop code to the service.

	if (!ControlService(
		schService,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp))
	{
		printf("ControlService failed (%d)\n", GetLastError());
		goto stop_cleanup;
	}

	// Wait for the service to stop.

	while (ssp.dwCurrentState != SERVICE_STOPPED)
	{
		Sleep(ssp.dwWaitHint);
		if (!QueryServiceStatusEx(
			schService,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded))
		{
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			goto stop_cleanup;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
			break;

		if (GetTickCount() - dwStartTime > dwTimeout)
		{
			printf("Wait timed out\n");
			goto stop_cleanup;
		}
	}
	printf("Service stopped successfully\n");

stop_cleanup:
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}


/*
** Entry point for the process
*/
VOID _cdecl _tmain(int argc, TCHAR* argv[]) {

	SERVICE_TABLE_ENTRY DispatchTable[] = {
		{ SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
		{ NULL, NULL }
	};

	if (!StartServiceCtrlDispatcher(DispatchTable)) {
		SvcReportEvent(TEXT("StartServiceCtrDispatcher"));
	}

	if (!lstrcmpi(argv[1], TEXT("install"))) {
		SvcInstall();
	}
	else if (!lstrcmpi(argv[1], TEXT("uninstall"))) {
		DoDeleteSvc();
	}
	else if (!lstrcmpi(argv[1], TEXT("start"))) {
		DoStartSvc();
	}
	else if (lstrcmpi(argv[1], TEXT("stop"))) {
		DoStopSvc();
	}

}






