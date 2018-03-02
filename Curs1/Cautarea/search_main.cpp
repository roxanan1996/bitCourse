#include <windows.h>
#include <stdio.h>

#define MAX_ELEMENTS				100000000

typedef struct __list
{
	int info;
	__list * next;
}list;

typedef struct __returnList
{
	list * nod;
	int position;
}returnList;

list * head = NULL;
list * tail = NULL;

int cauta_in_vector(int * vector, int vector_size, int elem)
{
	for (int i = 0 ; i < vector_size ; i++)
	{
		if (vector[i] == elem)
		{
			return i;
		}
	}
	return -1;
}

int cauta_in_lista(list * head_list, int elem, returnList * ret)
{
	list * nod = head_list;
	int i = 0;
	while(nod)
	{
		if (nod->info == elem)
		{
			ret->nod = nod;
			ret->position = i;
			return i;
		}
		i++;
		nod = nod->next;
	}
	ret->nod = NULL;
	ret->position = -1;

	return -1;
}

int main()
{
	srand(GetTickCount());
	int no_memory = 0;
	int * vector = (int *)malloc(MAX_ELEMENTS * sizeof(int));
	if (!vector)
		no_memory = 1;
	for (int i = 0 ; i < MAX_ELEMENTS ; i++)
	{
		int elem = rand()%MAX_ELEMENTS;
		vector[i] = elem;
		list * new_elem = (list *)malloc(sizeof(list));
		if (!new_elem)
		{
			no_memory = 1;
			break;
		}
		if (!head)
		{
			tail = new_elem;
			head = new_elem;
			new_elem->next = NULL;
		}
		else
		{
			tail->next = new_elem;
			tail = new_elem;
			new_elem->next = NULL;			

		}
		new_elem->info = elem;
	}

	int search_elem = rand()%MAX_ELEMENTS;
	int vector_ret = -1;
	returnList list_ret;
	list_ret.nod = NULL;
	list_ret.position = -1;

	if (!no_memory)
	{
		DWORD time_start1 = GetTickCount();
		vector_ret = cauta_in_vector(vector, MAX_ELEMENTS, search_elem);
		DWORD time_end1 = GetTickCount();


		DWORD time_start2 = GetTickCount();
		cauta_in_lista(head, search_elem, &list_ret);
		DWORD time_end2 = GetTickCount();

		printf("cautarea in vector a intors %d in timpul %u, cautarea in lista a intors %d %p in timpul %u.\n", vector_ret, time_end1-time_start1, list_ret.position, list_ret.nod, time_end2-time_start2);
	}
	return 0;
}