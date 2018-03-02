#include <thread>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <algorithm>

#define NUM_WRITER 1
#define NUM_READER 1
#define WRITE_INTERVAL 10000
#define READ_INTERVAL 10000
#define INDEX_INTERVAL 20000
#define CHECK_INTERVAL 5000
#define MAX_SIZE 1000

CRITICAL_SECTION csSync; // pentru rawIndex in cazul in care caut si se adauga fix atunci
CRITICAL_SECTION data;

int rawIndex = 0, sortIndex = 0;
std::vector<int> database;

DWORD WINAPI write(LPVOID param) {
	while (true) {
		Sleep(WRITE_INTERVAL);
		printf("a intrat in write\n\n");
		EnterCriticalSection(&csSync);
		int toAdd = rand() % 1000;
		database.push_back(toAdd);
		printf("Am adaugat %d  %d\n", toAdd, database.back());
		rawIndex++;
		printf("raw index and size %d %d\n", rawIndex, database.size());
		std::vector<int>::iterator it;
		/*for (it = database.begin(); it < database.begin() + rawIndex; it++)
			std::cout << *it << " ";
		std::cout << std::endl;
		*/
		for (int i = 0; i < database.size(); ++i) {
			std::cout << "i este " << i << "\n";
			std::cout << database[i] << " ";
		}
		std::cout << "\n";
		LeaveCriticalSection(&csSync);
	}
	return 0;
}

DWORD WINAPI read(LPVOID param) {
	while (true) {
		Sleep(READ_INTERVAL);
		printf("a intrat in read\n\n");
		EnterCriticalSection(&data);
		int toFind = rand() % 1000;

		printf("cauta in zona sortata\n\n");
		bool found = std::binary_search(database.begin(), database.begin() + sortIndex, toFind);
		//std::cout << "toFind " << toFind << std::endl;
		std::vector<int>::iterator it;
		EnterCriticalSection(&csSync);
		if (!found) {
			printf("cauta in zona nesortata\n");
			for (it = database.begin() + sortIndex; it <= database.begin() + rawIndex; it++) {
				std::cout << "database " << *it << std::endl;
				if (*it == toFind) {
					found = true;
					break;
				}
			}
		}
		LeaveCriticalSection(&csSync);
		LeaveCriticalSection(&data);
		if (found)
			printf("Am gasit %d.\n", toFind);
		else
			printf("Nu am gasit %d.\n", toFind);

	}
	return 0;
}

DWORD WINAPI sort(LPVOID param) {
	while (true) {
		Sleep(INDEX_INTERVAL);
		//nu e bine
		printf("a intrat in sort\n\n");
		EnterCriticalSection(&data);
		printf("rawIndex = %d\n", rawIndex);
		sortIndex = rawIndex;
		std::sort(database.begin(), database.begin() + sortIndex);
		//sortIndex = rawIndex;
		printf("sortIndex = %d\n", sortIndex);
		std::vector<int>::iterator it;
		for (it = database.begin(); it <= database.begin() + rawIndex; it++)
			std::cout << *it << " ";
		std::cout << std::endl;
		LeaveCriticalSection(&data);
	}
	return 0;
}

DWORD WINAPI check(LPVOID param) {
	
	return 0;
}

int main() {
	//nu vreau sa caut cand se sorteaza
	//nu vreau cand scriu sa scriu de doua ori in acelasi loc

	InitializeCriticalSection(&csSync);
	InitializeCriticalSection(&data);

	HANDLE writerHandles[NUM_WRITER];
	DWORD wThreadIds[NUM_WRITER];
	HANDLE readerHandles[NUM_READER];
	DWORD rThreadIds[NUM_READER];
	HANDLE indexerThread; //sorteaza
	DWORD iThreadId;
	HANDLE checkThread;
	DWORD cThreadId;

	database.reserve(MAX_SIZE);
	for (int i = 0; i < NUM_WRITER; i++) 
		writerHandles[i] = CreateThread(NULL, 0, write, NULL, 0, &wThreadIds[i]);

	for (int i = 0; i < NUM_READER; i++) 
		readerHandles[i] = CreateThread(NULL, 0, read, NULL, 0, &rThreadIds[i]);

	indexerThread = CreateThread(NULL, 0, sort, NULL, 0, &iThreadId);
	checkThread = CreateThread(NULL, 0, check, NULL, 0, &cThreadId);

	WaitForMultipleObjects(NUM_WRITER, writerHandles, TRUE, INFINITE);
	WaitForMultipleObjects(NUM_READER, readerHandles, TRUE, INFINITE);
	WaitForSingleObject(indexerThread, INFINITE);
	WaitForSingleObject(checkThread, INFINITE);
	
	DeleteCriticalSection(&csSync);
	DeleteCriticalSection(&data);
	for (int i = 0; i < NUM_WRITER; i++) {
		CloseHandle(writerHandles[i]);
	}

	for (int i = 0; i < NUM_READER; i++) {
		CloseHandle(readerHandles[i]);
	}

	CloseHandle(indexerThread);
	CloseHandle(checkThread);
}