#include <Windows.h>
#include <stdio.h>

int main(int argc, wchar_t* argv[]) {

	if (argc != 2) {
		printf("wrong number of arguments, pierce of shit!!!\n");
		return -1;
	}

	WIN32_FIND_STREAM_DATA stream;
	HANDLE firstStream = FindFirstStreamW(
		argv[1],
		FindStreamInfoStandard,
		&stream,
		0);

	if (firstStream == INVALID_HANDLE_VALUE) {
		printf("could not create firstStreamHandle (%d)\n", GetLastError());
		return -1;
	}

	wprintf(L"Stream Name is %s\n\n", stream.cStreamName);

	while (1) {
		int err = FindNextStreamW(
			firstStream,
			&stream);
		if (!err) {
			if (GetLastError() == ERROR_HANDLE_EOF) {
				printf("no more streams\n\n");
			}
			else {
				printf("erroare");
			}
			break;
		}
		else {
			wprintf(L"Stream Name is %s\n\n", stream.cStreamName);
		}
	}

}