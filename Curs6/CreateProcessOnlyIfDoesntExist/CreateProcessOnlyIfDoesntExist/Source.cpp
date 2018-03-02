#include <Windows.h>
#include <stdio.h>
#include <TCHAR.h>
#include <tlhelp32.h>
#include <cstdio>

bool IsProcessRunning(const wchar_t *processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry)) {
			printf("while\n");
			if (!wcsicmp(entry.szExeFile, processName)) {
				exists = true;
				printf("a gasit\n");
				break;
			}
		}
	CloseHandle(snapshot);
	return exists;
}

int _tmain(DWORD args, TCHAR* argv[]) {
	bool ceva = IsProcessRunning(argv[1]);

	if (ceva) {
		printf("running\n");
	}
	else {
		printf("not running\n");
	}
	return 0;
}