#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>

typedef int(*ISVALID)(char *);

int main()
{
	ISVALID isMyFunc;
	char* szExpresie = "expresia mea";

	HMODULE hDll = LoadLibrary(L"IsValid.dll");
	isMyFunc = (ISVALID)GetProcAddress(hDll, "IsValid");

	std::cout << isMyFunc(szExpresie) << "\n";

	FreeLibrary(hDll);

	return 0;
}