#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>

int z = 1;

typedef struct {
	int columns;
	int rows;
	int resSize;
	int** array1;
	int** array2;
	int** resArr;
} pthrData;

int write_log(const char* path, pthread_t tid, const char* msg)
{
	char buf[50];
	time_t timer;
	struct tm* TIME;

	FILE* log = fopen("log.txt", "a+");

	timer = time(NULL);
	TIME = localtime(&timer);
	strftime(buf, 50, "%d.%m.%Y %H:%M:%S, %A", TIME);
	fprintf(log, "%s: tid = %ld || %s\n", buf, tid, msg);
	fclose(log);
}

void* threadFunc(void* thread_data) {
	int q = 5000;
	pthrData* data = (pthrData*)thread_data;
	pthread_t tid = pthread_self();
	write_log("log.txt", tid, ": thread created");

	for (int j = 0; j < data->resSize; j++) 
	{
		for (int k = 0; k < data->columns; k++) {
		data->resArr[data->rows][j] += data->array1[data->rows][k] * data->array2[k][j];
		}
	}

	write_log("log.txt", tid, ": result row calculated");

	z++;
	q *= z * 2;
	usleep(q);
	printf("tid: %ld  --  ", tid);

	for (int j = 0; j < data->resSize; j++)
	{
		printf("%3d ", data->resArr[j][data->rows]);
	}
	printf("\n");

	sleep(10);
}

int main() {

	int N;
	int M;
	printf("Streams (N size):\n");
	scanf("%d", &N);
	printf("Columns (M size):\n");
	scanf("%d", &M);

	pthread_t tid = pthread_self();
	write_log("log.txt", tid, ": main start");
	int** matrix1 = (int**)malloc(N * sizeof(int*)); //N strok (matrix1[N][M]
	int** matrix2 = (int**)malloc(M * sizeof(int*)); //m strok (matrix2[M][N]
	int** resultMatrix = (int**)malloc(N * sizeof(int*)); //n strok

	
	for (int i = 0; i < N; i++) {
		matrix1[i] = (int*)malloc(M * sizeof(int));
		resultMatrix[i] = (int*)malloc(N * sizeof(int));
	}

	for (int i = 0; i < M; i++) {
		matrix2[i] = (int*)malloc(N * sizeof(int));
	}

	srand(time(NULL));
	
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++) {
			matrix1[i][j] = rand() % 100 - 200;
		}
	}

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++) {
			matrix2[i][j] = rand() % 10;
		}
	}

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			resultMatrix[i][j] = 0;
		}
	}

	printf("\n");
	printf("matrix A:\n");

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < M; j++)
		{
			printf("%d  ", matrix1[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	printf("matrix B:\n");

	for (int i = 0; i < M; i++) {
		for (int j = 0; j < N; j++)
		{
			printf("%d  ", matrix2[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	
	pthread_t* threads = (pthread_t*)malloc(N * sizeof(pthread_t));
	pthrData* threadData = (pthrData*)malloc(N * sizeof(pthrData));

	for (int i = 0; i < N; i++) {
		threadData[i].resSize = N;
		threadData[i].rows = i;
		threadData[i].columns = M;
		threadData[i].array1 = matrix1;
		threadData[i].array2 = matrix2;
		threadData[i].resArr = resultMatrix;
		pthread_create(&(threads[i]), NULL, threadFunc, &threadData[i]);
		
	}

	usleep(800000);
	write_log("log.txt", tid, ": main end");

	for (int i = 0; i < N; i++)
	{
		pthread_join(threads[i], NULL);
	}

	free(threads);
	free(threadData);
	for (int i = 0; i < N; i++) {
		free(matrix1[i]);
		free(resultMatrix[i]);
	}

	for (int i = 0; i < M; i++) {
		free(matrix2[i]);
	}

	free(matrix1);
	free(matrix2);
	free(resultMatrix);
	return 0;
}
