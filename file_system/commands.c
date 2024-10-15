#include "commands.h"

struct action table[] = {
	{
	"print",
	do_print
	},
	{
	"chdir",
	do_chdir
	},
	{
	"mkdir",
	do_mkdir
	},
	{
	"rmdir",
	do_rmdir
	},
	{
	"mvdir",
	do_mvdir
	},
	{
	"mkfile",
	do_mkfile
	},
	{
	"rmfile",
	do_rmfile
	},
	{
	"mvfile",
	do_mvfile
	},
	{
	"szfile",
	do_szfile
	},
	{
	"exit",
	do_exit
	},
	{
	"help",
	do_help
	},
	{
	NULL,
	NULL
	} // end mark, do not remove, gives wierd errors! :(
};

int do_help(char* name, char* size) {

	printf("=== File system help ===\n");
	printf("print - вывод информации\n");
	printf("chdir - изменить текущий рабочий каталог (.. ссылается на родительский каталог) \n"); 
	printf("mkdir - создать директорию\n"); 
	printf("rmdir - удалить директорию\n"); 
	printf("mvdir - переименовать директорию\n"); 
	printf("mkfile - создать файл\n"); 
	printf("rmfile - удалить файл\n"); 
	printf("mvfile - переименовать файл\n");
	printf("szfile - изменить размер файла\n");
	printf("exit - выход из файловой системы.\n");
	return 0;
}

/*--------------------------------------------------------------------------------*/
int do_print(char* name, char* size) {

	if (disk_allocated == false) {
		printf("Ошибка: Диск не выделен\n");
		return 0;
	}
	//Начинаем с корневого каталога, который является типом каталога (true)
	printing("root", 0);

	if (DEBUG) 
		printf("\n\t[%s] Вывод информации закончен\n", __func__);

	return 0;
}

/*--------------------------------------------------------------------------------*/
int do_chdir(char* name, char* size) {

	if (disk_allocated == false) {
		printf("Ошибка: Диск не выделен\n");
		return 0;
	}
	//если ".." является аргументом для "chdir",
	if (strcmp(name, "..") == 0) {
		//Если мы находимся в корневом каталоге, то мы не сможем вернуться назад
		if (strcmp(current.directory, "root") == 0)
			return 0;
		//Регулируем структуру working_directory
		strcpy(current.directory, current.parent);
		strcpy(current.parent, get_directory_top_level(current.parent));
		if (DEBUG) printf("\t[%s] Текущий каталог теперь [%s], родительский каталог [%s]\n", __func__, current.directory, current.parent);
		return 0;
	}
	else {
		char tmp[20];
		//Проверяем, что это подкаталог, который необходимо изменить
		//Если имя отсутствует в текущем каталоге, то return -1, иначе return 0
		if ((strcmp(get_directory_subitem(current.directory, -1, name), "-1") == 0) && strcmp(current.parent, name) != 0) {
			if (DEBUG) printf("\t\t\t[%s] Не удается перейти в каталог [%s]\n", __func__, name);
			if (!DEBUG) printf("%s: %s: Нет такого файла или каталога\n", "chdir", name);
			return 0;
		}
		strcpy(tmp, get_directory_name(name));
		if (strcmp(tmp, "") == 0)
			return 0;
		if (strcmp(tmp, name) == 0) {
			//Регулируем структуру working_directory
			strcpy(current.directory, tmp);
			strcpy(current.parent, get_directory_top_level(name));
			if (DEBUG) printf("\t[%s] Текущий каталог теперь [%s], родительский каталог [%s]\n", __func__, current.directory, current.parent);
			return 1;
		}
		return -1;
	}
	return 0;
}
/*--------------------------------------------------------------------------------*/
int do_mkdir(char* name, char* size) {

	if (disk_allocated == false) {
		printf("Ошибка: Диск не выделен\n");
		return 0;
	}

	//If it returns 0, there is a subitem with that name already
	if (get_directory_subitem(current.directory, -1, name) == 0) {

		if (DEBUG) 
			printf("\t\t\t[%s] Невозможно создать каталог [%s]\n", __func__, name);

		if (!DEBUG) 
			printf("%s: не удается создать каталог '%s': он уже существует\n", "mkdir", name);
		return 0;
	}

	//Call add directory
	if (DEBUG) 
		printf("\t[%s] Creating Directory: [%s]\n", __func__, name);

	if (add_directory(name) != 0) {
		if (!DEBUG) 
			printf("%s: отсутствующий операнд\n", "mkdir");
		return 0;
	}
	//Редактируем текущий каталог, чтобы добавить наш новый каталог в элемент "подкаталог" текущего каталога.
	//NULL ==> для простого редактирования подкаталога
	edit_directory(current.directory, name, NULL, false, true);

	if (DEBUG) 
		printf("\t[%s] Обновление содержимого родительского подпункта\n", __func__);

	if (DEBUG) 
		printf("\t[%s] Каталог успешно создан\n", __func__);

	if (DEBUG) 
		print_directory(name);

	return 0;
}
/*--------------------------------------------------------------------------------*/
int do_rmdir(char* name, char* size) {

	if (disk_allocated == false) {
		printf("Ошибка: Диск не выделен\n");
		return 0;
	}

	if (strcmp(name, "") == 0) {
		if (DEBUG) 
			printf("\t[%s] Недопустимая команда\n", __func__);
		if (!DEBUG) 
			printf("%s: отсутствующий операнд\n", "rmdir");
		return 0;
	}

	if ((strcmp(name, ".") == 0) || (strcmp(name, "..") == 0)) {

		if (DEBUG) 
			printf("\t[%s] Недопустимая команда [%s] не удалит каталог\n", __func__, name);
		if (!DEBUG) 
			printf("%s: %s: Нет такого файла или каталога\n", "rmdir", name);
		return 0;
	}

	//Проверяем, что это подкаталог, который необходимо изменить
	//Если имя отсутствует в текущем каталоге, то return -1, иначе return 0
	if (strcmp(get_directory_subitem(current.directory, -1, name), "-1") == 0) {

		if (DEBUG) 
			printf("\t[%s] Не удается удалить каталог [%s]\n", __func__, name);
		if (!DEBUG) 
			printf("%s: %s: Нет такого файла или каталога\n", "rmdir", name);
		return 0;
	}

	//Удаление каталога из родительских подпунктов.
	dir_type* folder = malloc(BLOCK_SIZE);
	int block_index = find_block(name, true);
	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	dir_type* top_folder = malloc(BLOCK_SIZE);

	// top_level создается на основе папки
	int top_block_index = find_block(folder->top_level, true);
	memcpy(disk + block_index * BLOCK_SIZE, folder, BLOCK_SIZE);
	memcpy(top_folder, disk + top_block_index * BLOCK_SIZE, BLOCK_SIZE);

	char subitem_name[MAX_STRING_LENGTH]; // содержит текущий подпункт в массиве родительского каталога
	const int subcnt = top_folder->subitem_count; // количество подпунктов
	int j;
	int k = 0;
	//выполняет итерацию по количеству подпунктов
	for (j = 0; j < subcnt; j++) {
		strcpy(subitem_name, top_folder->subitem[j]);
		if (strcmp(subitem_name, name) != 0) {
			strcpy(top_folder->subitem[k], subitem_name);
			//printf("------ Подпункт [%s] скопирован ------\n", subitem_name);
			k++;
		}
	}

	//Удаление подпункта каталога из родительского
	strcpy(top_folder->subitem[k], "");
	top_folder->subitem_count--;
	memcpy(disk + top_block_index * BLOCK_SIZE, top_folder, BLOCK_SIZE);
	free(top_folder);

	//Удаление каталога вместе с его содержимым
	if (DEBUG) 
		printf("\t[%s] Удаление каталога: [%s]\n", __func__, name);

	if (remove_directory(name) == -1) {
		return 0;
	}

	if (DEBUG) 
		printf("\t[%s] Каталог успешно удален\n", __func__);
	return 0;
}

/*--------------------------------------------------------------------------------*/
int do_mvdir(char* name, char* size) {		//"size" is actually the new name
	
	if (disk_allocated == false) {
		printf("Ошибка: Диск не выделен\n");
		return 0;
	}

	//Переименовываем каталог
	if (DEBUG) 
		printf("\t[%s] Переименование каталога: [%s]\n", __func__, name);

	//если каталог "name" не найден, return -1
	if (edit_directory(name, "", size, true, true) == -1) {
		if (!DEBUG) printf("%s: не удается переименовать файл или каталог '%s'\n", "mvdir", name);
		return 0;
	}

	//в противном случае каталог будет переименован
	if (DEBUG) 
		printf("\t[%s] Каталог успешно переименован: [%s]\n", __func__, size);
	if (DEBUG) 
		print_directory(size);

	return 0;
}
/*--------------------------------------------------------------------------------*/
int do_mkfile(char* name, char* size) {

	if (disk_allocated == false) {
		printf("Ошибка: Диск не выделен\n");
		return 0;
	}

	if (DEBUG) 
		printf("\t[%s] Создаем файл: [%s], размером: [%s]\n", __func__, name, size);

	//Если return 0, то уже существует подпункт с таким именем
	if (get_directory_subitem(current.directory, -1, name) == 0) {
		if (DEBUG) printf("\t\t\t[%s] Не удается создать файл [%s], файл или каталог [%s] уже существует\n", __func__, name, name);
		if (!DEBUG) printf("%s: не удается создать файл '%s': файл существует\n", "mkfile", name);
		return 0;
	}

	if (add_file(name, atoi(size)) != 0)
		return 0;

	//Редактируем текущий каталог, чтобы добавить наш новый файл в элемент "подкаталог" текущего каталога.
	edit_directory(current.directory, name, NULL, false, false);

	if (DEBUG) 
		printf("\t[%s] Обновление содержимого родительского подпункта\n", __func__);

	if (DEBUG) 
		print_file(name);

	return 0;
}
/*--------------------------------------------------------------------------------*/


int do_rmfile(char* name, char* size) {		// Удалить файл

	if (disk_allocated == false) {
		printf("Error: Ошибка: Диск не выделен\n");
		return 0;
	}

	if (DEBUG) 
		printf("\t[%s] Удаление файла: [%s]\n", __func__, name);

	//Если файл, подлежащий удалению, действительно существует в текущем каталоге, удаляем его
	if (strcmp(get_directory_subitem(current.directory, -1, name), "0") == 0) {
		remove_file(name);
	}
	else { // Если он не существует, выводим error and return 0
		if (DEBUG) 
			printf("\t\t\t[%s] Не удается удалить файл [%s], он не существует в этом каталоге\n", __func__, name);
		if (!DEBUG) 
			printf("%s: %s: Нет такого файла или каталога\n", "rmfile", name);
	}

	return 0;
}
/*--------------------------------------------------------------------------------*/

int do_mvfile(char* name, char* size) {		// Переименовать файл

	if (disk_allocated == false) {
		printf("Ошибка: Диск не выделен\n");
		return 0;
	}

	if (DEBUG) 
		printf("\t[%s] Переименовываем файл: [%s], в: [%s]\n", __func__, name, size);

	//return 0, если уже есть подпункт с таким названием
	if (get_directory_subitem(current.directory, -1, size) == 0) {
		if (DEBUG) 
			printf("\t\t\t[%s] Не удается переименовать файл [%s], файл или каталог [%s] уже существует\n", __func__, name, size);
		if (!DEBUG) 
			printf("%s: не удается переименовать файл или каталог '%s'\n", "mvfil", name);
		return 0;
	}

	int er = edit_file(name, 0, size);

	if (er == -1) 
		return -1;

	if (DEBUG) 
		print_file(size);

	return 0;
}
/*--------------------------------------------------------------------------------*/

int do_szfile(char* name, char* size) {		// Resize a file -- addon to be implemented

	if (disk_allocated == false) {
		printf("Ошибка: Диск не выделен\n");
		return 0;
	}

	if (DEBUG) 
		printf("\t[%s] Изменение размера файла: [%s], до: [%s]\n", __func__, name, size);

	//удаляем файл; создаём новый файл с обновлённым размером
	if (remove_file(name) != -1){
		do_mkfile(name, size);
	}
	else {
		if (DEBUG) 
			printf("\t[%s] Файл: [%s] не существует. Невозможно изменить размер.\n", __func__, name);
		if (!DEBUG) 
			printf("%s: не удается изменить размер '%s': такого файла или каталога нет\n", "szfil", name);
	}
	return 0;
}

/*--------------------------------------------------------------------------------*/
int do_exit(char* name, char* size) {

	if (DEBUG) 
		printf("\t[%s] Выход\n", __func__);

	exit(0);
	return 0;
}