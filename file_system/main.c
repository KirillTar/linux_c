#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include "defines.h"
#include "commands.h"

extern bool disk_allocated; // гарантирует, что do_root вызывается первым делом и вызывается только один раз
extern char* disk;
extern working_directory current;
extern struct action table[];

/*--------------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {

	char in[LINESIZE];
	char* cmd, * fnm, * fsz;
	char dummy[] = "";
	system("clear");

	printf("Инициализация...\n");
	initFileSystem();
	sleep(2);
	system("clear");

	printf("=== File system by 1044™️ ===\n");
	printf("Введите help для списка команд.\n");

	// if (DEBUG) printf("размер file_type = %d\nразмер dir_type = %d\nразмер descriptor_block = %d\nMAX_FILE_SIZE %d\n", sizeof(file_type), sizeof(dir_type), sizeof(descriptor_block), MAX_FILE_DATA_BLOCKS );
	int n;
	char* a[LINESIZE];
	printf(">");

	while (fgets(in, LINESIZE, stdin) != NULL) {
		//все команды имеют форму "cmd filename filesize\n" с пробелом в качестве разделителя
		// анализируем вводимые данные
		parse(in, &n, a);
		cmd = (n > 0) ? a[0] : dummy;
		fnm = (n > 1) ? a[1] : dummy;
		fsz = (n > 2) ? a[2] : dummy;
		// if (DEBUG) printf(":%s:%s:%s:\n", cmd, fnm, fsz);
		if (n == 0) continue; // пустая строка
		int found = 0;
		for (struct action* ptr = table; ptr->cmd != NULL; ptr++) {
			if (strcmp(ptr->cmd, cmd) == 0) {
				found = 1;
				int ret = (ptr->action)(fnm, fsz);
				//каждая функция возвращает -1 при сбое
				if (ret == -1) {
					printf(" %s %s %s: неудачно\n", cmd, fnm, fsz);
				}
				break;
			}
		}
		if (!found) {
			printf("Команда не найдена: %s\n", cmd);
		}
		printf(">");
	}

	return 0;
}

/*--------------------------------------------------------------------------------*/
void parse(char* buf, int* argc, char* argv[]) {

	char* delim; // указывает на первый разделитель
	int count = 0; // number of args
	char whsp[] = " \t\n\v\f\r"; // пробельные символы

	while (1) // build the argv list
	{
		buf += strspn(buf, whsp); // пропустить начальный пробел
		delim = strpbrk(buf, whsp); // следующий пробельный символ или NULL

		if (delim == NULL) // конец строки, входные данные проанализированыы
		{
			break;
		}

		argv[count++] = buf;
		*delim = '\0';
		buf = delim + 1;
	}

	argv[count] = NULL;
	*argc = count;
	return;
}