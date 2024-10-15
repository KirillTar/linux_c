#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

FILE* output;
int z = 0;

void fill(int N) {
	
}

void* multy(int N){

	output = fopen("log.txt", "w");

	pthread_t tid = pthread_self();
	printf("tid: %ld ", tid);

	int** A = (int**)malloc(N * sizeof(int*));
	int** B = (int**)malloc(N * sizeof(int*));
	int** C = (int**)malloc(N * sizeof(int*));

	for (int i = 0; i < N; i++)
	{
		A[i] = (int*)malloc(N * sizeof(int));
		B[i] = (int*)malloc(N * sizeof(int));
		C[i] = (int*)malloc(N * sizeof(int));
	}

	srand(time(NULL));
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
		{
			A[i][j] = rand() % 10;
			B[i][j] = rand() % 10;
		}
	}
	printf("matrix A:\n");

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
		{
			printf("%d  ", A[i][j]);
		}
		printf("\n");
	}
		printf("\n");
		printf("matrix B:\n");

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
		{
			printf("%d  ", B[i][j]);
		}
		printf("\n");
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
		{
			C[i][j] = 0;
			for (int k = 0; k < N; k++)
				C[i][j] += A[i][k] * B[k][j];
		}
	}

	z++;

	printf("\n");
	fprintf(output, "\n");
	printf("matrix C (%d):\n", z);

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++)
		{
			printf("%3d ", C[i][j]);
			fprintf(output, "%3d ", C[i][j]);
		}
		printf("\n");
		fprintf(output, "\n");
	}
}

void main() {
	int N;
	int M;
	printf("Matrix size:\n");
	scanf("%d", &N);
	printf("Streams amount:\n");
	scanf("%d", &M);
	int l = 0;

	pthread_t* th = (pthread_t*)malloc(M * sizeof(pthread_t));

	for (l = 0; l < M; l++) {
		pthread_create(&(th[l]), NULL, multy(N), NULL);
	}

	for (l = 0; l < M; l++) {
		pthread_join(th[l], NULL);
	}

	fclose(output);
}
