#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "protocol.h"
#include "storage.c"

#define PORT 1090
short socket_fd;

void SocketCreate()
{
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_fd == -1)
	{
		perror("Socket create");
		exit(1);
	}
	printf("\033[34m[Info]\033[0m: Socket created!\n");

	int optval = 1;

	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval)) < 0) {
		perror("Socket create");
		exit(1);
	}
}

void BindCreatedSocket()
{
	signal(SIGPIPE, SIG_IGN);
	struct sockaddr_in remote = { 0 };
	memset(&remote, 0, sizeof(remote));
	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr = htonl(INADDR_ANY);
	remote.sin_port = htons(PORT);

	if (bind(socket_fd, (struct sockaddr*)&remote, sizeof(remote)) == -1)
	{
		perror("Socket bind");
		exit(1);
	}
}

int reciveData(int net_fd, void* data, size_t size, uint8_t returnNum) {

	int recived;

	if ((recived = recv(net_fd, data, size, MSG_WAITALL)) < 0 && !returnNum)
	{
		perror("recv failed");
		exit(1);
	}

	return recived;
}

void sendData(int net_fd, void* data, size_t size) {

	if (send(net_fd, data, size, 0) < 0)
	{
		perror("send failed");
		exit(1);
	}
}

void client_error(int net_fd, char* message) {

	printf("\033[31m[Error] [Client %d]\033[0m: %s!\n", net_fd, message);
	close(net_fd);
	pthread_exit(NULL);
}

User* ProtocolAuthefication(int net_fd) {

	struct AUTH_REQUEST request;
	struct AUTH_RESPONSE response = { AUTH_RESPONSE };
	User* user;

	reciveData(net_fd, &request, sizeof(request), 0);

	if (request.type != AUTH_REQUEST && request.type != REGISTER_REQUEST) 
		client_error(net_fd, "Некорректный тип запроса на авторизацию");

	if (request.type == REGISTER_REQUEST) { // регистрация

		if (checkLoginAlradyExists(request.login)) {
			response.status = AUTH_BADLOGIN;
			sendData(net_fd, &response, sizeof(response));
			return NULL;
		}

		user = registerNewUser(request.login, request.password);
		printf("\033[34m[Info] [Client %d]\033[0m: \033[32mRegistered new client: %s\033[0m!\n", net_fd, user->login);
		response.status = AUTH_OK;
		sendData(net_fd, &response, sizeof(response));
		return user;
	}
	else if (request.type == AUTH_REQUEST) { // авторизация

		if (user = checkCredentials(request.login, request.password)) {
			response.status = AUTH_OK;
			sendData(net_fd, &response, sizeof(response));
			return user;
		}
		else {
			response.status = AUTH_BADLOGIN;
			sendData(net_fd, &response, sizeof(response));
			return NULL;
		}
	}
}

void handleListRequest(int net_fd, User* user) {

	struct LIST_RESPONSE response = { LIST_RESPONSE };
	char* files = getUserFiles(user);
	// char* files = malloc(50);
	// strcpy(files, "111 aaa abc lll");
	response.size = strlen(files);
	sendData(net_fd, &response, sizeof(response));
	sendData(net_fd, files, response.size);
	free(files);
	printf("\033[34m[Info] [Client %d (%s)]\033[0m: Client requsted files list (strlen: %ld)!\n", net_fd, user->login, response.size);
}

void handleFileRequest(int net_fd, User* user) {

	struct FILE_REQUEST request;
	struct FILE_RESPONSE response = { FILE_RESPONSE };
	reciveData(net_fd, (void*)((long int)&request + sizeof(request.type)), sizeof(request) - sizeof(request.type), 0);

	switch (request.action)
	{
	case FILE_CREATE:

		createUserFile(request.file_name, user);
		response.size = 0;
		sendData(net_fd, &response, sizeof(response));
		printf("\033[34m[Info] [Client %d (%s)]\033[0m: Client created file %s!\n", net_fd, user->login, request.file_name);
		break;

	case FILE_EDIT:

		FILE* file = getUserFile(request.file_name, user);
		if (file == NULL) client_error(net_fd, "Файл не найден");
		fseek(file, 0, SEEK_END); // seek to end of file
		response.size = ftell(file); // get current file pointer
		fseek(file, 0, SEEK_SET); // seek back to beginning of file
		sendData(net_fd, &response, sizeof(response));
		char c;
		do
		{
			c = fgetc(file);
			sendData(net_fd, &c, 1);
		} while (c != EOF);

		printf("\033[34m[Info] [Client %d (%s)]\033[0m: Client getted file %s for edit!\n", net_fd, user->login, request.file_name);
		break;

	case FILE_REMOVE:

		if (!deleteUserFile(request.file_name, user)) client_error(net_fd, "Не удалось удалить файл");
		response.size = 0;
		sendData(net_fd, &response, sizeof(response));
		printf("\033[34m[Info] [Client %d (%s)]\033[0m: Client deleted file%s!\n", net_fd, user->login, request.file_name);
		break;

	default:
		client_error(net_fd, "Неизвестное действие над файлом");
	}
}

void handleFileSaveRequest(int net_fd, User* user) {

	struct FILE_SAVE_REQUEST request;
	struct FILE_SAVE_RESPONSE response = { FILE_SAVE_RESPONSE };
	reciveData(net_fd, (void*)((long int)&request + sizeof(request.type)), sizeof(request) - sizeof(request.type), 0);
	printf("\033[34m[Info] [Client %d]\033[0m: Client try save to file %s, size: %ld bytes.\n", net_fd, request.file_name, request.size);

	if (!modifyUserFile(request.file_name, user, net_fd, request.size)) 
		client_error(net_fd, "Не удалось сохранить файл");
	sendData(net_fd, &response, sizeof(response));
}

void* client_process(void* fd)
{
	int net_fd = *(int*)fd;
	printf("\033[34m[Info] [Client %d]\033[0m: Client has been connected!\n", net_fd);
	User* user;

	while (!(user = ProtocolAuthefication(net_fd))); // авторизация
	printf("\033[34m[Info] [Client %d]\033[0m: Client user name %s!\n", net_fd, user->login);

	enum MESSAGE_TYPES type;
	int recvSize;

	while (reciveData(net_fd, &type, sizeof(type), 1) > 0) {
		switch (type)
		{
		case LIST_REQUEST:
			handleListRequest(net_fd, user);
			break;
		case FILE_REQUEST:
			handleFileRequest(net_fd, user);
			break;
		case FILE_SAVE_REQUEST:
			handleFileSaveRequest(net_fd, user);
			break;
		default:
			client_error(net_fd, "Неизвестный тип сообщения");
			break;
		}
	}

	printf("\033[34m[Info] [Client %d (%s)]\033[0m: Client disconected!\n", net_fd, user->login);
	close(net_fd);
	free(user);
	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	int sock, clientLen, read_size, remotelen;
	struct sockaddr_in server, client;
	SocketCreate();
	BindCreatedSocket();
	printf("\033[34m[Info]\033[0m: Bind done!\n");
	//Listen
	listen(socket_fd, 25);
	memset(&client, 0, sizeof(client));
	pthread_t worker;

	while (1)
	{
		printf("\033[34m[Info]\033[0m: Waiting for incoming connections...\n");
		clientLen = sizeof(struct sockaddr_in);
		sock = accept(socket_fd, (struct sockaddr*)&client, (socklen_t*)&clientLen);
		if (sock < 0)
		{
			perror("accept failed");
			exit(1);
		}
		if (pthread_create(&worker, NULL, &client_process, (void*)&sock) == 0) {
			pthread_detach(worker);
		}
		else {
			printf("\033[34m[Error]\033[0m: error pthread_create...\n");
			exit(0);
		}
	}
	return 0;
}