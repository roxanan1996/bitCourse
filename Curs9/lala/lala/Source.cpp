
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

#define SVCNAME TEXT("SvcName")

std::queue<SOCKET> sockets;
CRITICAL_SECTION crQueue;
CONDITION_VARIABLE QueueNotEmpty;
SERVICE_STATUS          gSvcStatus;
SERVICE_STATUS_HANDLE   gSvcStatusHandle;
HANDLE                  ghSvcStopEvent = NULL;

VOID SvcInstall(void);
VOID WINAPI SvcCtrlHandler(DWORD);
VOID WINAPI SvcMain(DWORD, LPTSTR *);

VOID ReportSvcStatus(DWORD, DWORD, DWORD);
VOID SvcInit(DWORD, LPTSTR *);
VOID SvcReportEvent(LPTSTR);


//
// Purpose: 
//   Entry point for the process
//
// Parameters:
//   None
// 
// Return value:
//   None
//
void __cdecl _tmain(int argc, TCHAR *argv[])
{
	// If command-line parameter is "install", install the service. 
	// Otherwise, the service is probably being started by the SCM.

	if (lstrcmpi(argv[1], TEXT("install")) == 0)
	{
		SvcInstall();
		return;
	}

	// TO_DO: Add any additional services for the process to this table.
	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{ SVCNAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
		{ NULL, NULL }
	};

	// This call returns when the service has stopped. 
	// The process should simply terminate when the call returns.

	if (!StartServiceCtrlDispatcher(DispatchTable))
	{
		SvcReportEvent(TEXT("StartServiceCtrlDispatcher"));
	}
}

//
// Purpose: 
//   Installs a service in the SCM database
//
// Parameters:
//   None
// 
// Return value:
//   None
//
VOID SvcInstall()
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	TCHAR szPath[MAX_PATH];

	if (!GetModuleFileName(NULL, szPath, MAX_PATH))
	{
		printf("Cannot install service (%d)\n", GetLastError());
		return;
	}

	// Get a handle to the SCM database. 

	schSCManager = OpenSCManager(
		NULL,                    // local computer
		NULL,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager)
	{
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return;
	}

	// Create the service

	schService = CreateService(
		schSCManager,              // SCM database 
		SVCNAME,                   // name of service 
		SVCNAME,                   // service name to display 
		SERVICE_ALL_ACCESS,        // desired access 
		SERVICE_WIN32_OWN_PROCESS, // service type 
		SERVICE_DEMAND_START,      // start type 
		SERVICE_ERROR_NORMAL,      // error control type 
		szPath,                    // path to service's binary 
		NULL,                      // no load ordering group 
		NULL,                      // no tag identifier 
		NULL,                      // no dependencies 
		NULL,                      // LocalSystem account 
		NULL);                     // no password 

	if (schService == NULL)
	{
		printf("CreateService failed (%d)\n", GetLastError());
		CloseServiceHandle(schSCManager);
		return;
	}
	else printf("Service installed successfully\n");

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
}

//
// Purpose: 
//   Entry point for the service
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None.
//
VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	// Register the handler function for the service

	gSvcStatusHandle = RegisterServiceCtrlHandler(
		SVCNAME,
		SvcCtrlHandler);

	if (!gSvcStatusHandle)
	{
		SvcReportEvent(TEXT("RegisterServiceCtrlHandler"));
		return;
	}

	// These SERVICE_STATUS members remain as set here

	gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	gSvcStatus.dwServiceSpecificExitCode = 0;

	// Report initial status to the SCM

	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	// Perform service-specific initialization and work.

	SvcInit(dwArgc, lpszArgv);
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
			SleepConditionVariableCS(&QueueNotEmpty, &crQueue, 5000);
			//break;
		}

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
	return 0;
}

//
// Purpose: 
//   The service code
//
// Parameters:
//   dwArgc - Number of arguments in the lpszArgv array
//   lpszArgv - Array of strings. The first string is the name of
//     the service and subsequent strings are passed by the process
//     that called the StartService function to start the service.
// 
// Return value:
//   None
//
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

	/*WaitForMultipleObjects(4, dwHandles, TRUE, 10000);
	ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
	*/
	//WaitForMultipleObjects(4, dwHandles, TRUE, 10000);

	// do the work here
	return;
}

//
// Purpose: 
//   Sets the current service status and reports it to the SCM.
//
// Parameters:
//   dwCurrentState - The current state (see SERVICE_STATUS)
//   dwWin32ExitCode - The system error code
//   dwWaitHint - Estimated time for pending operation, 
//     in milliseconds
// 
// Return value:
//   None
//
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

//
// Purpose: 
//   Called by SCM whenever a control code is sent to the service
//   using the ControlService function.
//
// Parameters:
//   dwCtrl - control code
// 
// Return value:
//   None
//
VOID WINAPI SvcCtrlHandler(DWORD dwCtrl)
{
	// Handle the requested control code. 

	switch (dwCtrl)
	{
	case SERVICE_CONTROL_STOP:
		ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

		// Signal the service to stop.

		SetEvent(ghSvcStopEvent);
		ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);

		return;

	case SERVICE_CONTROL_INTERROGATE:
		break;

	default:
		break;
	}

}

//
// Purpose: 
//   Logs messages to the event log
//
// Parameters:
//   szFunction - name of function that failed
// 
// Return value:
//   None
//
// Remarks:
//   The service must have an entry in the Application event log.
//
VOID SvcReportEvent(LPTSTR szFunction)
{
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