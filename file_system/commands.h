#ifndef COMMANDS_H
#define COMMANDS_H 1
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../defines.h"
#include "../types.h"
#include "../core.h"

extern bool disk_allocated; //гарантирует, что do_root вызывается первым делом и вызывается только один раз
extern char* disk;
extern working_directory current;

/* команды
* ------- ------
* root - инициализирует корневой каталог
* print - вывод текущего рабочего каталога и его дочерних папок
* chdir - изменить текущий рабочий каталог (.. ссылается на родительский каталог)
* mkdir - создать директорию
* rmdir - удалить директорию
* mvdir - переименовать директорию
* mkfil - создать файл
* rmfil - удалить файл
* mvfil - переименовать файл
* szfil - изменить размер файла
* exit - выход
*/
/*
returns 0 (success) or -1 (failure)
*/

int do_print(char* name, char* size);
int do_chdir(char* name, char* size);
int do_mkdir(char* name, char* size);
int do_rmdir(char* name, char* size);
int do_mvdir(char* name, char* size);
int do_mkfile(char* name, char* size);
int do_rmfile(char* name, char* size);
int do_mvfile(char* name, char* size);
int do_szfile(char* name, char* size);
int do_exit(char* name, char* size);
int do_help(char* name, char* size);

#endif // COMMANDS_H