#include <windows.h>
#include <stdio.h>

#define MAX_ELEMENTS				1000000

typedef struct __queue
{
	int info;
	__queue * next;
}queue;

queue * head = NULL;
queue * tail = NULL;

int inserare(queue ** head, queue ** tail, int elem)
{
	queue * new_elem = (queue *)malloc(sizeof(queue));	
	new_elem->info = elem;
	if (*tail)
	{
		(*tail)->next = new_elem;
		*tail = new_elem;
	}
	else
		*tail = new_elem;
	new_elem->next = NULL;
	if (!*head)
		*head = new_elem;
	return 0;
}

queue * extragere(queue ** head)
{
	queue * temp = *head;
	if (temp)
	{
		*head = temp->next;
	}
	return temp;
}

int main()
{

	for (int i = 0 ; i < MAX_ELEMENTS ; i++)
	{
		inserare(&head, &tail, i);
	}

	queue * p = extragere(&head);
	printf("%d\n", p->info);

	while(p)
	{
		p = extragere(&head);
		printf("%d\n", p->info);
	
		if (p)
			free(p);

	}

	return 0;
}