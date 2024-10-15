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
	printf("print - ����� ����������\n");
	printf("chdir - �������� ������� ������� ������� (.. ��������� �� ������������ �������) \n"); 
	printf("mkdir - ������� ����������\n"); 
	printf("rmdir - ������� ����������\n"); 
	printf("mvdir - ������������� ����������\n"); 
	printf("mkfile - ������� ����\n"); 
	printf("rmfile - ������� ����\n"); 
	printf("mvfile - ������������� ����\n");
	printf("szfile - �������� ������ �����\n");
	printf("exit - ����� �� �������� �������.\n");
	return 0;
}

/*--------------------------------------------------------------------------------*/
int do_print(char* name, char* size) {

	if (disk_allocated == false) {
		printf("������: ���� �� �������\n");
		return 0;
	}
	//�������� � ��������� ��������, ������� �������� ����� �������� (true)
	printing("root", 0);

	if (DEBUG) 
		printf("\n\t[%s] ����� ���������� ��������\n", __func__);

	return 0;
}

/*--------------------------------------------------------------------------------*/
int do_chdir(char* name, char* size) {

	if (disk_allocated == false) {
		printf("������: ���� �� �������\n");
		return 0;
	}
	//���� ".." �������� ���������� ��� "chdir",
	if (strcmp(name, "..") == 0) {
		//���� �� ��������� � �������� ��������, �� �� �� ������ ��������� �����
		if (strcmp(current.directory, "root") == 0)
			return 0;
		//���������� ��������� working_directory
		strcpy(current.directory, current.parent);
		strcpy(current.parent, get_directory_top_level(current.parent));
		if (DEBUG) printf("\t[%s] ������� ������� ������ [%s], ������������ ������� [%s]\n", __func__, current.directory, current.parent);
		return 0;
	}
	else {
		char tmp[20];
		//���������, ��� ��� ����������, ������� ���������� ��������
		//���� ��� ����������� � ������� ��������, �� return -1, ����� return 0
		if ((strcmp(get_directory_subitem(current.directory, -1, name), "-1") == 0) && strcmp(current.parent, name) != 0) {
			if (DEBUG) printf("\t\t\t[%s] �� ������� ������� � ������� [%s]\n", __func__, name);
			if (!DEBUG) printf("%s: %s: ��� ������ ����� ��� ��������\n", "chdir", name);
			return 0;
		}
		strcpy(tmp, get_directory_name(name));
		if (strcmp(tmp, "") == 0)
			return 0;
		if (strcmp(tmp, name) == 0) {
			//���������� ��������� working_directory
			strcpy(current.directory, tmp);
			strcpy(current.parent, get_directory_top_level(name));
			if (DEBUG) printf("\t[%s] ������� ������� ������ [%s], ������������ ������� [%s]\n", __func__, current.directory, current.parent);
			return 1;
		}
		return -1;
	}
	return 0;
}
/*--------------------------------------------------------------------------------*/
int do_mkdir(char* name, char* size) {

	if (disk_allocated == false) {
		printf("������: ���� �� �������\n");
		return 0;
	}

	//If it returns 0, there is a subitem with that name already
	if (get_directory_subitem(current.directory, -1, name) == 0) {

		if (DEBUG) 
			printf("\t\t\t[%s] ���������� ������� ������� [%s]\n", __func__, name);

		if (!DEBUG) 
			printf("%s: �� ������� ������� ������� '%s': �� ��� ����������\n", "mkdir", name);
		return 0;
	}

	//Call add directory
	if (DEBUG) 
		printf("\t[%s] Creating Directory: [%s]\n", __func__, name);

	if (add_directory(name) != 0) {
		if (!DEBUG) 
			printf("%s: ������������� �������\n", "mkdir");
		return 0;
	}
	//����������� ������� �������, ����� �������� ��� ����� ������� � ������� "����������" �������� ��������.
	//NULL ==> ��� �������� �������������� �����������
	edit_directory(current.directory, name, NULL, false, true);

	if (DEBUG) 
		printf("\t[%s] ���������� ����������� ������������� ���������\n", __func__);

	if (DEBUG) 
		printf("\t[%s] ������� ������� ������\n", __func__);

	if (DEBUG) 
		print_directory(name);

	return 0;
}
/*--------------------------------------------------------------------------------*/
int do_rmdir(char* name, char* size) {

	if (disk_allocated == false) {
		printf("������: ���� �� �������\n");
		return 0;
	}

	if (strcmp(name, "") == 0) {
		if (DEBUG) 
			printf("\t[%s] ������������ �������\n", __func__);
		if (!DEBUG) 
			printf("%s: ������������� �������\n", "rmdir");
		return 0;
	}

	if ((strcmp(name, ".") == 0) || (strcmp(name, "..") == 0)) {

		if (DEBUG) 
			printf("\t[%s] ������������ ������� [%s] �� ������ �������\n", __func__, name);
		if (!DEBUG) 
			printf("%s: %s: ��� ������ ����� ��� ��������\n", "rmdir", name);
		return 0;
	}

	//���������, ��� ��� ����������, ������� ���������� ��������
	//���� ��� ����������� � ������� ��������, �� return -1, ����� return 0
	if (strcmp(get_directory_subitem(current.directory, -1, name), "-1") == 0) {

		if (DEBUG) 
			printf("\t[%s] �� ������� ������� ������� [%s]\n", __func__, name);
		if (!DEBUG) 
			printf("%s: %s: ��� ������ ����� ��� ��������\n", "rmdir", name);
		return 0;
	}

	//�������� �������� �� ������������ ����������.
	dir_type* folder = malloc(BLOCK_SIZE);
	int block_index = find_block(name, true);
	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	dir_type* top_folder = malloc(BLOCK_SIZE);

	// top_level ��������� �� ������ �����
	int top_block_index = find_block(folder->top_level, true);
	memcpy(disk + block_index * BLOCK_SIZE, folder, BLOCK_SIZE);
	memcpy(top_folder, disk + top_block_index * BLOCK_SIZE, BLOCK_SIZE);

	char subitem_name[MAX_STRING_LENGTH]; // �������� ������� �������� � ������� ������������� ��������
	const int subcnt = top_folder->subitem_count; // ���������� ����������
	int j;
	int k = 0;
	//��������� �������� �� ���������� ����������
	for (j = 0; j < subcnt; j++) {
		strcpy(subitem_name, top_folder->subitem[j]);
		if (strcmp(subitem_name, name) != 0) {
			strcpy(top_folder->subitem[k], subitem_name);
			//printf("------ �������� [%s] ���������� ------\n", subitem_name);
			k++;
		}
	}

	//�������� ��������� �������� �� �������������
	strcpy(top_folder->subitem[k], "");
	top_folder->subitem_count--;
	memcpy(disk + top_block_index * BLOCK_SIZE, top_folder, BLOCK_SIZE);
	free(top_folder);

	//�������� �������� ������ � ��� ����������
	if (DEBUG) 
		printf("\t[%s] �������� ��������: [%s]\n", __func__, name);

	if (remove_directory(name) == -1) {
		return 0;
	}

	if (DEBUG) 
		printf("\t[%s] ������� ������� ������\n", __func__);
	return 0;
}

/*--------------------------------------------------------------------------------*/
int do_mvdir(char* name, char* size) {		//"size" is actually the new name
	
	if (disk_allocated == false) {
		printf("������: ���� �� �������\n");
		return 0;
	}

	//��������������� �������
	if (DEBUG) 
		printf("\t[%s] �������������� ��������: [%s]\n", __func__, name);

	//���� ������� "name" �� ������, return -1
	if (edit_directory(name, "", size, true, true) == -1) {
		if (!DEBUG) printf("%s: �� ������� ������������� ���� ��� ������� '%s'\n", "mvdir", name);
		return 0;
	}

	//� ��������� ������ ������� ����� ������������
	if (DEBUG) 
		printf("\t[%s] ������� ������� ������������: [%s]\n", __func__, size);
	if (DEBUG) 
		print_directory(size);

	return 0;
}
/*--------------------------------------------------------------------------------*/
int do_mkfile(char* name, char* size) {

	if (disk_allocated == false) {
		printf("������: ���� �� �������\n");
		return 0;
	}

	if (DEBUG) 
		printf("\t[%s] ������� ����: [%s], ��������: [%s]\n", __func__, name, size);

	//���� return 0, �� ��� ���������� �������� � ����� ������
	if (get_directory_subitem(current.directory, -1, name) == 0) {
		if (DEBUG) printf("\t\t\t[%s] �� ������� ������� ���� [%s], ���� ��� ������� [%s] ��� ����������\n", __func__, name, name);
		if (!DEBUG) printf("%s: �� ������� ������� ���� '%s': ���� ����������\n", "mkfile", name);
		return 0;
	}

	if (add_file(name, atoi(size)) != 0)
		return 0;

	//����������� ������� �������, ����� �������� ��� ����� ���� � ������� "����������" �������� ��������.
	edit_directory(current.directory, name, NULL, false, false);

	if (DEBUG) 
		printf("\t[%s] ���������� ����������� ������������� ���������\n", __func__);

	if (DEBUG) 
		print_file(name);

	return 0;
}
/*--------------------------------------------------------------------------------*/


int do_rmfile(char* name, char* size) {		// ������� ����

	if (disk_allocated == false) {
		printf("Error: ������: ���� �� �������\n");
		return 0;
	}

	if (DEBUG) 
		printf("\t[%s] �������� �����: [%s]\n", __func__, name);

	//���� ����, ���������� ��������, ������������� ���������� � ������� ��������, ������� ���
	if (strcmp(get_directory_subitem(current.directory, -1, name), "0") == 0) {
		remove_file(name);
	}
	else { // ���� �� �� ����������, ������� error and return 0
		if (DEBUG) 
			printf("\t\t\t[%s] �� ������� ������� ���� [%s], �� �� ���������� � ���� ��������\n", __func__, name);
		if (!DEBUG) 
			printf("%s: %s: ��� ������ ����� ��� ��������\n", "rmfile", name);
	}

	return 0;
}
/*--------------------------------------------------------------------------------*/

int do_mvfile(char* name, char* size) {		// ������������� ����

	if (disk_allocated == false) {
		printf("������: ���� �� �������\n");
		return 0;
	}

	if (DEBUG) 
		printf("\t[%s] ��������������� ����: [%s], �: [%s]\n", __func__, name, size);

	//return 0, ���� ��� ���� �������� � ����� ���������
	if (get_directory_subitem(current.directory, -1, size) == 0) {
		if (DEBUG) 
			printf("\t\t\t[%s] �� ������� ������������� ���� [%s], ���� ��� ������� [%s] ��� ����������\n", __func__, name, size);
		if (!DEBUG) 
			printf("%s: �� ������� ������������� ���� ��� ������� '%s'\n", "mvfil", name);
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
		printf("������: ���� �� �������\n");
		return 0;
	}

	if (DEBUG) 
		printf("\t[%s] ��������� ������� �����: [%s], ��: [%s]\n", __func__, name, size);

	//������� ����; ������ ����� ���� � ���������� ��������
	if (remove_file(name) != -1){
		do_mkfile(name, size);
	}
	else {
		if (DEBUG) 
			printf("\t[%s] ����: [%s] �� ����������. ���������� �������� ������.\n", __func__, name);
		if (!DEBUG) 
			printf("%s: �� ������� �������� ������ '%s': ������ ����� ��� �������� ���\n", "szfil", name);
	}
	return 0;
}

/*--------------------------------------------------------------------------------*/
int do_exit(char* name, char* size) {

	if (DEBUG) 
		printf("\t[%s] �����\n", __func__);

	exit(0);
	return 0;
}