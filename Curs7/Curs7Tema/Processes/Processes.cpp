#include <stdio.h>
#include <Windows.h>

int main() {

//	BOOL fResult;
	DWORD cbMessage = 0;
	DWORD cMessage = 0;
	wchar_t mailslotName[100] = L"\\\\.\\mailslot\\mailslotName";
	HANDLE handle = CreateFile(mailslotName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (handle == INVALID_HANDLE_VALUE)
	{
		printf("CreateFile failed: %d\n", GetLastError());
	}


	int nbOfSeconds;
	DWORD bytesRead = 0;
	BOOL err = ReadFile(handle, &nbOfSeconds, 4, &bytesRead, NULL);
	
	
	if (!err) {
		printf("ReadFile error: %d\n", GetLastError());
		return -1;
	}
	
	printf("AM PRIIT MESAAJul : %d secundee\n\n", nbOfSeconds);


	return 0;

}