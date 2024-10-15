#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>

#define SIZE 255
#define PORT 22001

int clients_amount;
int* clients_list;
int sockfd = -1;

struct buf {
	int size;
	char* buf;
};

struct buf buf;
pthread_mutex_t mutex;


void ins_client(int socket) 
{

	clients_list = (int*)realloc(clients_list, sizeof(int) * (clients_amount + 1));
	clients_list[clients_amount] = socket;
	clients_amount++;

}

void del_client(int socket) 
{

	pthread_mutex_lock(&mutex);
	close(socket);
	printf("client disconnected (id - %d)\n", socket);

	if (clients_amount == 1)
	{
		clients_list = NULL;
		clients_amount--;
		pthread_mutex_unlock(&mutex);
		return;
	}

	int* tmp = (int*)malloc(sizeof(int) * (clients_amount - 1));

	for (int i = 0; i < clients_amount; i++)
	{
		if (clients_list[i] != socket)
		{
			tmp[i] = clients_list[i];
		}
	}

	clients_list = (int*)malloc(sizeof(int) * (clients_amount - 1));
	clients_amount--;

	for (int i = 0; i < clients_amount; i++) 
	{
		clients_list[i] = tmp[i];
	}

	pthread_mutex_unlock(&mutex);
}

void* reciever_thread(int* id)
{
	int sock = *(int*)id;
	pthread_detach(pthread_self());

	while (1)
	{
		int size = 0;
		char* msg = NULL;

		if (recv(sock, &size, sizeof(int), MSG_WAITALL) == -1)
		{
			printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));
		}

		if (size == -1)
		{
			del_client(sock);
			pthread_exit(0);
		}

		msg = (char*)realloc(msg, size);
		if (recv(sock, msg, size, MSG_WAITALL) == -1)
			printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));

		pthread_mutex_lock(&mutex);


		buf.size += size;
		buf.buf = (char*)realloc(buf.buf, buf.size);
		strcat(buf.buf, msg);

		msg = strtok(msg, "\n");
		printf("msg from id %d: %s\n", sock, msg);

		pthread_mutex_unlock(&mutex);
	}
}

void send_data(int status)
{

	if (status == -1)
	{
		for (int i = 0; i < clients_amount; i++)
		{
			send(clients_list[i], &status, sizeof(int), 0);
		}
		return;
	}

	pthread_mutex_lock(&mutex);

	printf("\x1b[32m60 seconds is over. Sending data\n\x1b[0m");
	for (int i = 0; i < clients_amount; i++)
	{
		send(clients_list[i], &buf.size, sizeof(int), 0);
		send(clients_list[i], buf.buf, buf.size, 0);
	}
	buf.buf = NULL;
	buf.size = 0;

	pthread_mutex_unlock(&mutex);
}

void* timeout_sender()
{
	pthread_detach(pthread_self());

	while (1)
	{
		sleep(10);
		if (buf.buf)
		{
			send_data(0);
		}
		else
		{
			printf("\x1b[33mBuf is empty. Nothing to send\n\x1b[0m");
		}
	}
	
}

void sig_handler(int signum)
{
	send_data(-1);

	for (int i = 0; i < clients_amount; i++)
	{
		del_client(clients_list[i]);
	}

	close(sockfd);
	printf("\n\x1b[34mClosing...\x1b[0m\n");
	exit(0);
}

int main()
{
	struct sockaddr_in addr;
	clients_amount = 0;
	clients_list = NULL;
	buf.buf = NULL;
	buf.size = 0;
	pthread_mutex_init(&mutex, NULL);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
		printf("\x1b[31mCreation error\nError: %s\n\x1b[0m", strerror(errno));

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	listen(sockfd, 1);

	signal(SIGINT, sig_handler);

	pthread_t recieve_msg;
	pthread_t send_buf;

	pthread_create(&send_buf, NULL, (void*)timeout_sender, NULL);

	int client_id = 0;

	while (1)
	{
		if ((client_id = accept(sockfd, NULL, NULL)) == -1)
		{
			perror("accept\n");
			exit(3);
		}
		printf("New connection (id - %d)\n", client_id);

		pthread_mutex_lock(&mutex);
		ins_client(client_id);

		pthread_create(&recieve_msg, NULL, (void*)reciever_thread, &client_id);
		pthread_mutex_unlock(&mutex);
	}

}

