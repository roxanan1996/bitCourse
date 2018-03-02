#include <Windows.h>
#include <stdio.h>

int wmain(int argc, wchar_t * argv[])
{
	if (argc != 2)
		wprintf(L"Se ruleaza %s nume_fisier\n", argv[0]);


	HANDLE hFile = CreateFile(argv[1], GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Nu a putut fi deschis fisierul %s\n", argv[1]);
		return -1;
	}
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	BOOL bRead = FALSE;
	DWORD dwRead = 0;
	DWORD dwWritten = 0;
	char buffer[4096];
	do {
		bRead = ReadFile(hFile, buffer, 4096, &dwRead, NULL);
		WriteFile(hStdOut, buffer, dwRead, &dwWritten, NULL);
		if (dwRead != dwWritten)
		{
			printf("S-a produs o eroare la scriere - getting out.\n");
			return -2;
		}
	} while (bRead && dwRead);

	return 0;
}