#include <Windows.h>
#include <iostream>

int main() {

	LPTCH env = GetEnvironmentStrings();
	LPTSTR variables = (LPTSTR)env;

	while (*variables)
	{
		std::cout << variables << std::endl;
		variables += lstrlen(variables) + 1;
	}
	FreeEnvironmentStrings(env);
	return 0;
}