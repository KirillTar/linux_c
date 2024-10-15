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
#define IP "25.59.56.69" 

int sockfd = -1;
pthread_mutex_t mutex;
char* msg = NULL;

int create_connect() 
{
    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        printf("\x1b[31mCreation error\nError: %s\n\x1b[0m", strerror(errno));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //addr.sin_addr.s_addr = inet_addr(IP);
        
    return connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
}

void sig_handler(int signum)
{
    printf("\n\x1b[34mClosing...\n\x1b[0m");
    int dis = -1;
    send(sockfd, &dis, sizeof(int), 0);
    close(sockfd);
    exit(0);
}

void* reciever()
{
    pthread_detach(pthread_self());

    char* buf = NULL;
    int size = -1;

    while (1)
    {

        if (recv(sockfd, &size, sizeof(int), MSG_WAITALL) == -1)
            printf("\x1b[31mRecieving error\nError: %s\n\x1b[0m", strerror(errno));

        if (size == -1)
        {
            printf("\n\x1b[31mServer destroyed\n\x1b[0m");
            close(sockfd);
            exit(0);
        }

        buf = (char*)malloc(size);

        if (recv(sockfd, buf, size, MSG_WAITALL) == -1)
            printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));

        printf("\n___________________________\n");
        printf("msg: %s", buf);
        printf("\n___________________________\n");
        puts("Enter: ");

        buf = NULL;
    }
    
}

int main()
{   
    msg = (char*)malloc(255);
    pthread_t recieve_buf;

    if (create_connect() == -1)
    {
        printf("\x1b[31mConnecting error\nError: %s\n\x1b[0m", strerror(errno));
        exit(0);
    }

    signal(SIGINT, sig_handler);
    pthread_create(&recieve_buf, NULL, (void*)reciever, NULL);
   
    while (1)
    {
        printf("Enter: ");
        fgets(msg, SIZE, stdin);
        int length = strlen(msg);

        //printf("sending: %s, %d\n", msg, length);

        pthread_mutex_lock(&mutex);

        if (send(sockfd, &length, sizeof(int), 0) == -1)
            printf("\x1b[31mSending error\nError: %s\n\x1b[0m", strerror(errno));

        if (send(sockfd, msg, length, 0) == -1)
            printf("\x1b[31mSending error\nError: %s\n\x1b[0m", strerror(errno));

        pthread_mutex_unlock(&mutex);
    }

}
