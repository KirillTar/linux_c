#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define SIZE 255
#define PORT 22000
#define WAITING_DATA 2
#define SENDING_MESSAGE 3
#define CONNECTED 4
#define RECIEVING 5

int sockfd = -1;

struct client {
	int id;
	int clients_status;
	char* buf;
	char* msg;
};

struct client* clients;
int clients_amount;



int get_max_socket(struct client*, int sockfd) {

	int max_socket = sockfd;

	for (int i = 0; i < clients_amount; i++)
	{
		if (clients[i].id > max_socket)
		{
			max_socket = clients[i].id;
		}
	}

	return max_socket;
}

void ins_client(int sock) {

	//if (clients_amount == 0) {
	//	/*clients.clients_arr = (int*)malloc(sizeof(int));
	//	clients.clients_status = (int*)malloc(sizeof(int));
	//	clients.buf = (char**)malloc(sizeof(char*));

	//	clients.buf[0] = (char*)malloc(255);
	//	strcpy(clients.buf[0], "");
	//	clients.clients_status[0] = CONNECTED;
	//	clients.clients_arr[0] = sock;*/

	//	clients = (struct client*)realloc(clients, sizeof(struct client) * (clients_amount + 1));
	//	clients[0].id = sock;
	//	clients[0].clients_status = CONNECTED;
	//	clients[0].buf = (char*)malloc(255*5);
	//	strcpy(-jclients[0].buf, "");

	//	clients_amount++;
	//	return;
	//}
	
	//struct client* tmp = (struct client*)malloc(sizeof(struct client) * clients_amount);
	/*int* tmp1 = (int*)malloc(clients.clients_amount * sizeof(int));
	char** tmpbuf = (char**)malloc(clients.clients_amount * sizeof(char*));*/

	/*for (int i = 0; i < clients_amount; i++) {
		tmp[i].id = clients[i].id;
		tmp[i].clients_status = clients[i].clients_status;
		tmp[i].buf = (char*)malloc(255*5);
		strcpy(tmp[i].buf, clients[i]buf);
	}*/

	/*clients.clients_arr = (int*)malloc((clients.clients_amount + 1) * sizeof(int));
	clients.clients_status = (int*)malloc((clients.clients_amount + 1) * sizeof(int));
	clients.buf = (char**)malloc((clients.clients_amount + 1) * sizeof(char*));*/
	clients = (struct client*)realloc(clients, sizeof(struct client) * (clients_amount + 1));

	//for (int i = 0; i < clients_amount; i++) {
	//	clients[i].id = tmp[i].id;
	//	clients[i].clients_status = tmp[i].clients;
	//	strcpy(clients.buf[i], tmp[i]);
	//}

	clients[clients_amount].id = sock;
	clients[clients_amount].clients_status = CONNECTED;
	clients[clients_amount].buf = (char*)malloc(255*5);
	strcpy(clients[clients_amount].buf, "");

	clients_amount++;

}

void del_client(int index) {

	struct client* tmp = (struct client*)malloc(sizeof(struct client) * clients_amount);
	//int* tmp1 = (int*)malloc((clients.clients_amount - 1) * sizeof(int));

	for (int i = 0; i < clients_amount; i++)
	{
		if (i != index)
		{
			tmp[i].id = clients[i].id;
			tmp[i].clients_status = clients[i].clients_status;
			tmp[i].buf = (char*)malloc(255 * 5);
			strcpy(tmp[i].buf, clients[i].buf);
		}
	}

	clients = (struct client*)malloc(sizeof(struct client) * (clients_amount - 1));

	for (int i = 0; i < clients_amount; i++) {

		clients[i].id = tmp[i].id;
		clients[i].clients_status = tmp[i].clients_status;
		clients[i].buf = (char*)malloc(255 * 5);
		strcpy(clients[i].buf, tmp[i].buf);
	}

	clients_amount--;

}

int main()
{
	char buf[SIZE];
	char bufToSend[SIZE][10];
	struct sockaddr_in addr;
	clients_amount = 0;
	clients = NULL;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		printf("\x1b[31mCreation error\nError: %s\n\x1b[0m", strerror(errno));

	fcntl(sockfd, F_SETFL, O_NONBLOCK);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int n = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	listen(sockfd, 1);

	int send_size = 0;
	int sum = 0;
	char* istr = NULL;
	char* list_str = NULL;

	while (1)
	{
		clock_t start, end;
		time(&start);

		while((difftime(time(&end), start)) < 10)
		{
			//printf("diftime = %f\n", difftime(time(&end), start));
			fd_set readset;
			FD_ZERO(&readset);
			FD_SET(sockfd, &readset);

			struct timeval timeout;
			timeout.tv_sec = 30;
			timeout.tv_usec = 0;

			
			for (int i = 0; i < clients_amount; i++)
			{
				FD_SET(clients[i].id, &readset);
			}


			int max = get_max_socket(clients, sockfd);
			// printf("%d - %d\n", max, sockfd);
			if (select(max + 1, &readset, NULL, NULL, &timeout) <= 0)
			{
				perror("No one is sending messages for 60 seconds");
				exit(3);
			}

			if (FD_ISSET(sockfd, &readset))
			{
				int sock = accept(sockfd, NULL, NULL);
				if (sock < 0)
				{
					perror("accept");
					exit(3);
				}
				printf("New connection\n");

				//fcntl(sock, F_SETFL, O_NONBLOCK);

				ins_client(sock);
			}

			list_str = NULL;
			int size = 0;

			for (int i = 0; i < clients_amount; i++)
			{
				if (FD_ISSET(clients[i].id, &readset))
				{
					//printf("\n==================\n\n");

					if (clients[i].clients_status != RECIEVING)
					{
						if (recv(clients[i].id, &clients[i].clients_status, sizeof(int), MSG_WAITALL) < 0)
						{
							printf("client disconnected\n");
							del_client(clients[i].id);
							break;
						}
						//printf("1 status: %d\n", clients[i].clients_status);
					}
					

					if (clients[i].clients_status == WAITING_DATA || clients[i].clients_status == CONNECTED)
					{
						//printf("2 status: %d\n", clients.clients_status[i]);
						size = strlen(clients[i].buf);
						send(clients[i].id, &size, sizeof(int), 0);


						if (strlen(clients[i].buf)) {
							printf("sending to %d, size: %ld, str: %s\n", clients[i].id, strlen(clients[i].buf), clients[i].buf);
							send(clients[i].id, clients[i].buf, size, 0);
							strcpy(clients[i].buf, "");
						}
						//recv(clients.clients_arr[i], &clients.clients_status[i], sizeof(int), MSG_WAITALL);
						break;
					}

					if (clients[i].clients_status == RECIEVING)
					{
						//printf("3 status: %d\n", clients.clients_status[i]);
						n = recv(clients[i].id, &size, sizeof(int), MSG_WAITALL);
						if (n == -1)
							printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));

						//printf("size recieved: %d\n", size);

						list_str = (char*)realloc(list_str, size);
						
						printf("check: %s\n", list_str);

						n = recv(clients[i].id, list_str, size, MSG_WAITALL);
						if (n == -1)
						{
							printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));
							close(clients[i].id);
							del_client(i);
							break;
						}

						printf("recieved msg: %s, size: %d\n", list_str, size);

						if (size)
						{
							sum += size;
							istr = (char*)realloc(istr, sum);
							strncat(istr, list_str, size);
							list_str = NULL;
							clients[i].clients_status = WAITING_DATA;

							printf("buf: %s\n", istr);
						}				
						//break;
					}		

					if (clients[i].clients_status == SENDING_MESSAGE)
					{
						clients[i].clients_status = RECIEVING;
						//printf("4 status: %d\n", clients.clients_status[i]);
					}

					//printf("getted from %d, status: %d\n", clients.clients_arr[i], clients.clients_status[i]);
				}

			}
		}

		printf("\x1b[1m\nOne minute left. Sending messages (..)\n\x1b[0m");
		//printf("!!%d - %s\n", sum, istr);


		for (int i = 0; i < clients_amount; i++)
		{
			
			//printf("sending to %d, status: %d\n", clients.clients_arr[i], clients.clients_status[i]);

			if (clients[i].clients_status == CONNECTED)
			{
				recv(clients[i].id, &clients[i].clients_status, sizeof(int), MSG_WAITALL);
			}
		
			
			printf("%s, size: %ld\n", clients[i].buf, sum + strlen(clients[i].buf));
			clients[i].buf = (char*)realloc(clients[i].buf, sum + strlen(clients[i].buf));
			strcpy(clients[i].buf, istr);
			//clients.clients_status[i] = WAITING_DATA;
		}
		istr = NULL;
		list_str = NULL;
		sum = 0;
		printf("=============================\n");
			
	}
}