#include <windows.h>
#include <stdio.h>
#include <stack>

#define MAX_ELEMENTS 1000000


int main()
{
	std::stack<int> std_stack;
	for (int i = 0 ; i < MAX_ELEMENTS ; i++)
	{
		std_stack.push(i);
	}

	
	while(!std_stack.empty())
	{

		printf("%d\n", std_stack.top());
		std_stack.pop();
	}

	return 0;
}