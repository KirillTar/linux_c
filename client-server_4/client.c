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

bool isArrElFree(int semid, int index)
{
	if (semctl(semid, index, GETVAL, 0) == 0)
	{
		return false;
	}
}

void changeArrValues(int semid, int index, int num)
{
	while (!isArrElFree(semid, index))
	{
		//wait
	}

	lock_sem.sem_num = index;
	semop(semid, &lock_sem, 1);

	arr->values[index] = num;
	//sleep(7);

	open_sem.sem_num = index;
	semop(semid, &open_sem, 1);
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

	semArrID = semget(keySem, SEMNUMS, IPC_CREAT | 0666);
	if (semArrID == -1) {
		printf("sem create error \n");
		return 0;
	}


	keyShm = ftok(".", 'r');

	shmArrID = shmget(keyShm, sizeof(arr_t), IPC_CREAT | 0666);
	if (shmArrID == -1) {
		printf("mem create error \n");
		return 0;
	}


	arr = (arr_t*)shmat(shmArrID, 0, 0);

	if (arr == NULL) {
		printf("shm attach error \n");
		return 0;
	}

	int index;
	int num;

	while (i < 3) {

		for (;;) {
			printf("Enter index: \n");
			scanf("%d", &index);

			if (index < 0 || index >= SEMNUMS) {
				printf("Invalid index! \n");
			}
			else { break; }
		}

		printf("Enter num: \n");
		scanf("%d", &num);

		changeArrValues(semArrID, index, num);
		i++;
	}

	shmdt(arr);
	return 1;
}