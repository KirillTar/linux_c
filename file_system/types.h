#ifndef TYPES_H
#define TYPES_H 1
#include "defines.h"

typedef struct {
	char directory[MAX_STRING_LENGTH];
	int directory_index;
	char parent[MAX_STRING_LENGTH];
	int parent_index;
} working_directory;

typedef struct {
	bool free[BLOCKS];
	bool directory[BLOCKS];
	char(*name)[MAX_STRING_LENGTH];
} descriptor_block;

typedef struct action {
	char* cmd; // указатель на строку
	int(*action)(char* name, char* size); // указатель на функцию
} action;

typedef struct dir_type {
	char name[MAX_STRING_LENGTH]; //Имя файла или каталога
	char top_level[MAX_STRING_LENGTH]; //Имя каталога на один уровень выше (непосредственный родительский)
	char(*subitem)[MAX_STRING_LENGTH];
	bool subitem_type[MAX_SUBDIRECTORIES]; //true если каталог, false если файл
	int subitem_count;
	struct dir_type* next;
} dir_type;

typedef struct file_type {
	char name[MAX_STRING_LENGTH]; //Имя файла или каталога
	char top_level[MAX_STRING_LENGTH]; //Имя каталога на один уровень выше
	int data_block_index[MAX_FILE_DATA_BLOCKS];
	int data_block_count;
	int size;
	struct file_type* next;
} file_type;

#endif // TYPES_H