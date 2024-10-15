#include "core.h"

//Эта функция запускает диск, дескриптор внутри диска, а также корневой каталог
int initFileSystem() {

	if (disk_allocated == true)
		return 0;

	//Запуск диска
	disk = (char*)malloc(DISK_PARTITION);
	if (DEBUG) 
		printf("\t[%s] Распределение [%d] байтов памяти на диске:\n", __func__, DISK_PARTITION);

	//Добавление структуры working_directory
	add_descriptor("descriptor");
	if (DEBUG) 
		printf("\t[%s] Создание блока дескриптора\n", __func__);

	add_directory("root");
	if (DEBUG) 
		printf("\t[%s] Создание корневого каталога\n", __func__);

	//Установка структуры рабочих каталогов
	strcpy(current.directory, "root");
	current.directory_index = 3;
	strcpy(current.parent, "");
	current.parent_index = -1;

	if (DEBUG) 
		printf("\t[%s] Установка текущего каталога в [%s], с родительским каталогом [%s]\n", __func__, "root", "");
	if (DEBUG) 
		printf("\t[%s] Диск успешно выделен\n", __func__);

	disk_allocated = true;
	return 0;
}
/*--------------------------------------------------------------------------------*/


/******************************* Запуск вспомогательных функций *****************************/

//Вывод информации о каталогах и файлах, начинающихся с корневого каталога
void printing(char* name, int deep) {

	//Выделение памяти для dir_type, чтобы мы могли скопировать папку из памяти в эту переменную.
	dir_type* folder = malloc(BLOCK_SIZE);
	int block_index = find_block(name, true);
	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	printf("\033[92m", folder->name);

	for (int i = 0; i < deep; i++) 
		printf("-");

	printf("%s\n\033[0m", folder->name);

	for (int i = 0; i < folder->subitem_count; i++) {

		if (folder->subitem_type[i] == true) {
			//Рекурсивный вызов функции
			printing(folder->subitem[i], deep + 1);
		}
		else {
			for (int i = 0; i < deep + 1; i++) 
				printf("-");
			// file_type * file = malloc(BLOCK_SIZE);
			// int file_block_index = find_block(folder->subitem[i], false);
			// memcpy(file, disk + file_block_index * BLOCK_SIZE, BLOCK_SIZE);
			printf("%s\n", folder->subitem[i]); // , file->name, file->size
		}
	}
}
/*--------------------------------------------------------------------------------*/

//Отображает содержимое блока дескрипторов и таблицы свободных блоков.
void print_descriptor() {

	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	memcpy(descriptor, disk, BLOCK_SIZE * 2);
	printf("Таблица свободных дисковых дескрипторов:\n");

	for (int i = 0; i < BLOCKS; i++) {
		printf("\tИндекс %d : %d\n", i, descriptor->free[i]);
	}

	free(descriptor);
}
/*--------------------------------------------------------------------------------*/

//Находит первый свободный блок на диске; возвращает индекс свободного блока
int allocate_block(char* name, bool directory) {

	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	memcpy(descriptor, disk, BLOCK_SIZE * 2);

	//Проходит через каждый блок, пока не будет найден свободный
	if (DEBUG) 
		printf("\t\t\t[%s] Поиск свободного блока памяти в дескрипторе\n", __func__);

	for (int i = 0; i < BLOCKS; i++) {
		if (descriptor->free[i]) {
			//Как только свободный блок будет найден, обновляет информацию о дескрипторе
			descriptor->free[i] = false;
			descriptor->directory[i] = directory;
			strcpy(descriptor->name[i], name);
			//Возвращает дескриптор в начало диска
			memcpy(disk, descriptor, BLOCK_SIZE * 2);

			if (DEBUG) 
				printf("\t\t\t[%s] Распределено [%s] в блоке памяти [%d]\n", __func__, name, i);

			free(descriptor);
			return i;
		}
	}

	free(descriptor);

	if (DEBUG) 
		printf("\t\t\t[%s] Не найдено свободного места: Returning -1\n", __func__);

	return -1;
}
/*--------------------------------------------------------------------------------*/

//обновляет блок дескриптора на диске, чтобы отразить, что блок больше не используется.
void unallocate_block(int offset) {

	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	memcpy(descriptor, disk, BLOCK_SIZE * 2);
	//проверка содержится ли в блоке файл, а затем освобождение всех его подблоков

	if (DEBUG) 
		printf("\t\t\t[%s] Освобождающий блок памяти [%d]\n", __func__, offset);

	descriptor->free[offset] = true;
	strcpy(descriptor->name[offset], "");
	memcpy(disk, descriptor, BLOCK_SIZE * 2);
	free(descriptor);
}
/*--------------------------------------------------------------------------------*/

//Принимает имя и выполняет поиск по блоку дескриптора, чтобы найти блок, содержащий элемент
int find_block(char* name, bool directory) {
	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	memcpy(descriptor, disk, BLOCK_SIZE * 2);
	if (DEBUG) 
		printf("\t\t\t[%s] Поиск дескриптора для [%s], который является [%s]\n", __func__, name, directory == true ? "папкой" : "файлом");

	for (int i = 0; i < BLOCKS; i++) {
		if (strcmp(descriptor->name[i], name) == 0) {
			//Убеждаемся, что он относится к тому типу, который мы ищем
			if (descriptor->directory[i] == directory) {
				if (DEBUG) printf("\t\t\t[%s] Поиск [%s] в блоке памяти [%d]\n", __func__, name, i);
				free(descriptor);
				//Возвращает индекс блока, в котором элемент находится в памяти
				return i;
			}
		}
	}

	free(descriptor);
	if (DEBUG) 
		printf("\t\t\t[%s] Блок не найден: Returning -1\n", __func__);

	return -1;
}
/*--------------------------------------------------------------------------------*/

int add_descriptor(char* name) {

	//Выделяет память типу descriptor_block, чтобы мы начали присваивать значения его элементам.
	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	if (DEBUG) 
		printf("\t\t[%s] Allocating Space for Descriptor Block\n", __func__);

	//Выделяет память массиву строк внутри блока дескриптора, который содержит имя каждого блока
	descriptor->name = malloc(sizeof * name * BLOCKS);
	if (DEBUG) 
		printf("\t\t[%s] Выделение пространства для элемента имени дескриптора\n", __func__);

	//инициализируем каждый блок ==> чтобы он был свободен
	if (DEBUG) 
		printf("\t\t[%s] Инициализирующий дескриптор для получения всей доступной памяти\n", __func__);

	for (int i = 0; i < BLOCKS; i++) {
		descriptor->free[i] = true;
		descriptor->directory[i] = false;
	}

	//дескриптор занял место на диске
	int limit = (int)(sizeof(descriptor_block) / BLOCK_SIZE) + 1;
	if (DEBUG) 
		printf("\t\t[%s] Обновление дескриптора, чтобы показать, что [%d] первые блоки памяти заняты\n", __func__, limit + 1);

	for (int i = 0; i < limit; i++) {
		descriptor->free[i] = false; //отмечает пространство, занимаемое используемым дескриптором
	}
	strcpy(descriptor->name[0], "дескриптор");
	//запись обновленного дескриптора в начало диска
	memcpy(disk, descriptor, (BLOCK_SIZE * (limit + 1)));
	return 0;
}
/*--------------------------------------------------------------------------------*/

//Позволяет нам напрямую обновлять значения в блоке дескриптора.
int edit_descriptor(int free_index, bool free, int name_index, char* name) {

	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	//Копирует дескриптор на диск в descriptor_block
	memcpy(descriptor, disk, BLOCK_SIZE * 2);
	//Каждый массив в блоке дескриптора будет обновлен
	if (free_index > 0) {
		descriptor->free[free_index] = free;
		if (DEBUG)
			printf("\t\t[%s] Свободный элемент дескриптора теперь показывает блок памяти [%d][%s]\n", __func__, free_index, free == true ? "Свободен" : "Используется");
	}

	if (name_index > 0) {

		strcpy(descriptor->name[name_index], name);
		if (DEBUG) 
			printf("\t\t[%s] Элемент имени дескриптора теперь отображает блок памяти [%d][%s]\n", __func__, name_index, name);
	}
	// запись обновленного дескриптора обратно в начало диска
	memcpy(disk, descriptor, BLOCK_SIZE * 2);
	return 0;
}
/*--------------------------------------------------------------------------------*/

//изменяет имя файла в дескрипторе; используется для перемещения файлов;
int edit_descriptor_name(int index, char* new_name) {

	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	memcpy(descriptor, disk, BLOCK_SIZE * 2);
	// Изменяет имя файла в индексе на new_name
	strcpy(descriptor->name[index], new_name);
	memcpy(disk, descriptor, BLOCK_SIZE * 2);
	free(descriptor);
	return 0;
}
/*--------------------------------------------------------------------------------*/

//Позволяет нам добавить папку на диск.
int add_directory(char* name) {

	if (strcmp(name, "") == 0) {
		if (DEBUG) 
			printf("\t\t[%s] Недопустимая команда\n", __func__);
		return -1;
	}

	//Выделение памяти для новой папки
	dir_type* folder = malloc(BLOCK_SIZE);

	if (DEBUG) 
		printf("\t\t[%s] Выделение места для новой папки\n", __func__);

	//Инициализируем новую папку
	strcpy(folder->name, name);
	strcpy(folder->top_level, current.directory);
	folder->subitem = malloc(sizeof * (folder->subitem) * MAX_SUBDIRECTORIES);
	folder->subitem_count = 0; // Imp : Инициализирует массив подэлементов так, чтобы он содержал 0 элементов
	//Находит свободный блок на диске для хранения папки; true => помечает блок как каталог
	int index = allocate_block(name, true);

	if (DEBUG) 
		printf("\t\t[%s] Назначение новой папки блоку памяти [%d]\n", __func__, index);

	//Скопирует папку на диск
	memcpy(disk + index * BLOCK_SIZE, folder, BLOCK_SIZE);

	if (DEBUG) 
		printf("\t\t[%s] Folder [%s] Успешно добавлен\n", __func__, name);

	free(folder);
	return 0;
}
/*--------------------------------------------------------------------------------*/

//Позволяет удалить каталог-папку с диска.
int remove_directory(char* name) {
	dir_type* folder = malloc(BLOCK_SIZE);
	int block_index = find_block(name, true);
	//Если подкаталог не был найден, то return -1

	if (block_index == -1) {
		if (DEBUG) printf("\t\t[%s] Каталог [%s] не существует в текущей папке [%s]\n", __func__, name, current.directory);
		return -1;
	}

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);

	//Пройдёт еще раз, если есть подкаталог ==> как реализовано в Unix
	for (int i = 0; i < folder->subitem_count; i++) {
		if (folder->subitem_type[i] == true) {
			//Рекурсивный вызов функции, чтобы удалить подпункт
			remove_directory(folder->subitem[i]);
		}
		else {
			//Удаляет подпункт, который является файлом
			remove_file(folder->subitem[i]);
		}
	}
	unallocate_block(block_index);
	free(folder);
	return 0;
}
/*--------------------------------------------------------------------------------*/

//Позволяет напрямую добавлять элементы в массив подэлементов folders или изменять название папки
int edit_directory(char* name, char* subitem_name, char* new_name, bool name_change, bool directory) {

	if (strcmp(name, "") == 0) {
		if (DEBUG) printf("\t\t[%s] Недопустимая команда\n", __func__);
		return -1;
	}

	dir_type* folder = malloc(BLOCK_SIZE);
	//Находит, где находится папка на диске
	int block_index = find_block(name, true);
	//Если каталог не найден, return

	if (block_index == -1) {
		if (DEBUG) 
			printf("\t\t[%s] Каталог [%s] не существует\n", __func__, name);
		return -1;
	}

	if (DEBUG) 
		printf("\t\t[%s] Папка [%s] найдена в блоке памяти [%d]\n", __func__, name, block_index);

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);

	if (strcmp(subitem_name, "") != 0) { //Случай, когда мы добавляем подпункт в блок дескриптора
		if (!name_change) { //добавление подпункта
			if (DEBUG) 
				printf("\t\t[%s] Добавлен подпункт [%s] по индексу подпункта [%d] в каталог [%s]\n", __func__, subitem_name, folder->subitem_count, folder->name);

			strcpy(folder->subitem[folder->subitem_count], subitem_name);
			folder->subitem_type[folder->subitem_count] = directory;
			folder->subitem_count++;

			if (DEBUG) 
				printf("\t\t[%s] У папки [%s] теперь есть [%d] подпункты\n", __func__, name, folder->subitem_count);
			//обновляем и диск тоже!
			memcpy(disk + block_index * BLOCK_SIZE, folder, BLOCK_SIZE);
			free(folder);
			return 0;
		}
		else { //редактирование названия подпункта
			for (int i = 0; i < folder->subitem_count; i++) {
				if (strcmp(folder->subitem[i], subitem_name) == 0) {
					strcpy(folder->subitem[i], new_name);

					if (DEBUG) 
						printf("\t\t[%s] Отредактированный подпункт [%s] к [%s] по индексу подпункта [%d] для каталога [%s]\n", __func__, subitem_name, new_name, i, folder->name);

					memcpy(disk + block_index * BLOCK_SIZE, folder, BLOCK_SIZE);
					free(folder);
					return 0;
				}
			}
			if (DEBUG) 
				printf("\t\t[%s] Подпункт не существует в каталоге [%s]\n", __func__, folder->name);
			free(folder);
			return -1;
		}
	}
	else { //меняем название папки

		//если каталог с заданным именем уже существует, не разрешает
		int block_index2 = find_block(new_name, true);
		//Если каталог для нового имени уже существует return -1

		if (block_index2 != -1) {
			if (DEBUG) printf("\t\t[%s] Каталог [%s] уже существует. Выберите другое имя\n", __func__, new_name);
			return -1;
		}

		strcpy(folder->name, new_name);

		if (DEBUG) 
			printf("\t\t[%s] Папка [%s] сейчас называется [%s]\n", __func__, name, folder->name);

		memcpy(disk + block_index * BLOCK_SIZE, folder, BLOCK_SIZE);
		//редактируем дескрипторы
		edit_descriptor(-1, false, block_index, new_name);

		if (DEBUG) 
			printf("\t\t[%s] Обновленный элемент имени дескриптора\n", __func__);
		if (DEBUG) 
			print_directory(folder->name);

		//изменение родительского имени
		edit_directory(folder->top_level, name, new_name, true, true);

		if (DEBUG) 
			printf("\t\t[%s] Обновлено название родительского подпункта\n", __func__);

		int child_index;
		//Выполняет итерацию, чтобы изменить название верхнего уровня подпунктов
		
		for (int i = 0; i < folder->subitem_count; i++) {

			file_type* child_file = malloc(BLOCK_SIZE);
			dir_type* child_folder = malloc(BLOCK_SIZE);
			child_index = find_block(folder->subitem[i], folder->subitem_type);

			if (folder->subitem_type[i]) {
				//if type == folder
				memcpy(child_folder, disk + child_index * BLOCK_SIZE, BLOCK_SIZE);
				strcpy(child_folder->top_level, new_name);
				memcpy(disk + child_index * BLOCK_SIZE, child_folder, BLOCK_SIZE);
				free(child_folder);
				free(child_file);
			}
			else {
				//if type == file
				memcpy(child_file, disk + child_index * BLOCK_SIZE, BLOCK_SIZE);
				strcpy(child_file->top_level, new_name);
				memcpy(disk + child_index * BLOCK_SIZE, child_file, BLOCK_SIZE);
				free(child_folder);
				free(child_file);
			}
		}
		free(folder);
		return 0;
	}
	free(folder);
}
/*--------------------------------------------------------------------------------*/

//Позволяет нам добавить файл на наш диск; Эта функция выделит этот блок файлового дескриптора (содержит информацию о файле), а также блоки данных
int add_file(char* name, int size) {

	char subname[20];

	if (size < 0 || strcmp(name, "") == 0) {

		if (DEBUG) 
			printf("\t\t[%s] Недопустимая команда\n", __func__);
		if (!DEBUG) 
			printf("%s: отсутствующий операнд\n", "mkfile");

		return 1;
	}

	//Выделяем память для file_type
	file_type* file = malloc(BLOCK_SIZE);

	if (DEBUG) 
		printf("\t\t[%s] Выделение места для нового файла\n", __func__);

	//Инициализируем все элементы нашего нового файла
	strcpy(file->name, name);
	strcpy(file->top_level, current.directory);

	file->size = size;
	file->data_block_count = 0;

	if (DEBUG) 
		printf("\t\t[%s] Инициализация элементов файла\n", __func__);

	//Найдите свободный блок, чтобы поместить этот блок файлового дескриптора в память, false ==> указывает на файл
	int index = allocate_block(name, false);

	//Найдите свободные блоки для размещения данных файла
	if (DEBUG) 
		printf("\t\t[%s] Распределяющие [%d] блоки данных в памяти для файловых данных\n", __func__, (int)size / BLOCK_SIZE);

	for (int i = 0; i < size / BLOCK_SIZE + 1; i++) {
		sprintf(subname, "%s->%d", name, i);
		file->data_block_index[i] = allocate_block(subname, false);
		file->data_block_count++;
	}

	//блоки данных в памяти, не скопированные на диск
	memcpy(disk + index * BLOCK_SIZE, file, BLOCK_SIZE);

	if (DEBUG) 
		printf("\t\t[%s] Файл [%s] успешно добавлен\n", __func__, name);

	free(file);
	return 0;
}

/*--------------------------------------------------------------------------------*/
int remove_file(char* name) {

	if (strcmp(name, "") == 0) {

		if (DEBUG) 
			printf("\t\t[%s] Недопустимая команда\n", __func__);
		if (!DEBUG) 
			printf("%s: отсутствующий операнд\n", "rmfil");

		return 1;
	}

	file_type* file = malloc(BLOCK_SIZE);
	dir_type* folder = malloc(BLOCK_SIZE);
	int file_index = find_block(name, false);

	// Если файл не найден, error, return -1
	if (file_index == -1) {
		if (DEBUG) printf("\t\t\t[%s] Файл [%s] не найден\n", __func__, name);
		return -1;
	}

	if (DEBUG) 
		printf("\t\t[%s] Файл [%s] найден в блоке памяти [%d]\n", __func__, name, file_index);

	memcpy(file, disk + file_index * BLOCK_SIZE, BLOCK_SIZE);

	//Найдите папку top_level на диске
	int folder_index = find_block(file->top_level, true);

	if (DEBUG) 
		printf("\t\t[%s] Папка [%s] найдена в блоке памяти [%d]\n", __func__, name, folder_index);

	memcpy(folder, disk + folder_index * BLOCK_SIZE, BLOCK_SIZE);

	// Проходимся по массиву подэлементов родительского каталога и удаляем наш файл
	char subitem_name[MAX_STRING_LENGTH];
	const int subcnt = folder->subitem_count; // количество подпунктов
	int j;
	int k = 0;

	for (j = 0; j < subcnt; j++) {
		strcpy(subitem_name, folder->subitem[j]);
		if (strcmp(subitem_name, name) != 0)
			// если этот элемент не тот, который мы удаляем, скопируем обратно
		{
			strcpy(folder->subitem[k], subitem_name);
			k++;
		}
	}

	strcpy(folder->subitem[k], "");
	folder->subitem_count--;
	memcpy(disk + folder_index * BLOCK_SIZE, folder, BLOCK_SIZE); // Обновляет папку в памяти

	//Imp : Отменяет выделение всех блоков данных из файла, который мы удаляем
	int i = 0;
	while (file->data_block_count != 0) {
		unallocate_block(file->data_block_index[i]);
		file->data_block_count--;
		i++;
	}

	unallocate_block(file_index); // Освободите блок управления файлом
	free(folder);
	free(file);
	return 0;
}
/*--------------------------------------------------------------------------------*/

//Позволяет вам напрямую редактировать файл и изменять его размер или название
int edit_file(char* name, int size, char* new_name) {

	file_type* file = malloc(BLOCK_SIZE);
	//Находит блок в памяти, в который записан этот файл
	int block_index = find_block(name, false);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] Файл [%s] не найден\n", __func__, name);

		return -1;
	}

	if (DEBUG) 
		printf("\t\t[%s] Файл [%s] найден в блоке памяти [%d]\n", __func__, name, block_index);

	memcpy(file, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);

	if (size > 0) {
		//Если размер больше нуля, то размер файлов будет обновлен
		file->size = size;

		if (DEBUG) 
			printf("\t\t[%s] Размер файла [%s] [%d]\n", __func__, name, size);

		free(file);
		return 0;
	}
	else {
		//В противном случае имя файла будет обновлено
		char top_level[MAX_STRING_LENGTH];
		strcpy(top_level, get_file_top_level(name));
		// Изменяет название подпункта каталога
		edit_directory_subitem(top_level, name, new_name);
		// Изменяет имя фактического файлового дескриптора
		edit_descriptor_name(block_index, new_name);
		strcpy(file->name, new_name);
		memcpy(disk + block_index * BLOCK_SIZE, file, BLOCK_SIZE);

		if (DEBUG) 
			printf("\t\t\t[%s] Файл [%s] сейчас называется [%s]\n", __func__, name, file->name);

		free(file);
		return 0;
	}
}
/*--------------------------------------------------------------------------------*/


/************************** Getter functions ************************************/

char* get_directory_name(char* name) {

	dir_type* folder = malloc(BLOCK_SIZE);
	char* tmp = malloc(sizeof(char) * MAX_STRING_LENGTH);

	//Аргумент True сообщает функции find, что мы ищем для каталога, а не для файла
	int block_index = find_block(name, true);

	if (block_index == -1) {

		if (DEBUG)
			printf("\t\t\t[%s] Папка [%s] не найдена\n", __func__, name);

		strcpy(tmp, "");
		return tmp;
	}
	
	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	strcpy(tmp, folder->name);

	if (DEBUG) 
		printf("\t\t\t[%s] Имя [%s] найдено для [%s] папки\n", __func__, tmp, name);

	free(folder);
	return tmp;
}
/*--------------------------------------------------------------------------------*/

char* get_directory_top_level(char* name) {

	dir_type* folder = malloc(BLOCK_SIZE);
	char* tmp = malloc(sizeof(char) * MAX_STRING_LENGTH);

	//true ==> указывает папку, а не файл
	int block_index = find_block(name, true);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] Папка [%s] не найдена\n", __func__, name);

		strcpy(tmp, "");
		return tmp;
	}

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	strcpy(tmp, folder->top_level);

	if (DEBUG) 
		printf("\t\t\t[%s] top_level [%s] найден для [%s] папки\n", __func__, tmp, name);

	free(folder);
	return tmp;
}
/*--------------------------------------------------------------------------------*/

char* get_directory_subitem(char* name, int subitem_index, char* subitem_name) {

	dir_type* folder = malloc(BLOCK_SIZE);
	char* tmp = malloc(sizeof(char) * MAX_STRING_LENGTH);
	int block_index = find_block(name, true);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] Папка [%s] не найдена\n", __func__, name);

		strcpy(tmp, "");
		return tmp;
	}

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);

	if (subitem_index >= 0) {
		//меняем название подпункта
		strcpy(tmp, folder->subitem[subitem_index]);

		if (DEBUG) 
			printf("\t\t\t[%s] подпункт[%d] = [%s] для [%s] папки\n", __func__, subitem_index, tmp, name);

		free(folder);
		return tmp;
	}
	else {
		//ищем подпункт
		for (int i = 0; i < folder->subitem_count; i++) {
			if (strcmp(folder->subitem[i], subitem_name) == 0) {

				if (DEBUG) 
					printf("\t\t\t[%s] Найден [%s] в качестве подпункта каталога [%s]\n", __func__, subitem_name, name);

				return "0";
			}
		}

		if (DEBUG) 
			printf("\t\t\t[%s] Не найден [%s] в качестве подпункта каталога [%s]\n", __func__, subitem_name, name);

		free(folder);
		return "-1";
	}
	free(folder);
	return tmp;
}
/*--------------------------------------------------------------------------------*/

int edit_directory_subitem(char* name, char* sub_name, char* new_sub_name) {

	dir_type* folder = malloc(BLOCK_SIZE);
	//Аргумент True сообщает функции find, что мы ищем для каталога, а не для файла
	int block_index = find_block(name, true);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] Папка [%s] не найдена\n", __func__, name);

	}

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	const int cnt = folder->subitem_count;
	int i;

	for (i = 0; i < cnt; i++) {
		if (strcmp(folder->subitem[i], sub_name) == 0) {
			strcpy(folder->subitem[i], new_sub_name);

			if (DEBUG) 
				printf("\t\t\t[%s] Отредактированный подпункт в %s от %s до %s\n", __func__, folder->name, sub_name, folder->subitem[i]);

			memcpy(disk + block_index * BLOCK_SIZE, folder, BLOCK_SIZE);
			free(folder);
			return i;
		}
	}
	free(folder);

	return -1;
}
/*--------------------------------------------------------------------------------*/

int get_directory_subitem_count(char* name) {

	dir_type* folder = malloc(BLOCK_SIZE);
	int tmp;
	int block_index = find_block(name, true);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] Папка [%s] не найдена\n", __func__, name);

		return -1;
	}

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	tmp = folder->subitem_count;

	if (DEBUG) 
		printf("\t\t\t[%s] количество подэлементов [%d] найдено для папки [%s]\n", __func__, folder->subitem_count, name);

	free(folder);
	return tmp;
}
/*--------------------------------------------------------------------------------*/

char* get_file_name(char* name) {

	file_type* file = malloc(BLOCK_SIZE);
	char* tmp = malloc(sizeof(char) * MAX_STRING_LENGTH);
	//false ==> указывает на файл, а не на папку
	int block_index = find_block(name, false);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] Файл [%s] не найден\n", __func__, name);

		strcpy(tmp, "");
		return tmp;
	}

	memcpy(file, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	strcpy(tmp, file->name);

	if (DEBUG) 
		printf("\t\t\t[%s] Найдено имя [%s] для файла [%s]\n", __func__, tmp, name);

	free(file);
	return tmp;
}
/*--------------------------------------------------------------------------------*/

char* get_file_top_level(char* name) {

	file_type* file = malloc(BLOCK_SIZE);
	char* tmp = malloc(sizeof(char) * MAX_STRING_LENGTH);
	int block_index = find_block(name, false);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] Файл [%s] не найден\n", __func__, name);

		strcpy(tmp, "");
		return tmp;
	}

	memcpy(file, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	strcpy(tmp, file->top_level);

	if (DEBUG) 
		printf("\t\t\t[%s] top_level [%s] найдено для файла [%s]\n", __func__, tmp, name);

	free(file);
	return tmp;
}
/*--------------------------------------------------------------------------------*/

int get_file_size(char* name) {

	file_type* file = malloc(BLOCK_SIZE);
	int tmp;
	int block_index = find_block(name, false);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] Файл [%s] не найден\n", __func__, name);

		return -1;
	}

	memcpy(file, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	tmp = file->size;

	if (DEBUG) 
		printf("\t\t\t[%s] размер [%d], найденный для файла [%s]\n", __func__, tmp, name);

	free(file);
	return tmp;
}
/*--------------------------------------------------------------------------------*/


/********************************* Print Functions ********************************/
void print_directory(char* name) {

	dir_type* folder = malloc(BLOCK_SIZE);
	int block_index = find_block(name, true);
	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);

	printf(" -----------------------------\n");
	printf(" Атрибуты новой папки:\n\n\tимя = %s\n\ttop_level = %s\n\tподпункты = ", folder->name, folder->top_level);

	for (int i = 0; i < folder->subitem_count; i++) {
		printf("%s ", folder->subitem[i]);
	}

	printf("\n\tколичество подпунктов = %d\n", folder->subitem_count);
	printf(" -----------------------------\n");
	free(folder);
}

void print_file(char* name) {

	file_type* file = malloc(BLOCK_SIZE);
	int block_index = find_block(name, false);
	memcpy(file, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);

	printf(" -----------------------------\n");
	printf(" Новые атрибуты файла:\n\n\tимя = %s\n\ttop_level = %s\n\tразмер файла = %d\n\tblock count = %d\n", file->name, file->top_level, file->size, file->data_block_count);
	printf(" -----------------------------\n");
	free(file);
}
/*--------------------------------------------------------------------------------*/