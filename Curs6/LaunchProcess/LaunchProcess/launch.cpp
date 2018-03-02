#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

int wmain(int argc, wchar_t* argv[]) {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	
	wchar_t commandArguments[100];
	wcscpy_s(commandArguments,argv[1]);
	for (int i = 2; i < argc; i++) {
		std::wcout << argv[i] << std::endl;
		wcscat_s(commandArguments, L" ");
		wcscat_s(commandArguments, argv[i]);
	}
	std::wcout << commandArguments << std::endl;
	wchar_t* newEnv = (wchar_t*)malloc(20 *sizeof(wchar_t));
	SetEnvironmentVariable(newEnv,L"var=VAR");
	//wchar_t* ceva = L"ceva\0ceva\0\0";
	bool isCreated = CreateProcess(NULL,
		commandArguments,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi);
	//termina ex
	if (isCreated)
		printf("Proces creat!\n");
	else
		printf("Eroare la crearea procesului!\n");

	DWORD exit_code = NULL;
	UINT code = NULL;
	Sleep(3000);
	if (GetExitCodeProcess(pi.hProcess, &exit_code) > 0) {
		std::cout << exit_code << std::endl;
		if (exit_code == STILL_ACTIVE)
			TerminateProcess(pi.hProcess, code);
	}
	std::cout << code;

	Sleep(3000);
	return 0;
}