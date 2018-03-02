#include <windows.h>
#include <stdio.h>


int parinte_(int i);
int left_(int i);
int right_(int i);
int do_position_heap_(int * my_array, int array_size, int position);
int do_heap_(int * my_array, int array_size);
int do_sort_(int * my_array, int array_size);

int parinte(int i)
{
	return i>>1;
}

int left(int i)
{
	return i<<1;
}

int right(int i)
{
	return (i<<1)|1;
}

int do_position_heap(int * my_array, int array_size, int position)
{
	int largest = 0;
	int l = left(position);
	int r = right(position);

	if (l <= array_size && my_array[position] < my_array[l])
		largest = l;
	else
		largest = position;

	if (r <= array_size && my_array[largest] < my_array[r])
		largest = r;

	if (largest != position)
	{
		int temp = my_array[largest];
		my_array[largest] = my_array[position];
		my_array[position] = temp;

		do_position_heap(my_array, array_size, largest);
	}

	return 0;
}

int do_heap(int * my_array, int array_size)
{
	for (int i = array_size/2 ; i >= 1; i--)
		do_position_heap(my_array, array_size, i);

	return 0;
}

int do_sort(int * my_array, int array_size)
{
	do_heap(my_array, array_size);
	int crt_array_size = array_size;
	for (int i = array_size ; i > 1 ; i--)
	{
		int temp = my_array[i];
		my_array[i] = my_array[1];
		my_array[1] = temp;
		crt_array_size--;
		do_position_heap(my_array, crt_array_size, 1);
	}
	return 0;
}

int main()
{
	int n;
	printf("Introduceti numarul de elemente:\n");
	scanf_s("%d", &n);

	int * my_array = (int *) malloc(sizeof(int) * (n+1));
	
	printf("Introduceti numerele:\n");


	my_array[0] = 0;
	for (int i = 0 ; i < n ; i++)
	{
		scanf_s("%d",&my_array[i+1]); 
	}

	do_sort(my_array, n);
	for (int i = 1 ; i <= n ; i++)
		printf ("%d ", my_array[i]);
	printf("\n");


//	my_array[0] = 0;
/*	for (int i = 0 ; i < n ; i++)
	{
		scanf_s("%d",&my_array[i]); 
	}

	do_sort_(my_array, n);

	for (int i = 0 ; i < n ; i++)
		printf ("%d ", my_array[i]);
	printf("\n");*/
	return 0;
}