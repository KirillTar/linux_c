#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Структура для хранения информации о пользователе
typedef struct {
	char login[255];
	char password[255];
} User;

// Функция для проверки занят ли логин
int checkLoginAlradyExists(char* login) {

	// Чтение файла со списком пользователей
	FILE* file = fopen("./store/users", "r");

	if (file == NULL) {
		return 0;
	}
	char storedLogin[255], storedPassword[255];
	while (fscanf(file, "%s %s", storedLogin, storedPassword)) {
		// Проверка логина и пароля
		if (strcmp(login, storedLogin) == 0) {
			fclose(file);
			return 1; // Возвращаем 1, если пользователь существует
		}
	}

	fclose(file);
	return 0; // Возвращаем 0, если пользователь не существует
}

// Функция для проверки занят ли логин
User* registerNewUser(char* login, char* password) {

	// Чтение файла со списком пользователей
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

	// Чтение файла со списком пользователей
	FILE* file = fopen("./store/users", "r");
	if (file == NULL) {
		printf("Ошибка открытия файла пользователей.\n");
		return NULL;
	}

	char line[100];
	while (fgets(line, sizeof(line), file)) {
		char storedLogin[50], storedPassword[50];
		sscanf(line, "%s %s", storedLogin, storedPassword);
		// Проверка логина и пароля
		if (strcmp(login, storedLogin) == 0 && strcmp(password, storedPassword) == 0) {
			fclose(file);
			User* user = (User*)malloc(sizeof(user));
			strcpy(user->login, login);
			strcpy(user->password, password);
			return user;
		}
	}
	fclose(file);
	return NULL; // Возвращаем 0, если логин и пароль неверны
}

// Функция для вывода всех файлов пользователя
char* getUserFiles(User* user) {

	// Формирование имени файла для пользователя
	char fileName[269];
	snprintf(fileName, sizeof(fileName), "./store/%s_files", user->login);

	// Чтение файла с файлами пользователя
	FILE* file = fopen(fileName, "r");
	if (file == NULL) {
		return (char*)"\0";
	}

	int numSectors = 1; // количество блоков по 1024
	char* result = malloc(sizeof(char) * 1024); // изначально выделяем 1024 байт
	size_t aviableSize = 1024; // храним оставшеесе пространство
	char* ptr = result; // указатель на текущую строку с файлом
	char line[255];

	while (fgets(line, sizeof(line), file)) { // читаем строку из файла
		while (aviableSize - strlen(line) < 0) { // если у нас не хвататет места в строке result, то расширяем строку пока не будет хватать
			numSectors++; // увеличваем количество блоков
			result = realloc(result, numSectors * 1024); // расширяем блок, копируя туда старые данные
			aviableSize += 1024; // увеличиваем достпное место
		}
		strcpy(ptr, line); // копируем на место новой строки строку из файла
		ptr += strlen(line) - 1; // перемещаем указатель на размер вставленной строки
		(*ptr) = ' ';
		ptr++;
	}

	(*ptr) = '\0';
	fclose(file);
	return result;
}
// Функция для создания файла
void createUserFile(const char* fileName, User* user) {
	// Формирование имени файла для пользователя
	char userFileName[269];
	char userFile[269];
	snprintf(userFileName, sizeof(userFileName), "./store/%s_files", user->login); //запись названия файла в "файлы пользователя"
	snprintf(userFile, sizeof(userFile), "./store/%s_%s.uf", user->login, fileName); // создание файла пользователя

	// Открытие файла для записи
	FILE* file = fopen(userFileName, "a+");
	FILE* file1 = fopen(userFile, "w");

	if ((file == NULL) || (file1 == NULL)) {
		printf("Ошибка открытия файла пользовательских файлов.\n");
		return;
	}
	// Запись данных в файл пользователя
	fprintf(file, "%s\n", fileName);
	fclose(file);
}
// Функция для удаления файла пользователя
int deleteUserFile(const char* fileName, User* user) {

	// Формирование имени файла для пользователя
	char userFileName[269];
	char userFile[269];
	snprintf(userFileName, sizeof(userFileName), "./store/%s_files", user->login);
	snprintf(userFile, sizeof(userFile), "./store/%s_%s.uf", user->login, fileName); // создание файла пользователя

	// Открытие файла с файлами пользователя для чтения
	FILE* file = fopen(userFileName, "r");
	if (file == NULL) {
		return 0;
	}

	// Открытие временного файла для записи
	FILE* tempFile = fopen("./store/temp", "w");
	if (tempFile == NULL) {
		fclose(file);
		return 0;
	}

	char line[255];
	int deleteFlag = 0; // Флаг для определения удаленного файла
	while (fgets(line, sizeof(line), file)) {
		if (strstr(line, fileName) == NULL) {
			fprintf(tempFile, "%s", line); // Запись строки во временный файл
		}
		else {
			deleteFlag = 1; // Файл найден и удален
		}
	}

	fclose(file);
	fclose(tempFile);

	if (deleteFlag) {
		remove(userFileName); // Удаление исходного файла
		remove(userFile);
		rename("./store/temp", userFileName); // Переименование временного файла
		return 1;
	}
	else {
		remove("./store/temp"); // Удаление временного файла, так как ничего не удалилось
		return 0;
	}
}

// Функция для проверки существования файла пользователя
int checkUserFile(User* user, const char* fileName) {

	// Формирование имени файла для пользователя
	char userFileName[269];
	snprintf(userFileName, sizeof(userFileName), "./store/%s_files", user->login);

	// Открытие файла с файлами пользователя для чтения
	FILE* file = fopen(userFileName, "r");
	if (file == NULL) {
		printf("Вы пытаетесь открыть чужой файл.\n");
		return 0;
	}

	char line[100];
	while (fgets(line, sizeof(line), file)) {
		char storedFile[50];
		sscanf(line, "%s", storedFile);
		// Проверка логина и пароля
		if (strcmp(fileName, storedFile) == 0) {
			fclose(file);
			return 1; // Возвращаем 1, если такой файл есть у пользователя
		}
	}

	fclose(file);
	return 0; // Возвращаем 0, если у пользователя нет такого файла
}

// Функция для изменения файла пользователя
int modifyUserFile(char* fileName, User* user, int fd, size_t size) {

	// Формирование имени файла для пользователя
	char userFileName[269];
	snprintf(userFileName, sizeof(userFileName), "./store/%s_%s.uf", user->login, fileName); // создание файла пользователя

	// Открытие файла с файлами пользователя для чтения
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

// Функция для чтения файла пользователя
FILE* getUserFile(char* fileName, User* user) {

	// Формирование имени файла для пользователя
	char userFile[267];
	snprintf(userFile, sizeof(userFile), "./store/%s_%s.uf", user->login, fileName); // создание файла пользователя

	// Открытие файла для чтения
	FILE* file = fopen(userFile, "r");
	if (file == NULL) {
		printf("Ошибка открытия файла пользовательских файлов.\n");
		return NULL;
	}
	return file;
}

int main() {

	char login[50];
	char password[50];
	int admin = 0;
	setlocale(LC_ALL, "Rus");
	// Ввод логина и пароля
	printf("Введите логин: ");
	scanf("%s", login);
	printf("Введите пароль: ");
	scanf("%s", password);

	// Проверка логина и пароля
	if (checkCredentials(login, password)) {

		if (strcmp("admin", login) == 0) {
			admin = 1;
		}

		printf("Вход выполнен успешно!\n");
		showUserFiles(login);
		// Возможность работы с файлами
		printf("\n1. Изменить файл\n2. Удалить файл\n3. Прочитать файл\n4. Создать новый файл\n");
		int choice;
		printf("Выберите действие (1-4): ");
		scanf("%d", &choice);

		switch (choice) {

		case 1: {
			char fileName[50];
			char content[100];
			printf("Введите имя файла: ");
			scanf("%s", fileName);

			//поиск файла с таким названием
			if (checkUserFile(login, fileName)) {
				printf("Введите содержимое файла: ");
				scanf("%s", content);
				modifyUserFile(fileName, content);
				printf("Файл изменен.\n");
			}
			else {
				printf("Такого файла не существует.\n");
			}

			break;
		}
		case 2: {

			char fileName[50];
			printf("Введите имя файла для удаления, включая его расширение: ");
			scanf("%s", fileName);

			//поиск файла с таким названием
			if (checkUserFile(login, fileName)) {
				
			}
			else {
				printf("Такого файла не существует.\n");
			}
			break;
		}
		case 3: {

			char fileName[50];
			printf("Введите имя файла для чтения, включая его расширение: ");
			scanf("%s", fileName);

			//поиск файла с таким названием
			if ((checkUserFile(login, fileName))||(admin == 1)) {
				readUserFile(fileName);
			}
			else { 
				printf("Такого файла не существует.\n");
			}
			break;
		}
		case 4: {
			char fileName[50];
			char content[100];
			printf("Введите имя нового файла, включая его расширение: ");
			scanf("%s", fileName);
			printf("Введите содержимое нового файла: ");
			scanf("%s", content);
			createUserFile(login, fileName, content);
			printf("Файл создан.\n");
			break;
		}
		default:
			printf("Некорректный выбор.\n");
		}
	}
	else {
		printf("Неверные логин или пароль.\n");
	}
	return 0;
}
