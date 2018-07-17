// buble_sort_parallel_lr3.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <omp.h>
#include <vector>

using namespace std;

const int n = 100000;
const int p = 4;
int A[n];
int A2[n];
int B[n];
int C[p*p];
int numbers[p * 2];

void init() {
	for (int i = 0; i < n; i++) {
		int k = rand();
		A[i] = k;
		A2[i] = k;
	}
}
void bubble_sort(int a[], int first, int last) {
	for (int index = last; index >= first; index--)
		for (int j = first; j < index; j++)
			if (a[j] > a[j + 1])
			{
				int tmp = a[j];
				a[j] = a[j + 1];
				a[j + 1] = tmp;
			}
}
int _tmain(int argc, _TCHAR* argv[])
{
	init();

	__int64 start, end, tps;
	QueryPerformanceFrequency((LARGE_INTEGER *)&tps);
	QueryPerformanceCounter((LARGE_INTEGER *)&start);

	const int k = n / p;

	const int m = n / (p*p);
	int i = 0;

#pragma omp parallel shared(A, k, m, i)
#pragma omp for
	for (i = 0; i < p; i++) bubble_sort(A, k*i, k*(i + 1) - 1);

#pragma omp barrier
	if (omp_get_thread_num() == 0) {
		int number = 0;
		for (int j = 0; j < p; j++)
			for (int index = 0; index <= (p - 1)*m; index = index + m) {
				C[number] = A[j*k + index];
				number++;
			}
		bubble_sort(C, 0, p*p - 1);

		int last_elem = -1;
		number = 0;
		int num = 0;

		for (int j = p + p / 2 - 1; j <= (p - 1)*p + p / 2 - 1; j = j + p) {
			numbers[num] = number;
			num++;
			if (last_elem != -1) {
				for (int index = 0; index < n; index++) {
					if ((A[index]>C[last_elem]) && (A[index] <= C[j])) {
						B[number] = A[index];
						number++;
					}
				}
				numbers[num] = number;
				num++;

				//numbers[num] = number;
				//num++;
				if (j == (p - 1)*p + p / 2 - 1) {
					for (int index = 0; index < n; index++) {
						if (A[index] > C[j]) {
							B[number] = A[index];
							number++;
						}
					}
				}

			}
			else {
				for (int index = 0; index < n; index++) {
					if (A[index] <= C[j]) {
						B[number] = A[index];
						number++;
					}
				}
			}
			numbers[num] = number;
			num++;
			last_elem = j;
		}

	}
#pragma omp barrier
#pragma omp for
	for (i = 0; i < p; i++) {
		bubble_sort(B, numbers[i * 2], numbers[i * 2 + 1] - 1);
	}

	QueryPerformanceCounter((LARGE_INTEGER *)&end);
	cout << "Time for parallel sort = " << (double)(end - start) / tps << endl;

	QueryPerformanceFrequency((LARGE_INTEGER *)&tps);
	QueryPerformanceCounter((LARGE_INTEGER *)&start);

	bubble_sort(A2, 0, n - 1);
	QueryPerformanceCounter((LARGE_INTEGER *)&end);
	cout << "Time for simple bubble sort = " << (double)(end - start) / tps << endl;

	system("pause");
	return 0;
}



