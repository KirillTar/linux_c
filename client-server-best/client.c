#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include "protocol.h"

int sockfd = -1;

//��������� ��� ��������� ������ ������
struct list {
    int size; //������
    char** list_str; //��������� ������������ ������ �����
};

//==========�������� �����������==================

int create_connect(int port, in_addr_t address) {

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("\nsocket create error\n");
        exit(0);
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = address;
    //serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int con = -1;
    con = connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    return con;
}

//====================================================


//==========����������� (1 - � ������ ������, 0 - � ������ ������)===
int auth() { 

    int cmd = -1;
    
    for (;;) 
    {
        system("clear");
        printf("1)Authorization   \n2)Registration\n\n");

        printf("Enter command: ");
        scanf("%d", &cmd);

        if (cmd < 1 || cmd > 2) 
        {
            printf("Bad command! \n");
            sleep(2);
        }
        else
        {
            break;
        }
    }

    enum MESSAGE_TYPES type;

    switch (cmd)
    {
    case 1:
        type = AUTH_REQUEST;
        break;
    case 2:
        type = REGISTER_REQUEST;
        break;
    default:
        type = REGISTER_REQUEST;
        break;
    }

    struct AUTH_REQUEST request;
    struct AUTH_RESPONSE response;

    request.type = type;
    printf("Enter login: ");
    scanf("%s", request.login);

    printf("Enter password: ");
    scanf("%s", request.password);

    //printf("\nType: %d login: %s password: %s Size:%ld\n", request.type, request.login, request.password, sizeof(struct AUTH_REQUEST));

    send(sockfd, &request, sizeof(struct AUTH_REQUEST), 0);
    recv(sockfd, &response, sizeof(struct AUTH_RESPONSE), MSG_WAITALL);

    if (response.type != AUTH_RESPONSE) {
        perror("\nrequest invalid type!\n");
        exit(0);
    }

    if (response.status == AUTH_BADLOGIN) 
    {
        return 0;
    }

    printf("\nauthorized succesful\n");
    printf("\nPress ENTER to exit...\n");
    getchar();
    getchar();
    return 1;
}
//===================================================================

//===�������� ��������������, ���� �� ���������======================
void auth_loop() {

    for (;;)
    {
        if (auth())
        {
            break;
        }
        else
        {
            printf("\nBad login!\n"); 
            printf("Check your login/password or register!\n");
            printf("\nPress ENTER to exit...\n");
            getchar();
            //getchar();
        }
    }
}
//====================================================================


//�������� ������ ������, ���������� ��������� list � ������������ �������
struct list get_list() {

        system("clear");

        struct LIST_REQUEST request;
        struct LIST_RESPONSE response;

        request.type = LIST_REQUEST;

        //���������� ������ �� ��������� ������
        send(sockfd, &request, sizeof(struct LIST_REQUEST), 0);

        //�������� ����� � �������� ������
        if (recv(sockfd, &response, sizeof(struct LIST_RESPONSE), MSG_WAITALL) < 0) {
            perror("\nrecv failed!\n");
            exit(0);
        }

        if (response.type != LIST_RESPONSE) {
            perror("\nrequest invalid type!\n");
            exit(0);
        }

        char sep[] = " ";
        char** istr;
        int files_amount = 1;
        int i = 0;

        //�������� ������ ��� ������ ������
        istr = (char**)malloc(files_amount * sizeof(char*));
        istr[i] = (char*)malloc(255);


        //���� ������ ������ == 0 - ���������� ������ ������
        if (response.size == 0) {
            strcpy(istr[0], "");
            struct list list = { 0, istr };
            return list;
        }

        char* list_str;
        list_str = (char*)malloc(response.size);

        //���� ������ > 0 - ���� ������ �������� size
        recv(sockfd, list_str, response.size, MSG_WAITALL);

        //���������� ��� ������� ����� �� ������� 0
        istr[i] = strtok(list_str, sep);
     
        //���� ���� " " � ������ - �������� �� �� ����� ������ � ���������� �� ������� i
        while (istr[i] != NULL)
        {
            files_amount++;
            i++;
            //����������� �������� ������ �� 255 � ������ �������
            istr = (char**)realloc(istr, files_amount * sizeof(char*));
            istr[i] = (char*)malloc(255);
            istr[i] = strtok(NULL, sep);
        }
        
        //���������� ����������� ���������, ��������� ���������� ������
        struct list list = { files_amount, istr };
        return list;
}
//=======================================================================

//=============����� ������ ������=======================================
void print_list(struct list list) {

     printf("===========FILE LIST============\n");

    for (int i = 0; i < list.size - 1; i++)
    {    
        printf("%d) %s\n", i, list.list_str[i]);
    }

    printf("================================\n");
}
//=======================================================================

//=============�������� ��� ����� �� ��� ������� � �������===============
char* select_file_name(struct list list) {
    print_list(list);

    int file_num = -1;

    //���� ������ - ���������� ������ ������
    if (list.size == 0) {
        printf("File list is empty! Create file\n");
        sleep(3);
        return list.list_str[0];
    }

    for (;;)
    {
        printf("\nEnter file number : ");
        scanf("%d", &file_num);

        if (file_num < 0 || file_num > list.size - 1)
        {
            printf("index out of range!\n");
        }
        else
        {
            break;
        }
    }

    //���������� ��� ��� (�� ������!)
    return list.list_str[file_num];
}


//===================�������� �����========================
void file_create() {

    system("clear");
    struct FILE_REQUEST request;
    struct FILE_RESPONSE response;

    request.type = FILE_REQUEST;
    request.action = FILE_CREATE;

    printf("Enter file name: ");
    scanf("%s", request.file_name);

    //���������� ������ �� �������� � ����������� ���������� FILE_REQUEST
    send(sockfd, &request, sizeof(struct FILE_REQUEST), 0);

    if (recv(sockfd, &response, sizeof(struct FILE_RESPONSE), MSG_WAITALL) < 0) {
        perror("\nrecv failed!\n");
        exit(0);
    }

    if (response.type != FILE_RESPONSE) {
        perror("\nrequest invalid type!\n");
        exit(0);
    }

    printf("\nfile created\n");
    printf("\nPress ENTER to exit...\n");
    getchar();
    getchar();
}
//================================================================


//=========================�������������� �����===================
void file_edit(char* file_name) {

    system("clear");

    printf("==========FILE EDITOR===========\n");
    printf("File Name: \033[32m%s\033[0m\n", file_name);
    printf("===\033[31mENTER \":q\" to save and exit\033[0m==\n");
    printf("================================\n");

    int size = 0;

    char* file_str = NULL;  //������ � ������ ��������� �����
    char* newstr = (char*)malloc(255);      //����� ������ ��� ���������� �� �������
    
    fgets(newstr, 255, stdin);
    fgets(newstr, 255, stdin);  //������ ������ ������ �� �������

    while (strcmp(newstr, ":q\n"))  //���������� ������ � �������� ������ ":q"
    {
        size += (strlen(newstr));     //��������� � ������� ����� ������ ��������� ������
        file_str = (char*)realloc(file_str, size*2);   //��������� ������ ��������

        if (file_str == NULL) {
            perror("\ninvalid pointer assigmesnt\n");
            exit(0);
        }

        strncat(file_str, newstr, strlen(newstr));    //��������� ��������� ������ � �������� �����
        fgets(newstr, 255, stdin);  //������ ��������� ������
    }

    printf("==================================\n");

    struct FILE_SAVE_REQUEST request;
    struct FILE_SAVE_RESPONSE response;

    request.type = FILE_SAVE_REQUEST;
    strcpy(request.file_name, file_name);
    request.size = size;

    //���������� ������ �� �������������� (��������� ��� ����� � ��� ������)
    send(sockfd, &request, sizeof(struct FILE_SAVE_REQUEST), 0);

    //����� ���������� ���� ������
    send(sockfd, file_str, size, 0);

    if (recv(sockfd, &response, sizeof(struct FILE_SAVE_RESPONSE), MSG_WAITALL) < 0) {
        perror("\nrecv failed!\n");
        exit(0);
    }

    if (response.type != FILE_SAVE_RESPONSE) {
        perror("\nrequest invalid type!\n");
        exit(0);
    }

    printf("\nfile %s edited\n", file_name);
    printf("\nPress ENTER to exit...\n");
    getchar();
}
//===================================================


//================�������� �����=====================
void file_remove(char* file_name) {

    struct FILE_REQUEST request;
    struct FILE_RESPONSE response;

    request.type = FILE_REQUEST;
    request.action = FILE_REMOVE;
    strcpy(request.file_name, file_name);

    send(sockfd, &request, sizeof(struct FILE_REQUEST), 0);

    if (recv(sockfd, &response, sizeof(struct FILE_RESPONSE), MSG_WAITALL) < 0) {
        perror("\nrecv failed!\n");
        exit(0);
    }

    if (response.type != FILE_RESPONSE) {
        perror("\nrequest invalid type!\n");
        exit(0);
    }

    printf("\nfile %s deleted\n", file_name);
    printf("\nPress ENTER to exit...\n");
    getchar();
    getchar();
}
//===========================================================


//============���� ������ �������============================
void menu() {
    
    int cmd = -1;
    struct list list;
    char* file_name;

    while (cmd != 0) {

        for (;;)
        {
            system("clear");
            printf("1)File create   \n2)File edit   \n3)File remove   \n0)EXIT\n\n");

            printf("Enter command: ");
            scanf("%d", &cmd);

            if (cmd < 0 || cmd > 3)
            {
                printf("Bad command! \n");
                sleep(2);
            }
            else
            {
                break;
            }
        }

        switch (cmd) {

        case 1:
            file_create();
            break;
        case 2:
            list = get_list();
            file_name = select_file_name(list);

            if (strcmp(file_name, "") == 0) {
                break;
            }

            file_edit(file_name);
            break;
        case 3:
            list = get_list();
            file_name = select_file_name(list);

            if (strcmp(file_name, "") == 0) {
                break;
            }

            file_remove(file_name);
            break;
        case 0:
        default:
            cmd = 0;
        break;
        }
    }
}
//=========================================================



int main(int argc, char* argv[]) {

    system("clear");

    if (argc != 3) {
        printf("\n missed args: Enter \"./output + port + ip\" \n");
        return 1;
    }
    
    //�������� ���� � ip �� ����������, �������� ������������
    int port = atoi(argv[1]);
    in_addr_t address = inet_addr(argv[2]);

    if (create_connect(port, address) < 0) {
        printf("\nConnection error!\n");
        return 1;
    }
    //=======================================================


    auth_loop();
    menu();

    close(sockfd);
    printf("\nSession ended\n");

    return 0;
}