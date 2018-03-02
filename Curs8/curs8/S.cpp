#include <Windows.h>
#include <stdio.h>

HANDLE tutunSemaphore;
HANDLE hartieSemaphore;
HANDLE chibrituriSemaphore;

int main() {

	STARTUPINFO siTutun;
	PROCESS_INFORMATION piTutun;

	ZeroMemory(&siTutun, sizeof(siTutun));
	siTutun.cb = sizeof(siTutun);
	ZeroMemory(&piTutun, sizeof(piTutun));


	STARTUPINFO siHartie;
	PROCESS_INFORMATION piHartie;

	ZeroMemory(&siHartie, sizeof(siHartie));
	siHartie.cb = sizeof(siHartie);
	ZeroMemory(&piHartie, sizeof(piHartie));


	STARTUPINFO siChibrituri;
	PROCESS_INFORMATION piChibrituri;

	ZeroMemory(&siChibrituri, sizeof(siChibrituri));
	siChibrituri.cb = sizeof(siChibrituri);
	ZeroMemory(&piChibrituri, sizeof(piChibrituri));

	// creare semafoare

	 hartieSemaphore = CreateSemaphore(
		 NULL,           // default security attributes
		 0,  // initial count
		 1,  // maximum count
		 L"Local\\hartieSemaphore");          // unnamed semaphore

	 if (hartieSemaphore == NULL)
	 {
		 printf("CreateSemaphore error: %d\n", GetLastError());
		 return 1;
	 }

	 tutunSemaphore = CreateSemaphore(
		 NULL,           // default security attributes
		 0,  // initial count
		 1,  // maximum count
		 L"Local\\tutunSemaphore");          // unnamed semaphore

	 if (tutunSemaphore == NULL)
	 {
		 printf("CreateSemaphore error: %d\n", GetLastError());
		 return 1;
	 }
	 
	 chibrituriSemaphore = CreateSemaphore(
		 NULL,           // default security attributes
		 0,  // initial count
		 1,  // maximum count
		 L"Local\\chibrituriSemaphore");          // unnamed semaphore

	 if (chibrituriSemaphore == NULL)
	 {
		 printf("CreateSemaphore error: %d\n", GetLastError());
		 return 1;
	 }

	 // creare procese

	 BOOL tutun = CreateProcess(L"E:\\Bitdefender\\Curs8\\problemaFumatorilor\\Debug\\tutun.exe",   // No module name (use command line)
		 NULL,        // Command line
		 NULL,           // Process handle not inheritable
		 NULL,           // Thread handle not inheritable
		 FALSE,          // Set handle inheritance to FALSE
		 0,              // No creation flags
		 NULL,           // Use parent's environment block
		 NULL,           // Use parent's starting directory 
		 &siTutun,            // Pointer to STARTUPINFO structure
		 &piTutun);           // Pointer to PROCESS_INFORMATION structure;

	 if (!tutun)
	 {
		 printf("CreateProcess failed (%d).\n", GetLastError());
		 return 1;
	 }

	 BOOL hartie = CreateProcess(L"E:\\Bitdefender\\Curs8\\problemaFumatorilor\\Debug\\hartie.exe",   // No module name (use command line)
		 NULL,        // Command line
		 NULL,           // Process handle not inheritable
		 NULL,           // Thread handle not inheritable
		 FALSE,          // Set handle inheritance to FALSE
		 0,              // No creation flags
		 NULL,           // Use parent's environment block
		 NULL,           // Use parent's starting directory 
		 &siHartie,            // Pointer to STARTUPINFO structure
		 &piHartie);           // Pointer to PROCESS_INFORMATION structure;

	 if (!hartie)
	 {
		 printf("CreateProcess failed (%d).\n", GetLastError());
		 return 1;
	 }

	 BOOL chibrituri = CreateProcess(L"E:\\Bitdefender\\Curs8\\problemaFumatorilor\\Debug\\chibrituri.exe",   // No module name (use command line)
		 NULL,        // Command line
		 NULL,           // Process handle not inheritable
		 NULL,           // Thread handle not inheritable
		 FALSE,          // Set handle inheritance to FALSE
		 0,              // No creation flags
		 NULL,           // Use parent's environment block
		 NULL,           // Use parent's starting directory 
		 &siChibrituri,            // Pointer to STARTUPINFO structure
		 &piChibrituri);           // Pointer to PROCESS_INFORMATION structure;

	 if (!chibrituri)
	 {
		 printf("CreateProcess failed (%d).\n", GetLastError());
		 return 1;
	 }

	 while (1) {
		 int r = rand() % 3 + 1;

		 switch (r) {
		 case 1:
			 ReleaseSemaphore(tutunSemaphore, 1, NULL);
			 ReleaseSemaphore(hartieSemaphore, 1, NULL);
			 Sleep(1000);
			 printf("A eliberat tutun si hartie\n");
			 break;
		 case 2:
			 ReleaseSemaphore(tutunSemaphore, 1, NULL);
			 ReleaseSemaphore(chibrituriSemaphore, 1, NULL);
			 printf("A eliberat tutun si chibrituri\n");
			 Sleep(1000);
			 break;
		 case 3:
			 ReleaseSemaphore(chibrituriSemaphore, 1, NULL);
			 ReleaseSemaphore(hartieSemaphore, 1, NULL);
			 printf("A eliberat chibrituri si hartie\n");
			 Sleep(1000);
				 break;
		 }
		 WaitForSingleObject(tutunSemaphore, 2000);
		 WaitForSingleObject(hartieSemaphore, 2000);
		 WaitForSingleObject(chibrituriSemaphore, 2000);
	 }

	 return 0;

}