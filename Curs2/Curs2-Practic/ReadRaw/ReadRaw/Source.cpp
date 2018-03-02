#include <windows.h>
#include <stdio.h>

int wmain(int argc, wchar_t * argv[])
{
	HANDLE hFile = CreateFile(L"\\\\.\\C:", GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, 0, NULL);


	char buffer[4096];
	DWORD dwRead;
	BOOL bRead = ReadFile(hFile, buffer, 4096, &dwRead, NULL);

	for (int i = 0; i < 4096; ++i) {
		printf("%c\n", buffer[i]);
	}

	CloseHandle(hFile);
	return 0;
}