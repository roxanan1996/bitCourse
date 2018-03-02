#include <windows.h>
#include <stdio.h>

#define MAX_ELEMENTS				1000000

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

int inserare_in_vector(int * vector, int current_vector_size, int max_vector_size, int elem, int position)
{
	if (current_vector_size + 1 > max_vector_size)
		return -1;
	if (current_vector_size < position)
		return -1;

	for (int i = current_vector_size ; i > position ; i--)
	{
		vector[i] = vector[i-1];
	}

	vector[position] = elem;

	return 0;
}

int inserare_in_lista(list * head, list * tail, list * prev, int elem)
{
	list * new_elem = (list *)malloc(sizeof(list));	
	new_elem->info = elem;
	if (!prev)
	{
		new_elem->next = NULL;
		head = new_elem;
		tail = new_elem;
		return 0;
	}
	else
	{
		if (prev == tail)
		{
			prev->next = new_elem;
			new_elem->next = NULL;
			tail = new_elem;
		}
		else
		{
			list * temp = prev->next;
			prev->next = new_elem;
			new_elem->next = temp;
		}
		return 0;
	}
	return -1;
}

int main()
{

	int * vector = (int *)malloc(MAX_ELEMENTS * sizeof(int));
	list * prev = NULL;

	int position = 0;
	for (int i = 0 ; i < MAX_ELEMENTS ; i++)
	{
		if (i == MAX_ELEMENTS/2)
		{
			prev = tail;
			continue;
		}

		vector[position] = i;
		list * new_elem = (list *)malloc(sizeof(list));
		if (!new_elem)
		{
			//no_memory = 1;
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
		new_elem->info = i;
		position ++;
	}

	int vector_ret = -1;
	int list_ret = -1;
	
	list* contor = head;
	for (int i = 0 ; i < MAX_ELEMENTS && contor ; i++)
	{
		printf("%d %d\n", vector[i], contor->info);
		contor = contor->next;
	}

	DWORD time_start1 = GetTickCount();
	vector_ret = inserare_in_vector(vector, MAX_ELEMENTS-1, MAX_ELEMENTS, MAX_ELEMENTS/2, MAX_ELEMENTS/2);
	DWORD time_end1 = GetTickCount();


	DWORD time_start2 = GetTickCount();
	list_ret = inserare_in_lista(head, tail, prev, MAX_ELEMENTS/2);
	DWORD time_end2 = GetTickCount();

	printf("inserarea in vector a intors %d in timpul %u, inserarea in lista a intors %d in timpul %u.\n", vector_ret, time_end1-time_start1, list_ret, time_end2-time_start2);


	list* contor2 = head;
	for (int i = 0 ; i < MAX_ELEMENTS && contor2 ; i++)
	{
		printf("%d %d\n", vector[i], contor2->info);
		contor2 = contor2->next;
	}
	return 0;
}