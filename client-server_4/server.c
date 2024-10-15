#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdbool.h>

#define SEMNUMS 5

struct sembuf lock_sem = { 0, -1, IPC_NOWAIT };
struct sembuf open_sem = { 0, 1, IPC_NOWAIT };
struct semid_ds semid_ds;

union semun {
	int val;
	struct semid_ds* buf;
	ushort* array;
} semopts;

typedef struct {
	int values[SEMNUMS];
} arr_t;

arr_t* arr;

bool isArrFree(int semid)
{
	for (int i = 0; i < SEMNUMS; i++) {
		if (semctl(semid, i, GETVAL, 0) == 0)
		{
			return false;
		}
	}
}

void printArrValues(int semid)
{
	while (!isArrFree(semid))
	{
		//wait
	}

	for (int i = 0; i < SEMNUMS; i++) {
		lock_sem.sem_num = i;
		semop(semid, &lock_sem, 1);
	}

	for (int i = 0; i < SEMNUMS; i++) {
		printf("[%d]", arr->values[i]);
	}
	printf("\n");

	for (int i = 0; i < SEMNUMS; i++) {
		open_sem.sem_num = i;
		semop(semid, &open_sem, 1);
	}
}

int main()
{
	int semArrID;
	int shmArrID;
	key_t keySem;
	key_t keyShm;
	int i = 0;
	int* adress;

	keySem = ftok(".", 'q');

	semArrID = semget(keySem, SEMNUMS, IPC_CREAT | IPC_EXCL | 0666);
	if (semArrID == -1) {
		printf("sem create error \n");
		return 0;
	}

	for (int s = 0; s < SEMNUMS; s++) {
		semctl(semArrID, s, SETVAL, 1);
	}

	keyShm = ftok(".", 'r');

	shmArrID = shmget(keyShm, sizeof(arr_t), IPC_CREAT | IPC_EXCL | 0666);
	if (shmArrID == -1) {
		printf("mem create error \n");
		return 0;
	}

	arr = (arr_t*)shmat(shmArrID, 0, 0);
	if (arr == NULL) {
		printf("shm attach error \n");
		return 0;
	}

	for (int a = 0; a < SEMNUMS; a++) {
		arr->values[a] = 0;
	}


	while (i < 10) {
		printArrValues(semArrID);
		sleep(3);
		i++;
	}

	semctl(semArrID, 0, IPC_RMID, 0);
	shmdt(arr);
	shmctl(shmArrID, IPC_RMID, 0);
	return 1;
}