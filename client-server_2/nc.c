#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/uio.h>

#define SIZE 255
#define PORT 22000
#define IP "5.18.208.86" 

#define DATA_IS_NOT_READY 0
#define DATA_IS_READY 1
#define WAITING_DATA 2
#define SENDING_MESSAGE 3


struct get_msg {
    int ready;
    int size;
};

int status;

int main()
{
    char* buf = NULL;
    struct sockaddr_in addr;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        printf("\x1b[31mCreation error\nError: %s\n\x1b[0m", strerror(errno));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // addr.sin_addr.s_addr = inet_addr(IP);

    int n = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (n == -1)
        printf("\x1b[31mConnecting error\nError: %s\n\x1b[0m", strerror(errno));
    //printf("%d\n", n);

    char* list_str = NULL;
    char msg[3] = "hi\n";
    while (1)
    {
        int size = 0;
        while (1)
        {
            status = WAITING_DATA;
            send(sockfd, &status, sizeof(int), 0);

            int n = recv(sockfd, &size, sizeof(int), MSG_WAITALL);
            
            printf("%d\n", size);

            if (n == -1)
                printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));

            //printf("status: %d\n", status);

            if (size)
                break;

            status = SENDING_MESSAGE;
            send(sockfd, &status, sizeof(int), 0);

            buf = (char*)realloc(buf, 255);

            printf("\x1b[36mMy message:\x1b[0m ");
            //fgets(buf, SIZE, stdin);
            strcpy(buf, msg);
            int length = strlen(buf);

            // printf("%d - %s\n", length, buf);
            printf("sending: %s, %d\n", buf, length);

            send(sockfd, &length, sizeof(int), 0);
           /* if (n == -1)
                printf("\x1b[31mSending error\nError: %s\n\x1b[0m", strerror(errno));*/

            //printf("sendind %s, size - %d\n", buf, length);

            send(sockfd, buf, length, 0);
            /*if (n == -1)
                printf("\x1b[31mSending error\nError: %s\n\x1b[0m", strerror(errno));*/
            sleep(3);
        }


        //recv(sockfd, &size, sizeof(int), MSG_WAITALL);
        list_str = (char*)malloc(size);
        n = recv(sockfd, list_str, size, MSG_WAITALL);
        if (n == -1)
            printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));
       
        printf("======================\n");
        printf("msg: %s, %d\n", list_str, size);
        printf("======================\n");

        list_str = NULL;
    }
    close(sockfd);
    return 1;
}