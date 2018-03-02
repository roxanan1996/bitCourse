#include <windows.h>
#include <stdio.h>

#define MAX_ELEMENTS				100000

//In my Project ->Properties -> Configuration Properties -> Linker -> System -> Stack Reserve Size :

int afiseaza(int totalelements, char* stack_base )
{
	char pos = 0;
	char * stack_pos = &pos;
	printf("stack size %u", stack_base - stack_pos);
	if (!totalelements)
	{
		printf("0\n");
		return 0;
	}
	afiseaza(totalelements-1, stack_base);
	printf("%d\n", totalelements);
	return 0;
}


int main()
{
	char stack_base = 0;
	afiseaza(MAX_ELEMENTS, &stack_base);
	return 0;
}