#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// ��������� ��� �������� ���������� � ������������
typedef struct {
	char login[255];
	char password[255];
} User;

// ������� ��� �������� ����� �� �����
int checkLoginAlradyExists(char* login) {

	// ������ ����� �� ������� �������������
	FILE* file = fopen("./store/users", "r");

	if (file == NULL) {
		return 0;
	}
	char storedLogin[255], storedPassword[255];
	while (fscanf(file, "%s %s", storedLogin, storedPassword)) {
		// �������� ������ � ������
		if (strcmp(login, storedLogin) == 0) {
			fclose(file);
			return 1; // ���������� 1, ���� ������������ ����������
		}
	}

	fclose(file);
	return 0; // ���������� 0, ���� ������������ �� ����������
}

// ������� ��� �������� ����� �� �����
User* registerNewUser(char* login, char* password) {

	// ������ ����� �� ������� �������������
	FILE* file = fopen("./store/users", "a+");
	if (file == NULL) {
		return NULL;
	}

	fprintf(file, "%s %s\n", login, password);
	fclose(file);
	User* user = (User*)malloc(sizeof(user));
	strcpy(user->login, login);
	strcpy(user->password, password);
	return user;
}
User* checkCredentials(char* login, char* password) {

	// ������ ����� �� ������� �������������
	FILE* file = fopen("./store/users", "r");
	if (file == NULL) {
		printf("������ �������� ����� �������������.\n");
		return NULL;
	}

	char line[100];
	while (fgets(line, sizeof(line), file)) {
		char storedLogin[50], storedPassword[50];
		sscanf(line, "%s %s", storedLogin, storedPassword);
		// �������� ������ � ������
		if (strcmp(login, storedLogin) == 0 && strcmp(password, storedPassword) == 0) {
			fclose(file);
			User* user = (User*)malloc(sizeof(user));
			strcpy(user->login, login);
			strcpy(user->password, password);
			return user;
		}
	}
	fclose(file);
	return NULL; // ���������� 0, ���� ����� � ������ �������
}

// ������� ��� ������ ���� ������ ������������
char* getUserFiles(User* user) {

	// ������������ ����� ����� ��� ������������
	char fileName[269];
	snprintf(fileName, sizeof(fileName), "./store/%s_files", user->login);

	// ������ ����� � ������� ������������
	FILE* file = fopen(fileName, "r");
	if (file == NULL) {
		return (char*)"\0";
	}

	int numSectors = 1; // ���������� ������ �� 1024
	char* result = malloc(sizeof(char) * 1024); // ���������� �������� 1024 ����
	size_t aviableSize = 1024; // ������ ���������� ������������
	char* ptr = result; // ��������� �� ������� ������ � ������
	char line[255];

	while (fgets(line, sizeof(line), file)) { // ������ ������ �� �����
		while (aviableSize - strlen(line) < 0) { // ���� � ��� �� �������� ����� � ������ result, �� ��������� ������ ���� �� ����� �������
			numSectors++; // ���������� ���������� ������
			result = realloc(result, numSectors * 1024); // ��������� ����, ������� ���� ������ ������
			aviableSize += 1024; // ����������� �������� �����
		}
		strcpy(ptr, line); // �������� �� ����� ����� ������ ������ �� �����
		ptr += strlen(line) - 1; // ���������� ��������� �� ������ ����������� ������
		(*ptr) = ' ';
		ptr++;
	}

	(*ptr) = '\0';
	fclose(file);
	return result;
}
// ������� ��� �������� �����
void createUserFile(const char* fileName, User* user) {
	// ������������ ����� ����� ��� ������������
	char userFileName[269];
	char userFile[269];
	snprintf(userFileName, sizeof(userFileName), "./store/%s_files", user->login); //������ �������� ����� � "����� ������������"
	snprintf(userFile, sizeof(userFile), "./store/%s_%s.uf", user->login, fileName); // �������� ����� ������������

	// �������� ����� ��� ������
	FILE* file = fopen(userFileName, "a+");
	FILE* file1 = fopen(userFile, "w");

	if ((file == NULL) || (file1 == NULL)) {
		printf("������ �������� ����� ���������������� ������.\n");
		return;
	}
	// ������ ������ � ���� ������������
	fprintf(file, "%s\n", fileName);
	fclose(file);
}
// ������� ��� �������� ����� ������������
int deleteUserFile(const char* fileName, User* user) {

	// ������������ ����� ����� ��� ������������
	char userFileName[269];
	char userFile[269];
	snprintf(userFileName, sizeof(userFileName), "./store/%s_files", user->login);
	snprintf(userFile, sizeof(userFile), "./store/%s_%s.uf", user->login, fileName); // �������� ����� ������������

	// �������� ����� � ������� ������������ ��� ������
	FILE* file = fopen(userFileName, "r");
	if (file == NULL) {
		return 0;
	}

	// �������� ���������� ����� ��� ������
	FILE* tempFile = fopen("./store/temp", "w");
	if (tempFile == NULL) {
		fclose(file);
		return 0;
	}

	char line[255];
	int deleteFlag = 0; // ���� ��� ����������� ���������� �����
	while (fgets(line, sizeof(line), file)) {
		if (strstr(line, fileName) == NULL) {
			fprintf(tempFile, "%s", line); // ������ ������ �� ��������� ����
		}
		else {
			deleteFlag = 1; // ���� ������ � ������
		}
	}

	fclose(file);
	fclose(tempFile);

	if (deleteFlag) {
		remove(userFileName); // �������� ��������� �����
		remove(userFile);
		rename("./store/temp", userFileName); // �������������� ���������� �����
		return 1;
	}
	else {
		remove("./store/temp"); // �������� ���������� �����, ��� ��� ������ �� ���������
		return 0;
	}
}

// ������� ��� �������� ������������� ����� ������������
int checkUserFile(User* user, const char* fileName) {

	// ������������ ����� ����� ��� ������������
	char userFileName[269];
	snprintf(userFileName, sizeof(userFileName), "./store/%s_files", user->login);

	// �������� ����� � ������� ������������ ��� ������
	FILE* file = fopen(userFileName, "r");
	if (file == NULL) {
		printf("�� ��������� ������� ����� ����.\n");
		return 0;
	}

	char line[100];
	while (fgets(line, sizeof(line), file)) {
		char storedFile[50];
		sscanf(line, "%s", storedFile);
		// �������� ������ � ������
		if (strcmp(fileName, storedFile) == 0) {
			fclose(file);
			return 1; // ���������� 1, ���� ����� ���� ���� � ������������
		}
	}

	fclose(file);
	return 0; // ���������� 0, ���� � ������������ ��� ������ �����
}

// ������� ��� ��������� ����� ������������
int modifyUserFile(char* fileName, User* user, int fd, size_t size) {

	// ������������ ����� ����� ��� ������������
	char userFileName[269];
	snprintf(userFileName, sizeof(userFileName), "./store/%s_%s.uf", user->login, fileName); // �������� ����� ������������

	// �������� ����� � ������� ������������ ��� ������
	FILE* file = fopen(userFileName, "w");

	if (file == NULL) {
		return 0;
	}

	char c;
	for (size_t i = 0; i < size; i++)
	{
		read(fd, &c, 1);
		fputc(c, file);
	}

	fclose(file);
	return 1;
}

// ������� ��� ������ ����� ������������
FILE* getUserFile(char* fileName, User* user) {

	// ������������ ����� ����� ��� ������������
	char userFile[267];
	snprintf(userFile, sizeof(userFile), "./store/%s_%s.uf", user->login, fileName); // �������� ����� ������������

	// �������� ����� ��� ������
	FILE* file = fopen(userFile, "r");
	if (file == NULL) {
		printf("������ �������� ����� ���������������� ������.\n");
		return NULL;
	}
	return file;
}

int main() {

	char login[50];
	char password[50];
	int admin = 0;
	setlocale(LC_ALL, "Rus");
	// ���� ������ � ������
	printf("������� �����: ");
	scanf("%s", login);
	printf("������� ������: ");
	scanf("%s", password);

	// �������� ������ � ������
	if (checkCredentials(login, password)) {

		if (strcmp("admin", login) == 0) {
			admin = 1;
		}

		printf("���� �������� �������!\n");
		showUserFiles(login);
		// ����������� ������ � �������
		printf("\n1. �������� ����\n2. ������� ����\n3. ��������� ����\n4. ������� ����� ����\n");
		int choice;
		printf("�������� �������� (1-4): ");
		scanf("%d", &choice);

		switch (choice) {

		case 1: {
			char fileName[50];
			char content[100];
			printf("������� ��� �����: ");
			scanf("%s", fileName);

			//����� ����� � ����� ���������
			if (checkUserFile(login, fileName)) {
				printf("������� ���������� �����: ");
				scanf("%s", content);
				modifyUserFile(fileName, content);
				printf("���� �������.\n");
			}
			else {
				printf("������ ����� �� ����������.\n");
			}

			break;
		}
		case 2: {

			char fileName[50];
			printf("������� ��� ����� ��� ��������, ������� ��� ����������: ");
			scanf("%s", fileName);

			//����� ����� � ����� ���������
			if (checkUserFile(login, fileName)) {
				
			}
			else {
				printf("������ ����� �� ����������.\n");
			}
			break;
		}
		case 3: {

			char fileName[50];
			printf("������� ��� ����� ��� ������, ������� ��� ����������: ");
			scanf("%s", fileName);

			//����� ����� � ����� ���������
			if ((checkUserFile(login, fileName))||(admin == 1)) {
				readUserFile(fileName);
			}
			else { 
				printf("������ ����� �� ����������.\n");
			}
			break;
		}
		case 4: {
			char fileName[50];
			char content[100];
			printf("������� ��� ������ �����, ������� ��� ����������: ");
			scanf("%s", fileName);
			printf("������� ���������� ������ �����: ");
			scanf("%s", content);
			createUserFile(login, fileName, content);
			printf("���� ������.\n");
			break;
		}
		default:
			printf("������������ �����.\n");
		}
	}
	else {
		printf("�������� ����� ��� ������.\n");
	}
	return 0;
}
