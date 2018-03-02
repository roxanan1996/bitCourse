#include <windows.h>
#include <stdio.h>
#define HASH_TABLE_SIZE 0x10000

typedef struct __list
{
	int info;
	__list * prev;
	__list * next;
}list;

list * pHashTable[HASH_TABLE_SIZE];

int teviValues[]  = {2, 7, 21, 33, 5, 44, 9, 21, 41, 78, 30, 19, 98, 22};

int M = 65;
int InsertInList(list ** head, list * elem)
{
	if (!head)
		return -1;

	elem->prev = NULL;
	if (*head)
	{
		elem->next = *head;
	}
	else
	{
		elem->next = NULL;
		*head = elem;
	}

	return 0;
}

int DeleteFromList(list ** head, list *elem)
{
	if (!head || !elem)
		return -1;

	if (elem->prev == NULL)
	{
		list * temp = elem->next;
		*head = temp;
		if (temp)
			temp->prev = NULL;
	}
	else
	{
		list * temp = elem->next;
		temp->prev = elem->prev;
		elem->prev->next = temp;
	}

	return 0;
}

unsigned short HashFct(int info)
{
	return info % HASH_TABLE_SIZE;
}

list * SearchInHash(list ** pHashTable, int info)
{
	unsigned short pos = HashFct(info);

	list * p = pHashTable[pos];

	while(p)
	{
		if (p->info == info)
			return p;
		p = p->next;
	}
	return NULL;
}

int main()
{
	memset(pHashTable, 0, sizeof(HASH_TABLE_SIZE*sizeof(list*)));

	for (int i = 0 ; i < HASH_TABLE_SIZE ; i++)
	{
		list * elem = (list *)malloc(sizeof(list));
		elem->info = i;
		InsertInList(&pHashTable[HashFct(elem->info)], elem);
	}

	list * del1 = SearchInHash(pHashTable, 5);
	list * del2 = SearchInHash(pHashTable, 37);
	DeleteFromList(&pHashTable[HashFct(del1->info)], del1);
	DeleteFromList(&pHashTable[HashFct(del2->info)], del2);

	for (int i = 0 ; i < HASH_TABLE_SIZE ; i ++)
	{
		list * p = pHashTable[i];
		while(p)
		{
			printf("Val = %d\n", p->info);
			p = p->next;
		}
	}

	for (int i = 0 ; i < HASH_TABLE_SIZE ; i ++)
	{
		list * p = pHashTable[i];
		while(p)
		{
			list * temp = p;
			p = p->next;

			free(temp);
		}
	}


	return 0;
}
