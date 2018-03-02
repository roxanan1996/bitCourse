#include <windows.h>
#include <stdio.h>

#define MAX_ELEMENTS				10000000

typedef struct __list
{
	int info;
	__list * prev;
	__list * next;
}list;

typedef struct __returnList
{
	list * nod;
	int position;
}returnList;

list * head = NULL;
list * tail = NULL;

int stergerea_in_vector(int * vector, int current_vector_size, int max_vector_size, int position)
{
	if (current_vector_size < position)
		return -1;

	if (position < -1)
		return -1;

	for (int i = position ; i < current_vector_size ; i++)
	{
		vector[i] = vector[i+1];
	}

	return 0;
}

int stergerea_in_lista(list ** head, list ** tail, list * elem)
{
	list * old_elem = NULL;

	if (!elem)
	{

		return 0;
	}
	else
	{
		if (elem == *tail)
		{
			if (elem->prev)
			{
				//head e diferit de tail
				list * temp = elem->prev;
				//elem e chiar tail, deci next pentru elem e NULL;
				temp->next = NULL;
				*tail = temp;
				free(elem);
			}
			else
			{
				//head si tail sunt identice si e singurul element
				*head = NULL;
				*tail = NULL;
				free(elem);
			}
		}
		else
		{
			if (elem->prev)
			{
				//elem nu e nici head si nici tail
				list * temp1 = elem->prev;
				list * temp2 = elem->next;
				temp1->next = temp2;
				temp2->prev = temp1;
				free(elem);
			}
			else
			{
				//elem e head
				list * temp = elem->next;
				*head = temp;
				temp->prev = NULL;
				free(elem);
			}
		}
		return 0;
	}
	return -1;
}

int main()
{
	int * vector = (int *)malloc(MAX_ELEMENTS * sizeof(int));
	list * elem = NULL;

	for (int i = 0 ; i < MAX_ELEMENTS ; i++)
	{
		vector[i] = i;
		list * new_elem = (list *)malloc(sizeof(list));
		if (!new_elem)
		{
			break;
		}
		if (!head)
		{
			tail = new_elem;
			head = new_elem;
			new_elem->next = NULL;
			new_elem->prev = NULL;
		}
		else
		{
			tail->next = new_elem;
			new_elem->prev = tail;
			tail = new_elem;
			new_elem->next = NULL;			

		}
		new_elem->info = i;
		if (i == MAX_ELEMENTS/2)
		{
			elem = new_elem;
		}
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
	vector_ret = stergerea_in_vector(vector, MAX_ELEMENTS, MAX_ELEMENTS, MAX_ELEMENTS/2);
	DWORD time_end1 = GetTickCount();


	DWORD time_start2 = GetTickCount();
	list_ret = stergerea_in_lista(&head, &tail, elem);
	DWORD time_end2 = GetTickCount();

	printf("stergerea in vector a intors %d in timpul %u, stergerea in lista a intors %d in timpul %u.\n", vector_ret, time_end1-time_start1, list_ret, time_end2-time_start2);


	list* contor2 = head;
	for (int i = 0 ; i < MAX_ELEMENTS && contor2 ; i++)
	{
		printf("%d %d\n", vector[i], contor2->info);
		contor2 = contor2->next;
	}
	return 0;
}