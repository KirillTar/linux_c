#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

int auto1 = 0;
int auto2 = 0;
int auto3 = 0;
bool firstGreen = false;
bool secondGreen = false;
bool thirdGreen = false;

void print() {
	printf("       %d\n", auto3);
	
	if (thirdGreen) {
		printf("       \033[1;32m|\033[0m\n");
	}
	else {
		printf("       \033[1;31m|\033[0m\n");
	}
	printf("       |\n");

	if (firstGreen) {
		printf("%d\033[1;32m--\033[0m---", auto1);
	}
	else
		printf("%d\033[1;31m--\033[0m---", auto1);

	if (secondGreen) {
		printf("---\033[1;32m--\033[0m%d\n", auto2);
	}
	else
		printf("---\033[1;31m--\033[0m%d\n", auto2);
}

void firstRoad(int signum) {
	auto1++;
	system("clear");
	print();
}

void secondRoad(int signum) {
	auto2++;
	system("clear");
	print();
}

void thirdRoad(int signum) {
	auto3++;
	system("clear");
	print();
}


int main(void)
{
	pid_t pid;
	srand(time(NULL));
	int secs1 = 0;
	int secs2 = 0;
	int secs3 = 0;
	int k = 0;
	int m = 0;
	int n = 0;
	int i = 0;

	pid = fork();
	if (pid == 0 && i == 0) {
		while (1) {
			secs1 = rand() % 2 + 1;
			while (k < secs1) {			
				sleep(1);
				k++;
			}
			k = 0;
			kill(getppid(), SIGUSR1);
		}
	}

	pid = fork();
	i++;
	
	if (pid == 0 && i == 1) {
		while (1) {
			srand(time(NULL));
			secs2 = rand() % 2 + 1;
			while (m < secs2) {
				sleep(1);
				m++;
			}
			m = 0;
			kill(getppid(), SIGUSR2);
		}
	}

	pid = fork();
	i++;

	if (pid == 0 && i == 2) {
		while (1) {
			srand(time(NULL));
			secs3 = rand() % 2 + 1;
			while (n < secs3) {
				sleep(1);
				n++;
			}
			n = 0;
			kill(getppid(), SIGPIPE);
		}
	}

	if(pid != 0) {
		int light = 0;
		struct sigaction road1;
		memset(&road1, 0, sizeof(road1));

		struct sigaction road2;
		memset(&road2, 0, sizeof(road2));


		struct sigaction road3;
		memset(&road3, 0, sizeof(road3));
		while (1) {
			
			road1.sa_handler = firstRoad;
			road1.sa_flags = SA_RESTART;
			sigaction(SIGUSR1, &road1, 0);

			road2.sa_handler = secondRoad;
			road2.sa_flags = SA_RESTART;
			sigaction(SIGUSR2, &road2, 0);

			road3.sa_handler = thirdRoad;
			road3.sa_flags = SA_RESTART;
			sigaction(SIGPIPE, &road3, 0);

			light = (int)(secs1 / 2);
			if (light < 8) { light = 8; }

			firstGreen = true;
			secondGreen = false;
			thirdGreen = false;

			for (int j = 0; j < light * 2; j++) {
				auto1--;
				if (auto1 < 0) { auto1 = 0; }
				print();
				usleep(500000);
				system("clear");
			}


			light = (int)(secs2 / 2);
			if (light < 8) { light = 8; }

			firstGreen = false;
			secondGreen = true;
			thirdGreen = false;
			
			for (int j = 0; j < light * 2; j++) {
				auto2--;
				if (auto2 < 0) { auto2 = 0; }
				print();
				usleep(500000);
				system("clear");
			}


			light = (int)(secs3 / 2);
			if (light < 8) { light = 8; }

			firstGreen = false;
			secondGreen = false;
			thirdGreen = true;

			for (int j = 0; j < light * 2; j++) {
				auto3--;
				if (auto3 < 0) { auto3 = 0; }
				print();
				usleep(500000);
				system("clear");
			}
		}
	}

	return 0;
}
