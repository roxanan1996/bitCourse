#include <windows.h>
#include <stdio.h>

#define MyDir_EntryType_Directory	2
#define MyDir_EntryType_File		1
#define MAX_DIR_PATH				MAX_PATH //it is 256 on windows

typedef int (*MyDirEnumeratorFn_W)(void* pCtx, const wchar_t * szEntry, int iType);
int EnumDir_W(const wchar_t * szFrom, MyDirEnumeratorFn_W lpfnF, void* pFCtx);

int MyFunc(void * pCtx, const wchar_t * szEntry, int iType)
{
	if (iType == MyDir_EntryType_Directory)
	{
		wprintf(L"%s directory\n", (wchar_t*)pCtx);
		CreateDirectory((wchar_t*)pCtx, NULL);
		EnumDir_W(szEntry, MyFunc, pCtx);
	}
	else
	{
		wprintf(L"%s %s file\n", (wchar_t *)pCtx, szEntry);
		CopyFile(szEntry, (wchar_t*)pCtx, FALSE);
	}
	return 0;
}


int EnumDir_W(const wchar_t * szFrom, MyDirEnumeratorFn_W lpfnF, void* pFCtx)
{
	WIN32_FIND_DATAW	FindFileData;
	HANDLE			hFind = INVALID_HANDLE_VALUE;
	wchar_t			pFrom[MAX_DIR_PATH], pFF[MAX_DIR_PATH];
	wchar_t			pTo[MAX_DIR_PATH];
	int				iRet = 0;
	size_t			uFrom, uN;
		
	uFrom = wcslen(szFrom);
	
	if (uFrom >= MAX_DIR_PATH-3)
		return -1;
	
	wcscpy_s(pFF, MAX_DIR_PATH, szFrom);
	wcsncat_s(pFF, MAX_DIR_PATH, L"\\*", 3);
	
	do
	{
		if (hFind == INVALID_HANDLE_VALUE)
		{
			hFind = FindFirstFileW(pFF, &FindFileData);
			if (hFind == INVALID_HANDLE_VALUE)
				break;
		}
		else if (FindNextFileW(hFind, &FindFileData) == 0)
		{
			if (GetLastError() != ERROR_NO_MORE_FILES)
				iRet = -1;
			break;
		}
		
		if (wcscmp(FindFileData.cFileName, L".") == 0 || wcscmp(FindFileData.cFileName, L"..") == 0)
			continue;
		
		uN = wcslen(FindFileData.cFileName) + 1;
		if (uFrom + uN >= MAX_DIR_PATH)
		{
			iRet = -1;
			break;
		}
		
		swprintf_s(pFrom, MAX_DIR_PATH, L"%s%c%s", szFrom, '\\', FindFileData.cFileName);
		swprintf_s(pTo, MAX_DIR_PATH, L"%s%c%s", (wchar_t *)pFCtx, '\\', FindFileData.cFileName);
		

		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			iRet = lpfnF(pTo, pFrom, MyDir_EntryType_Directory);
		else
			iRet = lpfnF(pTo, pFrom, MyDir_EntryType_File);
	}
	while (iRet == 0);
	
	FindClose(hFind);
	
	return iRet;
}


int wmain(int argc, wchar_t * argv[])
{
	wprintf(L"%s %s\n", argv[1], argv[2]);
	if (!CreateDirectory(argv[2], NULL)) {
		printf("Error at creating directory\n");
		if (ERROR_ALREADY_EXISTS == GetLastError()) {
			printf("path not found\n");
		}
	}

	EnumDir_W(argv[1], MyFunc, argv[2]);
	return 0;
}