#include <windows.h>
#include <stdio.h>

#define MAX_ELEMENTS				1000000

typedef struct __stack
{
	int info;
	__stack * next;
}stack;

stack * head = NULL;


int inserare(stack ** head, int elem)
{
	stack * new_elem = (stack *)malloc(sizeof(stack));	
	new_elem->info = elem;
	new_elem->next = *head;
	*head = new_elem;
	return 0;
}

stack * extragere(stack ** head)
{
	stack * temp = *head;
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
		inserare(&head, i);
	}

	stack * p = extragere(&head);

	while(p)
	{
		printf("%d\n", p->info);
		p = extragere(&head);
	}

	return 0;
}