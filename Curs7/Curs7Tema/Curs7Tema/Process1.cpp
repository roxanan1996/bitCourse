#include <stdio.h>
#include <Windows.h>


int wmain(int argc, wchar_t * argv[]) {

	if (argc != 2) {
		wprintf(L"wrong number of arguments, piece of shit!!\n");
		return -1;
	}

	int N = _wtoi(argv[1]);


	wchar_t mailslotName[100] = L"\\\\.\\mailslot\\mailslotName";
	HANDLE mailslot = CreateMailslot(mailslotName, 0, MAILSLOT_WAIT_FOREVER, NULL);

	if (mailslot == INVALID_HANDLE_VALUE) {
		printf("CreateMailslot failed with %d\n", GetLastError());
		return -1;
	}
	else {
		printf("Mailslot created successfully.\n");
	}

	HANDLE fileMessage = CreateFile(mailslotName, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (LPOVERLAPPED)NULL);

	if (fileMessage == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed: %d\n", GetLastError());
	}
	
	int nbOfSeconds = 4;
	DWORD bytesWritten = 0;
	BOOL writing = WriteFile(fileMessage, (LPCVOID)&nbOfSeconds, 4, &bytesWritten, NULL);

	printf("s a scris %d\n", bytesWritten);
	if (!writing) {
		printf("eroare la scriere\n\n");
		return -1;
	}

	/*HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("ExampleSlot"));
	if (NULL == hEvent)
		return -1;

	OVERLAPPED ov;
	ov.Offset = 0;
	ov.OffsetHigh = 0;
	ov.hEvent = hEvent;
	*/
	int nb = 0;
	DWORD bytesRead = 0;
	if (fileMessage == NULL) {
		printf("pulaa\n");
	}
	BOOL err = ReadFile(fileMessage, &nb, 4, &bytesRead, (LPOVERLAPPED)NULL);



	if (!err) {
		printf("ReadFile erroroooooare: %d\n", GetLastError());
		return -1;
	}

	printf("AM PRIIT MESAAJul : %d secundee\n\n", nb);
	return -1;
	
	for (int i = 0; i < N; ++i) {

		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// Start the child process. 
		if (!CreateProcess(L"E:\\Bitdefender\\Curs8\\Curs7Tema\\Debug\\Processes.exe",   // No module name (use command line)
			NULL,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi))           // Pointer to PROCESS_INFORMATION structure
			{ 
			printf("CreateProcess failed (%d).\n", GetLastError());
			return -1;
		}

		Sleep(2000);
	}

	
	return 0;

}