#include <Windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include <strsafe.h>
#include <vector>
#include <map>
#include <iostream>

/**
 * compara doua fisiere
 * citesc cate un byte din fiecare fisier si le compar
 * intoarce true daca sunt identice, false otherwise
 */
BOOL compareTwoFiles(HANDLE h1, HANDLE h2) {
	BOOL h1Read = FALSE;
	BOOL h2Read = FALSE;
	DWORD dwRead1 = 0;
	DWORD dwRead2 = 0;
	char buffer1[1], buffer2[1];

	do {
		h1Read = ReadFile(h1, buffer1, 1, &dwRead1, NULL);
		h2Read = ReadFile(h2, buffer2, 1, &dwRead2, NULL);

		if (dwRead1 != dwRead2 || !(strcmp(buffer1, buffer2))) {
			return FALSE;
		}
	} while ((h1Read && dwRead1) || (h2Read && dwRead2));

	return TRUE;
}

int wmain(int argc, TCHAR* argv[]) {
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;

	/* map cu cheia <size_fis> si valoare o lista cu numele fisierelor */
	std::map<long long, std::vector <std::wstring> > sizeMap;


	if (argc != 2) {
		_tprintf(TEXT("\nUsage: %s <directory name>\n"), argv[0]);
		return -1;
	}

	length_of_arg = wcslen(argv[1]);

	if (length_of_arg > (MAX_PATH - 3)) {
		wprintf(L"\nDirectory path is too long.\n");
		return -1;
	}

	wprintf(L"\nTarget directory is %s\n\n", argv[1]);

	wcscpy_s(szDir, MAX_PATH, argv[1]);

	wcscat_s(szDir, MAX_PATH, L"\\*");

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		wprintf(L"Error on FindFirstFile");
		return dwError;
	}

	do {
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			// e fisier
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			long long size = filesize.QuadPart;
			std::wstring name(ffd.cFileName);

			/* verific daca mai exista o astfel de cheie in map */
			if (sizeMap.find(size) == sizeMap.end()) {

				/* creez o lista cu un singur element*/
				std::vector <std::wstring> l;
				l.push_back(name);
				sizeMap.insert(std::pair<long long, std::vector<std::wstring> >(size, l));
			}
			else {
				sizeMap[size].push_back(name);
			}
		}
	} while (FindNextFile(hFind, &ffd));

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		wprintf(L"FindFirstFile error %d", dwError);
	}

	FindClose(hFind);

	/** parcurg map-ul
	 * pentru fiecare lista (valoare), daca size > 1 
	 * compar fisierele doua cate doua
	 * marchez intr-un vector fisierele identice

	*/

	std::map<long long, std::vector<std::wstring> >::iterator it;
	for (it = sizeMap.begin(); it != sizeMap.end(); ++it) {
		std::vector<std::wstring> fileNames = it->second;
		int n = fileNames.size();

		if (n == 1) {
			std::wcout << "unic: " << fileNames[0] << "\n\n";
		} else {

			/* avem mai multe fisiere cu acelasi size */
			std::vector<int> index;
			std::vector<HANDLE> hFiles;

			/* pun intr-un vector referintele la fisiere deschide 
			pentru a nu apela createFile de fiecare data cand compar doua fis */
			for (int i = 0; i < n; ++i) {
				index.push_back(i);
				TCHAR dirName[MAX_PATH];
				TCHAR *nameFilei = (wchar_t*) fileNames[i].c_str();
				wcscpy_s(dirName, MAX_PATH, argv[1]);
				wcscat_s(szDir, MAX_PATH, nameFilei);
				HANDLE hFile = CreateFile(argv[1],
										 GENERIC_WRITE,
										  0,                      
									      NULL,                   
										  OPEN_EXISTING,             
									      0,  
									      NULL);                  
				hFiles.push_back(hFile);
			}

			/* compar doua cate doua, daca nu sunt marcate cu acelasi index */
			for (int i = 0; i < n; ++i) {
				for (int j = i + 1; j < n; ++j) {
					if (index[i] != index[j]) {
						if (compareTwoFiles(hFiles[i], hFiles[j])) {
							index[j] = index[i];
						}
					}
				}
			}

			/* pentru afisare, folosesc urmatorul map */
			std::map <int, std::vector<std::wstring> > m;

			/* la indexul x am o lista cu numele fisierelor la fel */
			for (int i = 0; i < n; ++i) {
				int in = index[i];
				if (m.find(in) == m.end()) {
					std::vector<std::wstring> v;
					v.push_back(fileNames[i]);
					m.insert(std::pair<int, std::vector<std::wstring> >(in, v));
				} else {
					m[in].push_back(fileNames[i]);
				}
			}

			/* afisare */
			std::map<int, std::vector<std::wstring> >::iterator iter;
			for (iter = m.begin(); iter != m.end(); ++iter) {
				std::cout << "indentice -----> ";

				std::vector<std::wstring>::iterator ii;
				for (ii = iter->second.begin(); ii != iter->second.end(); ++ii) {
					std::wcout << *ii << " ";
				}

				std::cout << "\n\n";
			}

			for (int i = 0; i < n; ++i) {
				FindClose(hFiles[i]);
			}
		}
	}

	return dwError;

}
