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
//#define __STDC_WANT_LIB_EXT2__ 1
// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")
std::queue<SOCKET> sockets;
CRITICAL_SECTION crQueue;

CONDITION_VARIABLE QueueNotEmpty;

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

int __cdecl main()
{

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
		return 1;
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
		return 1;
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
		return -1;
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
		return 1;
	}
	else
		wprintf(L"bind returned success\n");

	/* listen */

	int listenRes = listen(sock, SOMAXCONN);

	if (listenRes) {
		wprintf(L"EROARE LA LISTENING  %d\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return -1;
	}

	SOCKET acceptSocket;

	printf("WAITING FOR A CLIENT TO CONNECT :D \n\n");

	HANDLE dwHandles[4];
	DWORD dwThreadId[4];

	for (int i = 0; i < 4; ++i) {
		dwHandles[i] = CreateThread(NULL, 0, MyFunc, NULL, 0, &dwThreadId[i]);
	}
	
	while (1) {
		acceptSocket = accept(sock, NULL, NULL);

		if (acceptSocket == INVALID_SOCKET) {
			wprintf(L"accept failed with error: %ld\n", WSAGetLastError());
			closesocket(sock);
			WSACleanup();
			return 1;
		}


		wprintf(L"Client connected.\n\n");

		EnterCriticalSection(&crQueue);
		sockets.push(acceptSocket);
		WakeConditionVariable(&QueueNotEmpty);
		LeaveCriticalSection(&crQueue);

		Sleep(1000);

	}


	/*
		char url[100];

		int recvRes = recv(acceptSocket, url, 100, 0);

		if (recvRes == SOCKET_ERROR) {
			wprintf(L"ERROR AT RECEIVING URL MESSAGE  %d\n\n", WSAGetLastError());
			return -2;
		}

		printf("%s\n\n\n\n", url);

		// aflare nume fisier html :)
		char *token;
		token = strtok(url, "/");
		token = strtok(NULL, "/");
		token = strtok(token, " ");
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

		bRead = ReadFile(hFileConfig, (LPVOID) pathToFile, 51, &dwRead, NULL);
		if (!bRead) {
			printf("EROARE LA CITIRE DIN CONFIG\n");
			return - 1;
		}

		printf("%d", dwRead);

		pathToFile[dwRead] = '\0';
		//strcat(pathToFile, '\0');
		printf("lala %s\n", pathToFile);

		
		//wprintf(L"%s\n\n", confText);

		//std::wcout << L"confText pur: " << confText << L"\n";
		//wchar_t pathToFile[100];
		//wcscpy(pathToFile, confText);

		//wprintf(L"doar calea %s\n", pathToFile);
		//std::wcout << L"doar calea " << pathToFile << L"\n";
		strcat(pathToFile, token);
		strcat(pathToFile, ".txt");

		//wprintf(L"calea cat\re html :D\n\n");
		std::cout << "bla" << pathToFile << "caca\n\n";

		size_t size = strlen(pathToFile);

		printf("%d", size);
		wchar_t* wArr = new wchar_t[size];
		for (size_t i = 0; i < size; ++i) {
			wArr[i] = pathToFile[i];
		}

		wArr[size] = '\0';
		printf("%d\n", wcslen(wArr));
		wprintf(L"%s\n", wArr);

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
		
		message[dwRead] = '\0';
		printf("mesajul %s\n", message);
		int length = strlen(message) + 1; // Plus 1 for  null terminator
		int send_res = send(acceptSocket, message, length, 0); // Flag = 0
		Sleep(5000);
		if (send_res == SOCKET_ERROR) {
			wprintf(L"ERROR AT SENDIND MESSAGE  %d\n\n", WSAGetLastError());
			return -2;
		}

		printf("S A TRIIIIMIS \n");
	}
	*/

	// Close the socket to release the resources associated
	// Normally an application calls shutdown() before closesocket 
	//   to  disables sends or receives on a socket first
	// This isn't needed in this simple sample
	int iResult = closesocket(sock);
	if (iResult == SOCKET_ERROR) {
		wprintf(L"closesocket failed with error = %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}


	/* then call WSACleanup when done using the Winsock dll */

	WSACleanup();

}