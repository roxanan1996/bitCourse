#include <Windows.h>
#include <iostream>
#include <fstream>

using namespace std;

//CRITICAL_SECTION csSync;

int M, N, P;
int **result, **matrix1, **matrix2;

DWORD WINAPI ThreadProc(LPVOID param) {

	int i = *(int*)param;
	int j = *((int*)(param)+1);
	result[i][j] = 0;
	//std::cout << "\n i si j " << i << " " << j << "\n";
	for (int k = 0; k < N; ++k) {
		result[i][j] += matrix1[i][k] * matrix2[k][j];
	}

	return 0;
}

int main() {
	
	std::ifstream input;
	input.open("matrice.txt");

	input >> M >> N >> P;

	result = new int*[M];
	for (int i = 0; i < M; ++i) {
		result[i] = new int[P];
	}

	matrix1 = new int*[M];
	for (int i = 0; i < M; ++i) {
		matrix1[i] = new int[N];
	}

	matrix2 = new int*[N];
	for (int i = 0; i < N; ++i) {
		matrix2[i] = new int[P];
	}

	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			input >> matrix1[i][j];
		}
	}

	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < P; ++j) {
			input >> matrix2[i][j];
		}
	}

	std::cout << "afisare matrix1\n";
	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < N; ++j) {
			std::cout << matrix1[i][j] << " ";
		}
		std::cout << "\n";
	}

	std::cout << "afisare matrix2\n";
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < P; ++j) {
			std::cout << matrix2[i][j] << " ";
		}
		std::cout << "\n";
	}

	const DWORD nbOfThreads = M * P;
	HANDLE *hThr = new HANDLE[nbOfThreads];
	DWORD *dwThread = new DWORD[nbOfThreads];

	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < P; ++j) {
			int *p = new int[2];
			p[0] = i;
			p[1] = j;
			hThr[i + j] = CreateThread(
				NULL,
				0,
				ThreadProc,
				p,
				0,
				&dwThread[i + j]);
		}
	}

	WaitForMultipleObjects(nbOfThreads, hThr, TRUE, INFINITE);

	std::cout << "afisare matrice\n";
	for (int i = 0; i < M; ++i) {
		for (int j = 0; j < P; ++j) {
			std::cout << result[i][j] << " ";
		}
		std::cout << "\n";
	}


	return 0;
}



