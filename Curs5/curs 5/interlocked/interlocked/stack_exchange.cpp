#include <thread>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#define ERROR -99999
int DONE = 0;
int suma = 0;

struct Node {
	int value;
	Node* next;
};

Node* stack_head = NULL;
CRITICAL_SECTION print;

DWORD WINAPI push(LPVOID lpThreadParameter) {

	for (int i = 0; i <= 20; i++) {
		
			Node* newNode = (Node*)malloc(sizeof(Node));
			newNode->value = i;
			while (true) {
				Node* current = stack_head;
				newNode->next = current;
				Node* returned = (Node*) InterlockedCompareExchangePointer((void**)&stack_head, newNode, current);
				if (returned == current) {
					EnterCriticalSection(&print);
					printf("Elementul adaugat este %d!\n",i);
					LeaveCriticalSection(&print);
					break;
				}
			}
	}

	DONE = 1;
	return 0;
}

DWORD WINAPI pop(LPVOID lpThreadParameter) {
	
	while (1) {
		Node* old_head = stack_head;
		if (old_head == NULL) {
			if (DONE == 1) {
				EnterCriticalSection(&print);
				printf("Stiva este goala!\n");
				LeaveCriticalSection(&print);
				break;
			}
			else
				continue;
		}
		else {
			while (true) {
				int value = old_head->value;
				Node* new_head = old_head->next;
				Node* returned = (Node*)InterlockedCompareExchangePointer((void**)&stack_head, new_head, old_head);
				if (returned == old_head) {
					EnterCriticalSection(&print);
					printf("Elementul scos este %d!\n", value);
					LeaveCriticalSection(&print);
					suma += value;
					break;
				}
			}
		}
	}
	return 0;
	
}
 
int main() {

	HANDLE dwHandles[2];
	DWORD dwThreadId[2];

	InitializeCriticalSection(&print);
	dwHandles[0] = CreateThread(NULL, 0, push, NULL, 0, &dwThreadId[0]);
	dwHandles[1] = CreateThread(NULL, 0, pop, NULL, 0, &dwThreadId[1]);

	WaitForMultipleObjects(2, dwHandles, TRUE, INFINITE);
	printf("Suma este %d\n", suma);
	CloseHandle(dwHandles[0]);
	CloseHandle(dwHandles[1]);
	DeleteCriticalSection(&print);
	return 0;
}