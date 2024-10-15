//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <errno.h>
//#include <string.h>
//#include <sys/types.h>
//#include <pthread.h>
//#include <signal.h>
//
//int listenfd = 0, connfd = 0;
//pthread_mutex_t mutex;
//
//int create_connect(int port) {
//
//	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//		perror("\nsocket create error\n");
//		exit(0);
//	}
//
//	struct sockaddr_in serv_addr;
//	memset(&serv_addr, 0, sizeof(serv_addr));
//
//	serv_addr.sin_family = AF_INET;
//	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//	serv_addr.sin_port = htons(port);
//
//	int con = -1;
//	con = bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
//
//	return con;
//}
//
//void* send_text(int* sock) {
//
//	int id = *(int*)sock;
//	printf("%d\n", id);
//
//	pthread_detach(pthread_self());
//	FILE* fp;
//
//	while (1)
//	{
//	/*	char* buf = NULL;
//		char* file_str = NULL;
//		char cmd[] = "";
//		int size = 0;
//		int sum = 0;*/
//
//		int size = 0;
//		int sum = 0;
//		char* msg = NULL;
//		char* file_str = NULL;
//		char* buf = NULL;
//
//		if (recv(id, &size, sizeof(int), MSG_WAITALL) == -1)
//		{
//			printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));
//		}
//
//		/*if (size == -1)
//		{
//			del_client(sock);
//			pthread_exit(0);
//		}*/
//
//		msg = (char*)realloc(msg, size);
//		if (recv(id, msg, size, MSG_WAITALL) == -1)
//			printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));
//
//		/*if (recv(connfd, &size, sizeof(int), MSG_WAITALL) < 0) {
//			printf("\n error!\n");
//			exit(0);
//		}
//		printf("size: %d\n", size);*/
//
//		//cmd = (char*)realloc(cmd, size);
//		//printf("cmd: %s, size: %ld\n", msg, strlen(msg));
//
//		/*recv(connfd, cmd, size, MSG_WAITALL);*/
//
//		pthread_mutex_lock(&mutex);
//		printf("cmd: %s, size: %ld\n", msg, strlen(msg));
//
//		if (strcmp(msg, "discon\n") == 0)
//		{
//			printf("\nclient disconnected\n");
//			close(id);
//			pthread_mutex_unlock(&mutex);
//			pthread_exit(0);
//		}
//
//		fp = popen(msg, "r");
//
//		buf = (char*)realloc(buf, 255);
//
//		while (fgets(buf, 255, fp) != NULL) {
//			sum += strlen(buf);
//			file_str = (char*)realloc(file_str, sum);
//			strncat(file_str, buf, strlen(buf));
//		}
//
//		file_str = (char*)realloc(file_str, sum);
//		send(id, &sum, sizeof(int), 0);
//		send(id, file_str, sum, 0);
//
//		//pclose(fp);
//		pthread_mutex_unlock(&mutex);
//	}
//	
//}
//
//int main(int argc, char* argv[]) {
//	
//	pthread_t sender;
//	pthread_mutex_init(&mutex, NULL);
//
//	system("clear");
//
//	if (argc != 2) {
//		printf("\n missed args: Enter \"./output + port\" \n");
//		return 1;
//	}
//
//	int port = atoi(argv[1]);
//
//	if (port == 0) {
//		port = 22222;
//	}
//	
//	if (create_connect(port) < 0) {
//		perror("\nConnection error!\n");
//		exit(1);
//	}
//
//	listen(listenfd, 1);
//	printf("Server adress: anyaddr, port: %d\n", port);
//
//	int id;
//	while (1)
//	{
//		id = accept(listenfd, (struct sockaddr*)NULL, NULL);
//		printf("new con\n");
//		pthread_create(&sender, NULL, (void*)send_text, &id);
//	}
//	
//
//	//close(connfd);
//	printf("\nserver closed\n");
//
//}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#define PORT 4040 

int sockfd = 0;
pthread_mutex_t mutex;

void create_connect()
{
    struct sockaddr_in addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        printf("\x1b[31mCreation error\nError: %s\n\x1b[0m", strerror(errno));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int con;
    if ((con = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr))) < 0)
        printf("\x1b[31mBinding error\nError: %s\n\x1b[0m", strerror(errno));
}

void* send_text(int* sock)
{
    pthread_detach(pthread_self());

    FILE* fp;
    int id = *(int*)sock;
    // printf("\x1b[1mClient %d connected\n\x1b[0m", id);

    while (1)
    {
        int size = 0;
        int sum = 0;
        char* msg = NULL;
        char* buf = NULL;
        char* file_str = NULL;

        if (recv(id, &size, sizeof(int), MSG_WAITALL) < 0)
            printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));

        msg = (char*)realloc(msg, size);
        if (recv(id, msg, size, MSG_WAITALL) < 0)
            printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));

        pthread_mutex_lock(&mutex);
        printf("\x1b[36mCommand from client %d:\x1b[0m %s", id, msg);

        if (strcmp(msg, "quit\n") == 0)
        {
            printf("\x1b[31mClient %d disconnected\n\x1b[0m", id);
            close(id);
            pthread_mutex_unlock(&mutex);
            pthread_exit(0);
        }

        fp = popen(msg, "r");
        buf = (char*)realloc(buf, 255);

        while (fgets(buf, 255, fp) != NULL)
        {
            sum += strlen(buf);
            file_str = (char*)realloc(file_str, sum);
            strncat(file_str, buf, strlen(buf));
        }

        if (send(id, &sum, sizeof(int), 0) < 0)
            printf("\x1b[31mSending error\nError: %s\n\x1b[0m", strerror(errno));
        if (send(id, file_str, sum, 0) < 0)
            printf("\x1b[31mSending error\nError: %s\n\x1b[0m", strerror(errno));

        pthread_mutex_unlock(&mutex);
    }
    pclose(fp);
}

int main()
{
    pthread_t sender;
    pthread_mutex_init(&mutex, NULL);

    system("clear");
    create_connect();
    listen(sockfd, 1);

    int id;
    while (1)
    {
        if ((id = accept(sockfd, (struct sockaddr*)NULL, NULL)) < 0)
            printf("\x1b[31mAccepting error\nError: %s\n\x1b[0m", strerror(errno));

        printf("\x1b[32mClient %d connected\n\x1b[0m", id);
        pthread_create(&sender, NULL, (void*)send_text, &id);
    }

    close(sockfd);
}
