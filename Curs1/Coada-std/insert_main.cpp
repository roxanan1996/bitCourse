#include <windows.h>
#include <stdio.h>
#include <queue>

#define MAX_ELEMENTS 1000000


int main()
{
	std::queue<int> std_queue;
	for (int i = 0 ; i < MAX_ELEMENTS ; i++)
	{
		std_queue.push(i);
	}

	
	while(!std_queue.empty())
	{

		printf("%d\n", std_queue.front());
		std_queue.pop();
	}

	return 0;
}