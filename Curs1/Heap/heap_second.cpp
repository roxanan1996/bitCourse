int parinte_(int i)
{
	return (i+1)>>1;
}

int left_(int i)
{
	return (i<<1)|1;
}

int right_(int i)
{
	return (i+1)<<1;
}

int do_position_heap_(int * my_array, int array_size, int position)
{
	int largest = 0;
	int l = left_(position);
	int r = right_(position);

	if (l < array_size && my_array[position] < my_array[l])
		largest = l;
	else
		largest = position;

	if (r < array_size && my_array[largest] < my_array[r])
		largest = r;

	if (largest != position)
	{
		int temp = my_array[largest];
		my_array[largest] = my_array[position];
		my_array[position] = temp;

		do_position_heap_(my_array, array_size, largest);
	}

	return 0;
}

int do_heap_(int * my_array, int array_size)
{
	for (int i = array_size/2 ; i >= 0; i--)
		do_position_heap_(my_array, array_size, i);

	return 0;
}

int do_sort_(int * my_array, int array_size)
{
	do_heap_(my_array, array_size);
	int crt_array_size = array_size;
	for (int i = array_size-1 ; i > 0 ; i--)
	{
		int temp = my_array[i];
		my_array[i] = my_array[0];
		my_array[0] = temp;
		crt_array_size--;
		do_position_heap_(my_array, crt_array_size, 0);
	}
	return 0;
}