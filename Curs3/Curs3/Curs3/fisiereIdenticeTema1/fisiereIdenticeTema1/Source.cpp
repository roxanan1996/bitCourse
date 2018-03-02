#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <tchar.h>
#include <strsafe.h>
#include <map>
#include <list>
#include "md5.h"

int wmain(int argc, TCHAR *argv[]) {
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	size_t length_of_arg;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;


	/* map-ul unde tin <sizeFile, ListOfFile> */
	std::map<long, std::list<TCHAR*> > sizeMap;

	/* Daca directorul nu e pasat ca argument, print eroare */
	if (argc != 2) {
		_tprintf(TEXT("\nUsage: %s <directory name>\n"), argv[0]);
		return -1;
	}

	/* verific calea + 3 sa nu fie mai lunga decat MAX_PATH 
		(append \*) */
	length_of_arg = wcslen(argv[1]);

	if (length_of_arg > (MAX_PATH - 3)) {
		wprintf(L"\nDirectory path is too long.\n");
		return -1;
	}

	wprintf(L"\nTarget directory is %s\n\n", argv[1]);

	/* adaug \* la calea directorului */
	wcscpy_s(szDir, MAX_PATH, argv[1]);
	wcscat_s(szDir, MAX_PATH, L"\\*");

	/* caut primul fisier in director */

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind) {
		wprintf(L"Error on FindFirstFile");
		return dwError;
	}

	// list: 
	do {
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			// e fisier
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			long size = filesize.QuadPart;
			TCHAR* nameFile = ffd.cFileName;
			wprintf(L"numele: %s\n", nameFile);
			if (sizeMap.find(size) == sizeMap.end()) {
				printf("nu mai e in map\n");
				std::list<TCHAR*> l;
				l.push_back(nameFile); // ????
				sizeMap.insert(std::pair<long, std::list<TCHAR*> >(size, l));
				//wprintf(L"%s\n", sizeMap[size].front());
			}
			else {
				// exista deja :-?
				sizeMap[size].push_back(nameFile);
			}
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES) {
		wprintf(L"FindFirstFile error %d\n", dwError);
		return dwError;
	}

	FindClose(hFind);


	/*for (auto elem : sizeMap) {
		//aici parcurg lista cu fisiere cu acelasi size si fac md5 pe el
		/*if (elem.second.size() > 1) {
			std::map <int, std::list<string> > mapulet;
			for (auto file : elem.second) {
				//aici fac md5
				int md5File;
				if (mapulet.find(md5File) == mapulet.end()) {
					mapulet.insert(bla bla);
				}
				else {
					mapulet[md5File].push_back(nume);
				}
			}

			for (auto ceva : mapulet) {
				//afisez fiecare elem
			}
		}
		else {
			// afisez numele fisierului
		} 

		printf("size: %ld\n", elem.first);
		for (auto i : elem.second) {
			wprintf(L"%s\n", i);
		}
	}
	*/
	
	for (std::list<TCHAR*>::iterator it = sizeMap[0].begin(); it != sizeMap[0].end(); it++) {
		wprintf(L"%s ", *it);
	}
	
	return dwError;
}