//#include <sys/socket.h>
//#include <sys/types.h>
//#include <netinet/in.h>
//#include <netdb.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//#include <errno.h>
//#include <string.h>
//#include <arpa/inet.h>
//
//
//int sockfd = -1;
//
//
//int create_connect(int port/*, in_addr_t address*/) {
//
//    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
//        perror("\nsocket create error\n");
//        exit(0);
//    }
//
//    struct sockaddr_in serv_addr;
//    memset(&serv_addr, 0, sizeof(serv_addr));
//
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_port = htons(port);
//    //serv_addr.sin_addr.s_addr = address;
//    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//
//    int con = -1;
//    con = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
//
//    return con;
//}
//
//
//void print_text(char* text) {
//    //system("clear");
//
//    printf("================================\n");
//    printf("%s \n", text);
//    printf("================================\n");
//
//}
//
//
//void get_text() 
//{
//
//    int size = 0;
//    char* res = NULL;
//
//    recv(sockfd, &size, sizeof(int), MSG_WAITALL);
//
//    printf("\n%d\n", size);
//
//    res = (char*)realloc(res, size);
//
//    if (recv(sockfd, res, size, MSG_WAITALL) < 0) 
//    {
//        perror("\nrecv failed!\n");
//        exit(0);
//    }
//
//    printf("\n%d, %s\n", size, res);
//    print_text(res);
//
//}
//
//
//void send_command() {
//
//    while (1)
//    {
//        //system("clear");
//        char* str = NULL;
//        str = (char*)realloc(str, 255);
//        int size = 0;
//
//        printf("Enter command: ");
//        fgets(str, 255, stdin);
//
//        if (strcmp(str, "discon\n") == 0)
//        {
//            size = 7;
//            send(sockfd, &size, sizeof(int), 0);
//            send(sockfd, str, size, 0);
//            close(sockfd);
//            exit(0);
//        }
//
//        size = strlen(str);
//
//        printf("cmd: %s, size: %ld", str, strlen(str));
//
//        send(sockfd, &size, sizeof(int), 0);
//        send(sockfd, str, size, 0);
//
//        get_text();
//
//        printf("\nPress ENTER to exit...\n");
//        getchar();
//    }
//    
//}
//
//
//int main(int argc, char* argv[]) {
//
//    system("clear");
//
//    if (argc != 2) {
//        printf("\n missed args: Enter \"./output + port\" \n");
//        return 1;
//    }
//
//    int port = atoi(argv[1]);
//
//    if (port == 0) {
//        port = 22222;
//    }
//
//    //in_addr_t address = inet_addr(argv[2]);
//
//    if (create_connect(port/*, address*/) < 0) {
//        printf("\nConnection error!\n");
//        return 1;
//    }
//    
//    while (1)
//    {
//        send_command();
//    }
//
//    close(sockfd);
//    printf("\nconnection with client closed\n");
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

#define PORT 4040
#define IP "25.59.56.69"

int sockfd = -1;

void create_connect(/*in_addr_t address*/)
{
    // struct sockaddr_in addr;

    // if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    //   printf("\x1b[31mCreation error\nError: %s\n\x1b[0m", strerror(errno));

    // memset(&addr, 0, sizeof(addr));
    // addr.sin_family = AF_INET;
    // addr.sin_port = htons(PORT);
    // addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // // addr.sin_addr.s_addr = inet_addr(IP);

    // int con;
    // if((con = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr))) < 0)
    // printf("\x1b[31mConnecting error\nError: %s\n\x1b[0m", strerror(errno));

    struct sockaddr_in addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        printf("\x1b[31mCreation error\nError: %s\n\x1b[0m", strerror(errno));

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    //addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_addr.s_addr = inet_addr(IP);

    int con;
    con = connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    if (con < 0)
        printf("\x1b[31mConnecting error\nError: %s\n\x1b[0m", strerror(errno));
}

void get_text()
{
    int size = 0;
    char* res = NULL;

    if ((recv(sockfd, &size, sizeof(int), MSG_WAITALL)) < 0)
        printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));

    // printf("\n%d\n", size);

    res = (char*)realloc(res, size);

    if (recv(sockfd, res, size, MSG_WAITALL) < 0)
        printf("\x1b[31mReceiving error\nError: %s\n\x1b[0m", strerror(errno));

    // printf("\n%d, %s\n", size, res);
    printf("\x1b[36mAnswer:\n\x1b[0m%s \n", res);
}

void send_command()
{
    while (1)
    {
        int size = 0;
        char* str = NULL;
        str = (char*)realloc(str, 255);

        printf("\x1b[36mEnter command:\x1b[0m ");
        fgets(str, 255, stdin);
        size = strlen(str);

        if (send(sockfd, &size, sizeof(int), 0) < 0)
            printf("\x1b[31mSending error\nError: %s\n\x1b[0m", strerror(errno));
        if (send(sockfd, str, size, 0) < 0)
            printf("\x1b[31mSending error\nError: %s\n\x1b[0m", strerror(errno));

        if (strcmp(str, "quit\n") == 0)
        {
            close(sockfd);
            exit(0);
        }

        get_text();
    }
}

int main()
{
    system("clear");

    //in_addr_t address = inet_addr(argv[2]);
    create_connect(/*address*/); 
    send_command();
    close(sockfd);
}
