#include <stdio.h>
#include <Windows.h>

int main() {

	SC_HANDLE scManager = OpenSCManager(NULL, SERVICES_ACTIVE_DATABASE, SC_MANAGER_ALL_ACCESS);


	if (scManager == NULL) {
		printf("EROAREB LA MANAGER  %d\n", GetLastError());
		return -1;
	}

	ENUM_SERVICE_STATUS *buffer = (ENUM_SERVICE_STATUS*)malloc(28272);
	DWORD bytesNeeded = 0;
	DWORD servicesReturned = 0;
	DWORD var = 0;

	printf("size of structure %d\n", sizeof(ENUM_SERVICE_STATUS));
	printf("inainte de functie\n");
	BOOL enumServices = EnumServicesStatus(
		scManager,
		SERVICE_WIN32,
		SERVICE_ACTIVE,
		buffer,
		28272,
		&bytesNeeded,
		&servicesReturned,
		&var
	);

	printf("var %d\n", var);

	
	printf("numarul de servicii ramase %d\n", bytesNeeded);
	printf("numarul de servicii returnate:: %d\n", servicesReturned);

	
	for (int i = 0; i < servicesReturned; ++i) {
		wprintf(L"serviciul: %s\n", buffer[i].lpServiceName);
	}

	

}