#include <Windows.h>
#include <stdio.h>

SC_HANDLE schManager;

int stopDependencies(SC_HANDLE schService) {
	DWORD dwBytesNeeded;
	DWORD dwCount;

	LPENUM_SERVICE_STATUS lpDependencies = NULL;
	ENUM_SERVICE_STATUS ess;
	SC_HANDLE hDepService;
	SERVICE_STATUS_PROCESS ssp;

	DWORD dwStartTime = GetTickCount();
	DWORD dwTimeout = 30000;

	if (EnumDependentServices(schService, SERVICE_ACTIVE, lpDependencies, 0,
		&dwBytesNeeded, &dwCount)) {
		printf("no depeninte\n\n");
		return 0;
	}
	else {
		if (GetLastError() != ERROR_MORE_DATA) {
			printf("eroare la dependinte :(\n\n");
				return -1;
		}

		lpDependencies = LPENUM_SERVICE_STATUS(dwBytesNeeded);

		if (NULL == lpDependencies) {
			printf("eroare la alocare buffer de dependinte \n");
			return -1;
		}
		_try {
			if (!EnumDependentServices(schService, SERVICE_ACTIVE,
				lpDependencies, dwBytesNeeded, &dwBytesNeeded,
				&dwCount)) {
				printf("eroaree la dependinte dupa ce am alocat bufferul\n");
				return -1;
			}

			for (DWORD i = 0; i < dwCount; ++i) {
				SC_HANDLE hDepService = OpenService(schManager,
					lpDependencies[i].lpServiceName,
					SERVICE_STOP | SERVICE_QUERY_STATUS);

				if (!hDepService) {
					return -1;
				}

				_try {
					if (!ControlService(hDepService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp)) {
						return -1;
					}
					while (ssp.dwCurrentState != SERVICE_STOPPED) {
						Sleep(ssp.dwWaitHint);
						if (!QueryServiceStatusEx(
							hDepService,
							SC_STATUS_PROCESS_INFO,
							(LPBYTE)&ssp,
							sizeof(SERVICE_STATUS_PROCESS),
							&dwBytesNeeded))
							return FALSE;

						if (ssp.dwCurrentState == SERVICE_STOPPED)
							break;

						if (GetTickCount() - dwStartTime > dwTimeout)
							printf("timeout la una din dependinte\n\n");
							return -1;
					}
				}
				_finally {
					// Always release the service handle.
					CloseServiceHandle(hDepService);
				}
			}
		}
		_finally {
			// Always free the enumeration buffer.
			free(lpDependencies);
		}
	}
		return 0;
}

int stopService(LPCTSTR serviceName) {
	

	DWORD dwTimeOut = 3000;
	DWORD dwStartTime = GetTickCount();
	SC_HANDLE serviceHandler = OpenService(schManager,
		serviceName,
		SC_MANAGER_ALL_ACCESS);

	if (serviceHandler == NULL) {
		printf("EROARE LA OPENSERVICE :(%d\n\n", GetLastError());
		return -1;
	}

	SERVICE_STATUS_PROCESS ssp;
	DWORD dwBytesNeeded = 0;
	// vedem daca nu a fost oprit inainte
	if (!QueryServiceStatusEx(
		serviceHandler,
		SC_STATUS_PROCESS_INFO,
		(LPBYTE)&ssp,
		sizeof(SERVICE_STATUS_PROCESS),
		&dwBytesNeeded)) {
		printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
		return - 1;
	}

	if (ssp.dwCurrentState == SERVICE_STOPPED) {
		printf("Service is already stopped.\n");
		return -1;
	}

	// daca e suspendat asteptam

	while (ssp.dwCurrentState == SERVICE_STOP_PENDING) {
		printf("Service stop pending...\n");

		DWORD dwWaitTime = ssp.dwWaitHint / 10;
		dwWaitTime = dwWaitTime < 1000 ? 1000 : (dwWaitTime > 10000 ? 10000 : dwWaitTime);
		Sleep(dwWaitTime);


		if (!QueryServiceStatusEx(
			serviceHandler,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded)) {
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			return -1;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED) {
			printf("Service stopped successfully, SINGUREL.\n");
			return 0;
		}

		// if timeout

		if (GetTickCount() - dwStartTime > dwTimeOut) {
			printf("Service stop timed out.\n");
				return -1;
		}
	}

	// if the service is running the dependencies must be stopped first.

	stopDependencies(serviceHandler);

	if (!ControlService(
		serviceHandler,
		SERVICE_CONTROL_STOP,
		(LPSERVICE_STATUS)&ssp)) {
		printf("ControlService failed (%d)\n", GetLastError());
		return -1;
	}

	// Wait for the service to stop.

	while (ssp.dwCurrentState != SERVICE_STOPPED)
	{
		Sleep(ssp.dwWaitHint);
		if (!QueryServiceStatusEx(
			serviceHandler,
			SC_STATUS_PROCESS_INFO,
			(LPBYTE)&ssp,
			sizeof(SERVICE_STATUS_PROCESS),
			&dwBytesNeeded)) {
			printf("QueryServiceStatusEx failed (%d)\n", GetLastError());
			return -1;
		}

		if (ssp.dwCurrentState == SERVICE_STOPPED)
			break;

		if (GetTickCount() - dwStartTime > dwTimeOut) {
			printf("Wait timed out\n");
			return -1;
		}
	}
	printf("Service stopped successfully\n");

	return 0;
}

int suspendService(LPCTSTR  serviceName) {

	return 0;

}

int restartService(LPCTSTR  serviceName) {

	return 0;

}

int main(int args, wchar_t*argv[]) {

	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                    // local computer
		SERVICES_ACTIVE_DATABASE,                    // ServicesActive database 
		SC_MANAGER_ALL_ACCESS);  // full access rights 

	if (NULL == schSCManager) {
		printf("OpenSCManager failed (%d)\n", GetLastError());
		return -1;
	}

	if (args != 3) {
		printf("wrong number of arguments, piece of shit!!\n\n");
		return -1;
	}


	//wprintf(L"numele vietii: %s\n", name);
	int choice = _wtoi(argv[2]);
	switch (choice) {
		case 0:
			stopService((LPCTSTR)argv[1]);
			break;
		case 1:
			suspendService((LPCTSTR)argv[1]);
			break;
		case 2:
			restartService((LPCTSTR)argv[1]);
			break;
	}

	return 0;
}