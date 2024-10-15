#include "core.h"

//��� ������� ��������� ����, ���������� ������ �����, � ����� �������� �������
int initFileSystem() {

	if (disk_allocated == true)
		return 0;

	//������ �����
	disk = (char*)malloc(DISK_PARTITION);
	if (DEBUG) 
		printf("\t[%s] ������������� [%d] ������ ������ �� �����:\n", __func__, DISK_PARTITION);

	//���������� ��������� working_directory
	add_descriptor("descriptor");
	if (DEBUG) 
		printf("\t[%s] �������� ����� �����������\n", __func__);

	add_directory("root");
	if (DEBUG) 
		printf("\t[%s] �������� ��������� ��������\n", __func__);

	//��������� ��������� ������� ���������
	strcpy(current.directory, "root");
	current.directory_index = 3;
	strcpy(current.parent, "");
	current.parent_index = -1;

	if (DEBUG) 
		printf("\t[%s] ��������� �������� �������� � [%s], � ������������ ��������� [%s]\n", __func__, "root", "");
	if (DEBUG) 
		printf("\t[%s] ���� ������� �������\n", __func__);

	disk_allocated = true;
	return 0;
}
/*--------------------------------------------------------------------------------*/


/******************************* ������ ��������������� ������� *****************************/

//����� ���������� � ��������� � ������, ������������ � ��������� ��������
void printing(char* name, int deep) {

	//��������� ������ ��� dir_type, ����� �� ����� ����������� ����� �� ������ � ��� ����������.
	dir_type* folder = malloc(BLOCK_SIZE);
	int block_index = find_block(name, true);
	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	printf("\033[92m", folder->name);

	for (int i = 0; i < deep; i++) 
		printf("-");

	printf("%s\n\033[0m", folder->name);

	for (int i = 0; i < folder->subitem_count; i++) {

		if (folder->subitem_type[i] == true) {
			//����������� ����� �������
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

//���������� ���������� ����� ������������ � ������� ��������� ������.
void print_descriptor() {

	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	memcpy(descriptor, disk, BLOCK_SIZE * 2);
	printf("������� ��������� �������� ������������:\n");

	for (int i = 0; i < BLOCKS; i++) {
		printf("\t������ %d : %d\n", i, descriptor->free[i]);
	}

	free(descriptor);
}
/*--------------------------------------------------------------------------------*/

//������� ������ ��������� ���� �� �����; ���������� ������ ���������� �����
int allocate_block(char* name, bool directory) {

	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	memcpy(descriptor, disk, BLOCK_SIZE * 2);

	//�������� ����� ������ ����, ���� �� ����� ������ ���������
	if (DEBUG) 
		printf("\t\t\t[%s] ����� ���������� ����� ������ � �����������\n", __func__);

	for (int i = 0; i < BLOCKS; i++) {
		if (descriptor->free[i]) {
			//��� ������ ��������� ���� ����� ������, ��������� ���������� � �����������
			descriptor->free[i] = false;
			descriptor->directory[i] = directory;
			strcpy(descriptor->name[i], name);
			//���������� ���������� � ������ �����
			memcpy(disk, descriptor, BLOCK_SIZE * 2);

			if (DEBUG) 
				printf("\t\t\t[%s] ������������ [%s] � ����� ������ [%d]\n", __func__, name, i);

			free(descriptor);
			return i;
		}
	}

	free(descriptor);

	if (DEBUG) 
		printf("\t\t\t[%s] �� ������� ���������� �����: Returning -1\n", __func__);

	return -1;
}
/*--------------------------------------------------------------------------------*/

//��������� ���� ����������� �� �����, ����� ��������, ��� ���� ������ �� ������������.
void unallocate_block(int offset) {

	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	memcpy(descriptor, disk, BLOCK_SIZE * 2);
	//�������� ���������� �� � ����� ����, � ����� ������������ ���� ��� ���������

	if (DEBUG) 
		printf("\t\t\t[%s] ������������� ���� ������ [%d]\n", __func__, offset);

	descriptor->free[offset] = true;
	strcpy(descriptor->name[offset], "");
	memcpy(disk, descriptor, BLOCK_SIZE * 2);
	free(descriptor);
}
/*--------------------------------------------------------------------------------*/

//��������� ��� � ��������� ����� �� ����� �����������, ����� ����� ����, ���������� �������
int find_block(char* name, bool directory) {
	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	memcpy(descriptor, disk, BLOCK_SIZE * 2);
	if (DEBUG) 
		printf("\t\t\t[%s] ����� ����������� ��� [%s], ������� �������� [%s]\n", __func__, name, directory == true ? "������" : "������");

	for (int i = 0; i < BLOCKS; i++) {
		if (strcmp(descriptor->name[i], name) == 0) {
			//����������, ��� �� ��������� � ���� ����, ������� �� ����
			if (descriptor->directory[i] == directory) {
				if (DEBUG) printf("\t\t\t[%s] ����� [%s] � ����� ������ [%d]\n", __func__, name, i);
				free(descriptor);
				//���������� ������ �����, � ������� ������� ��������� � ������
				return i;
			}
		}
	}

	free(descriptor);
	if (DEBUG) 
		printf("\t\t\t[%s] ���� �� ������: Returning -1\n", __func__);

	return -1;
}
/*--------------------------------------------------------------------------------*/

int add_descriptor(char* name) {

	//�������� ������ ���� descriptor_block, ����� �� ������ ����������� �������� ��� ���������.
	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	if (DEBUG) 
		printf("\t\t[%s] Allocating Space for Descriptor Block\n", __func__);

	//�������� ������ ������� ����� ������ ����� �����������, ������� �������� ��� ������� �����
	descriptor->name = malloc(sizeof * name * BLOCKS);
	if (DEBUG) 
		printf("\t\t[%s] ��������� ������������ ��� �������� ����� �����������\n", __func__);

	//�������������� ������ ���� ==> ����� �� ��� ��������
	if (DEBUG) 
		printf("\t\t[%s] ���������������� ���������� ��� ��������� ���� ��������� ������\n", __func__);

	for (int i = 0; i < BLOCKS; i++) {
		descriptor->free[i] = true;
		descriptor->directory[i] = false;
	}

	//���������� ����� ����� �� �����
	int limit = (int)(sizeof(descriptor_block) / BLOCK_SIZE) + 1;
	if (DEBUG) 
		printf("\t\t[%s] ���������� �����������, ����� ��������, ��� [%d] ������ ����� ������ ������\n", __func__, limit + 1);

	for (int i = 0; i < limit; i++) {
		descriptor->free[i] = false; //�������� ������������, ���������� ������������ ������������
	}
	strcpy(descriptor->name[0], "����������");
	//������ ������������ ����������� � ������ �����
	memcpy(disk, descriptor, (BLOCK_SIZE * (limit + 1)));
	return 0;
}
/*--------------------------------------------------------------------------------*/

//��������� ��� �������� ��������� �������� � ����� �����������.
int edit_descriptor(int free_index, bool free, int name_index, char* name) {

	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	//�������� ���������� �� ���� � descriptor_block
	memcpy(descriptor, disk, BLOCK_SIZE * 2);
	//������ ������ � ����� ����������� ����� ��������
	if (free_index > 0) {
		descriptor->free[free_index] = free;
		if (DEBUG)
			printf("\t\t[%s] ��������� ������� ����������� ������ ���������� ���� ������ [%d][%s]\n", __func__, free_index, free == true ? "��������" : "������������");
	}

	if (name_index > 0) {

		strcpy(descriptor->name[name_index], name);
		if (DEBUG) 
			printf("\t\t[%s] ������� ����� ����������� ������ ���������� ���� ������ [%d][%s]\n", __func__, name_index, name);
	}
	// ������ ������������ ����������� ������� � ������ �����
	memcpy(disk, descriptor, BLOCK_SIZE * 2);
	return 0;
}
/*--------------------------------------------------------------------------------*/

//�������� ��� ����� � �����������; ������������ ��� ����������� ������;
int edit_descriptor_name(int index, char* new_name) {

	descriptor_block* descriptor = malloc(BLOCK_SIZE * 2);
	memcpy(descriptor, disk, BLOCK_SIZE * 2);
	// �������� ��� ����� � ������� �� new_name
	strcpy(descriptor->name[index], new_name);
	memcpy(disk, descriptor, BLOCK_SIZE * 2);
	free(descriptor);
	return 0;
}
/*--------------------------------------------------------------------------------*/

//��������� ��� �������� ����� �� ����.
int add_directory(char* name) {

	if (strcmp(name, "") == 0) {
		if (DEBUG) 
			printf("\t\t[%s] ������������ �������\n", __func__);
		return -1;
	}

	//��������� ������ ��� ����� �����
	dir_type* folder = malloc(BLOCK_SIZE);

	if (DEBUG) 
		printf("\t\t[%s] ��������� ����� ��� ����� �����\n", __func__);

	//�������������� ����� �����
	strcpy(folder->name, name);
	strcpy(folder->top_level, current.directory);
	folder->subitem = malloc(sizeof * (folder->subitem) * MAX_SUBDIRECTORIES);
	folder->subitem_count = 0; // Imp : �������������� ������ ������������ ���, ����� �� �������� 0 ���������
	//������� ��������� ���� �� ����� ��� �������� �����; true => �������� ���� ��� �������
	int index = allocate_block(name, true);

	if (DEBUG) 
		printf("\t\t[%s] ���������� ����� ����� ����� ������ [%d]\n", __func__, index);

	//��������� ����� �� ����
	memcpy(disk + index * BLOCK_SIZE, folder, BLOCK_SIZE);

	if (DEBUG) 
		printf("\t\t[%s] Folder [%s] ������� ��������\n", __func__, name);

	free(folder);
	return 0;
}
/*--------------------------------------------------------------------------------*/

//��������� ������� �������-����� � �����.
int remove_directory(char* name) {
	dir_type* folder = malloc(BLOCK_SIZE);
	int block_index = find_block(name, true);
	//���� ���������� �� ��� ������, �� return -1

	if (block_index == -1) {
		if (DEBUG) printf("\t\t[%s] ������� [%s] �� ���������� � ������� ����� [%s]\n", __func__, name, current.directory);
		return -1;
	}

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);

	//������ ��� ���, ���� ���� ���������� ==> ��� ����������� � Unix
	for (int i = 0; i < folder->subitem_count; i++) {
		if (folder->subitem_type[i] == true) {
			//����������� ����� �������, ����� ������� ��������
			remove_directory(folder->subitem[i]);
		}
		else {
			//������� ��������, ������� �������� ������
			remove_file(folder->subitem[i]);
		}
	}
	unallocate_block(block_index);
	free(folder);
	return 0;
}
/*--------------------------------------------------------------------------------*/

//��������� �������� ��������� �������� � ������ ������������ folders ��� �������� �������� �����
int edit_directory(char* name, char* subitem_name, char* new_name, bool name_change, bool directory) {

	if (strcmp(name, "") == 0) {
		if (DEBUG) printf("\t\t[%s] ������������ �������\n", __func__);
		return -1;
	}

	dir_type* folder = malloc(BLOCK_SIZE);
	//�������, ��� ��������� ����� �� �����
	int block_index = find_block(name, true);
	//���� ������� �� ������, return

	if (block_index == -1) {
		if (DEBUG) 
			printf("\t\t[%s] ������� [%s] �� ����������\n", __func__, name);
		return -1;
	}

	if (DEBUG) 
		printf("\t\t[%s] ����� [%s] ������� � ����� ������ [%d]\n", __func__, name, block_index);

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);

	if (strcmp(subitem_name, "") != 0) { //������, ����� �� ��������� �������� � ���� �����������
		if (!name_change) { //���������� ���������
			if (DEBUG) 
				printf("\t\t[%s] �������� �������� [%s] �� ������� ��������� [%d] � ������� [%s]\n", __func__, subitem_name, folder->subitem_count, folder->name);

			strcpy(folder->subitem[folder->subitem_count], subitem_name);
			folder->subitem_type[folder->subitem_count] = directory;
			folder->subitem_count++;

			if (DEBUG) 
				printf("\t\t[%s] � ����� [%s] ������ ���� [%d] ���������\n", __func__, name, folder->subitem_count);
			//��������� � ���� ����!
			memcpy(disk + block_index * BLOCK_SIZE, folder, BLOCK_SIZE);
			free(folder);
			return 0;
		}
		else { //�������������� �������� ���������
			for (int i = 0; i < folder->subitem_count; i++) {
				if (strcmp(folder->subitem[i], subitem_name) == 0) {
					strcpy(folder->subitem[i], new_name);

					if (DEBUG) 
						printf("\t\t[%s] ����������������� �������� [%s] � [%s] �� ������� ��������� [%d] ��� �������� [%s]\n", __func__, subitem_name, new_name, i, folder->name);

					memcpy(disk + block_index * BLOCK_SIZE, folder, BLOCK_SIZE);
					free(folder);
					return 0;
				}
			}
			if (DEBUG) 
				printf("\t\t[%s] �������� �� ���������� � �������� [%s]\n", __func__, folder->name);
			free(folder);
			return -1;
		}
	}
	else { //������ �������� �����

		//���� ������� � �������� ������ ��� ����������, �� ���������
		int block_index2 = find_block(new_name, true);
		//���� ������� ��� ������ ����� ��� ���������� return -1

		if (block_index2 != -1) {
			if (DEBUG) printf("\t\t[%s] ������� [%s] ��� ����������. �������� ������ ���\n", __func__, new_name);
			return -1;
		}

		strcpy(folder->name, new_name);

		if (DEBUG) 
			printf("\t\t[%s] ����� [%s] ������ ���������� [%s]\n", __func__, name, folder->name);

		memcpy(disk + block_index * BLOCK_SIZE, folder, BLOCK_SIZE);
		//����������� �����������
		edit_descriptor(-1, false, block_index, new_name);

		if (DEBUG) 
			printf("\t\t[%s] ����������� ������� ����� �����������\n", __func__);
		if (DEBUG) 
			print_directory(folder->name);

		//��������� ������������� �����
		edit_directory(folder->top_level, name, new_name, true, true);

		if (DEBUG) 
			printf("\t\t[%s] ��������� �������� ������������� ���������\n", __func__);

		int child_index;
		//��������� ��������, ����� �������� �������� �������� ������ ����������
		
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

//��������� ��� �������� ���� �� ��� ����; ��� ������� ������� ���� ���� ��������� ����������� (�������� ���������� � �����), � ����� ����� ������
int add_file(char* name, int size) {

	char subname[20];

	if (size < 0 || strcmp(name, "") == 0) {

		if (DEBUG) 
			printf("\t\t[%s] ������������ �������\n", __func__);
		if (!DEBUG) 
			printf("%s: ������������� �������\n", "mkfile");

		return 1;
	}

	//�������� ������ ��� file_type
	file_type* file = malloc(BLOCK_SIZE);

	if (DEBUG) 
		printf("\t\t[%s] ��������� ����� ��� ������ �����\n", __func__);

	//�������������� ��� �������� ������ ������ �����
	strcpy(file->name, name);
	strcpy(file->top_level, current.directory);

	file->size = size;
	file->data_block_count = 0;

	if (DEBUG) 
		printf("\t\t[%s] ������������� ��������� �����\n", __func__);

	//������� ��������� ����, ����� ��������� ���� ���� ��������� ����������� � ������, false ==> ��������� �� ����
	int index = allocate_block(name, false);

	//������� ��������� ����� ��� ���������� ������ �����
	if (DEBUG) 
		printf("\t\t[%s] �������������� [%d] ����� ������ � ������ ��� �������� ������\n", __func__, (int)size / BLOCK_SIZE);

	for (int i = 0; i < size / BLOCK_SIZE + 1; i++) {
		sprintf(subname, "%s->%d", name, i);
		file->data_block_index[i] = allocate_block(subname, false);
		file->data_block_count++;
	}

	//����� ������ � ������, �� ������������� �� ����
	memcpy(disk + index * BLOCK_SIZE, file, BLOCK_SIZE);

	if (DEBUG) 
		printf("\t\t[%s] ���� [%s] ������� ��������\n", __func__, name);

	free(file);
	return 0;
}

/*--------------------------------------------------------------------------------*/
int remove_file(char* name) {

	if (strcmp(name, "") == 0) {

		if (DEBUG) 
			printf("\t\t[%s] ������������ �������\n", __func__);
		if (!DEBUG) 
			printf("%s: ������������� �������\n", "rmfil");

		return 1;
	}

	file_type* file = malloc(BLOCK_SIZE);
	dir_type* folder = malloc(BLOCK_SIZE);
	int file_index = find_block(name, false);

	// ���� ���� �� ������, error, return -1
	if (file_index == -1) {
		if (DEBUG) printf("\t\t\t[%s] ���� [%s] �� ������\n", __func__, name);
		return -1;
	}

	if (DEBUG) 
		printf("\t\t[%s] ���� [%s] ������ � ����� ������ [%d]\n", __func__, name, file_index);

	memcpy(file, disk + file_index * BLOCK_SIZE, BLOCK_SIZE);

	//������� ����� top_level �� �����
	int folder_index = find_block(file->top_level, true);

	if (DEBUG) 
		printf("\t\t[%s] ����� [%s] ������� � ����� ������ [%d]\n", __func__, name, folder_index);

	memcpy(folder, disk + folder_index * BLOCK_SIZE, BLOCK_SIZE);

	// ���������� �� ������� ������������ ������������� �������� � ������� ��� ����
	char subitem_name[MAX_STRING_LENGTH];
	const int subcnt = folder->subitem_count; // ���������� ����������
	int j;
	int k = 0;

	for (j = 0; j < subcnt; j++) {
		strcpy(subitem_name, folder->subitem[j]);
		if (strcmp(subitem_name, name) != 0)
			// ���� ���� ������� �� ���, ������� �� �������, ��������� �������
		{
			strcpy(folder->subitem[k], subitem_name);
			k++;
		}
	}

	strcpy(folder->subitem[k], "");
	folder->subitem_count--;
	memcpy(disk + folder_index * BLOCK_SIZE, folder, BLOCK_SIZE); // ��������� ����� � ������

	//Imp : �������� ��������� ���� ������ ������ �� �����, ������� �� �������
	int i = 0;
	while (file->data_block_count != 0) {
		unallocate_block(file->data_block_index[i]);
		file->data_block_count--;
		i++;
	}

	unallocate_block(file_index); // ���������� ���� ���������� ������
	free(folder);
	free(file);
	return 0;
}
/*--------------------------------------------------------------------------------*/

//��������� ��� �������� ������������� ���� � �������� ��� ������ ��� ��������
int edit_file(char* name, int size, char* new_name) {

	file_type* file = malloc(BLOCK_SIZE);
	//������� ���� � ������, � ������� ������� ���� ����
	int block_index = find_block(name, false);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] ���� [%s] �� ������\n", __func__, name);

		return -1;
	}

	if (DEBUG) 
		printf("\t\t[%s] ���� [%s] ������ � ����� ������ [%d]\n", __func__, name, block_index);

	memcpy(file, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);

	if (size > 0) {
		//���� ������ ������ ����, �� ������ ������ ����� ��������
		file->size = size;

		if (DEBUG) 
			printf("\t\t[%s] ������ ����� [%s] [%d]\n", __func__, name, size);

		free(file);
		return 0;
	}
	else {
		//� ��������� ������ ��� ����� ����� ���������
		char top_level[MAX_STRING_LENGTH];
		strcpy(top_level, get_file_top_level(name));
		// �������� �������� ��������� ��������
		edit_directory_subitem(top_level, name, new_name);
		// �������� ��� ������������ ��������� �����������
		edit_descriptor_name(block_index, new_name);
		strcpy(file->name, new_name);
		memcpy(disk + block_index * BLOCK_SIZE, file, BLOCK_SIZE);

		if (DEBUG) 
			printf("\t\t\t[%s] ���� [%s] ������ ���������� [%s]\n", __func__, name, file->name);

		free(file);
		return 0;
	}
}
/*--------------------------------------------------------------------------------*/


/************************** Getter functions ************************************/

char* get_directory_name(char* name) {

	dir_type* folder = malloc(BLOCK_SIZE);
	char* tmp = malloc(sizeof(char) * MAX_STRING_LENGTH);

	//�������� True �������� ������� find, ��� �� ���� ��� ��������, � �� ��� �����
	int block_index = find_block(name, true);

	if (block_index == -1) {

		if (DEBUG)
			printf("\t\t\t[%s] ����� [%s] �� �������\n", __func__, name);

		strcpy(tmp, "");
		return tmp;
	}
	
	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	strcpy(tmp, folder->name);

	if (DEBUG) 
		printf("\t\t\t[%s] ��� [%s] ������� ��� [%s] �����\n", __func__, tmp, name);

	free(folder);
	return tmp;
}
/*--------------------------------------------------------------------------------*/

char* get_directory_top_level(char* name) {

	dir_type* folder = malloc(BLOCK_SIZE);
	char* tmp = malloc(sizeof(char) * MAX_STRING_LENGTH);

	//true ==> ��������� �����, � �� ����
	int block_index = find_block(name, true);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] ����� [%s] �� �������\n", __func__, name);

		strcpy(tmp, "");
		return tmp;
	}

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	strcpy(tmp, folder->top_level);

	if (DEBUG) 
		printf("\t\t\t[%s] top_level [%s] ������ ��� [%s] �����\n", __func__, tmp, name);

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
			printf("\t\t\t[%s] ����� [%s] �� �������\n", __func__, name);

		strcpy(tmp, "");
		return tmp;
	}

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);

	if (subitem_index >= 0) {
		//������ �������� ���������
		strcpy(tmp, folder->subitem[subitem_index]);

		if (DEBUG) 
			printf("\t\t\t[%s] ��������[%d] = [%s] ��� [%s] �����\n", __func__, subitem_index, tmp, name);

		free(folder);
		return tmp;
	}
	else {
		//���� ��������
		for (int i = 0; i < folder->subitem_count; i++) {
			if (strcmp(folder->subitem[i], subitem_name) == 0) {

				if (DEBUG) 
					printf("\t\t\t[%s] ������ [%s] � �������� ��������� �������� [%s]\n", __func__, subitem_name, name);

				return "0";
			}
		}

		if (DEBUG) 
			printf("\t\t\t[%s] �� ������ [%s] � �������� ��������� �������� [%s]\n", __func__, subitem_name, name);

		free(folder);
		return "-1";
	}
	free(folder);
	return tmp;
}
/*--------------------------------------------------------------------------------*/

int edit_directory_subitem(char* name, char* sub_name, char* new_sub_name) {

	dir_type* folder = malloc(BLOCK_SIZE);
	//�������� True �������� ������� find, ��� �� ���� ��� ��������, � �� ��� �����
	int block_index = find_block(name, true);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] ����� [%s] �� �������\n", __func__, name);

	}

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	const int cnt = folder->subitem_count;
	int i;

	for (i = 0; i < cnt; i++) {
		if (strcmp(folder->subitem[i], sub_name) == 0) {
			strcpy(folder->subitem[i], new_sub_name);

			if (DEBUG) 
				printf("\t\t\t[%s] ����������������� �������� � %s �� %s �� %s\n", __func__, folder->name, sub_name, folder->subitem[i]);

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
			printf("\t\t\t[%s] ����� [%s] �� �������\n", __func__, name);

		return -1;
	}

	memcpy(folder, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	tmp = folder->subitem_count;

	if (DEBUG) 
		printf("\t\t\t[%s] ���������� ������������ [%d] ������� ��� ����� [%s]\n", __func__, folder->subitem_count, name);

	free(folder);
	return tmp;
}
/*--------------------------------------------------------------------------------*/

char* get_file_name(char* name) {

	file_type* file = malloc(BLOCK_SIZE);
	char* tmp = malloc(sizeof(char) * MAX_STRING_LENGTH);
	//false ==> ��������� �� ����, � �� �� �����
	int block_index = find_block(name, false);

	if (block_index == -1) {

		if (DEBUG) 
			printf("\t\t\t[%s] ���� [%s] �� ������\n", __func__, name);

		strcpy(tmp, "");
		return tmp;
	}

	memcpy(file, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	strcpy(tmp, file->name);

	if (DEBUG) 
		printf("\t\t\t[%s] ������� ��� [%s] ��� ����� [%s]\n", __func__, tmp, name);

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
			printf("\t\t\t[%s] ���� [%s] �� ������\n", __func__, name);

		strcpy(tmp, "");
		return tmp;
	}

	memcpy(file, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	strcpy(tmp, file->top_level);

	if (DEBUG) 
		printf("\t\t\t[%s] top_level [%s] ������� ��� ����� [%s]\n", __func__, tmp, name);

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
			printf("\t\t\t[%s] ���� [%s] �� ������\n", __func__, name);

		return -1;
	}

	memcpy(file, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);
	tmp = file->size;

	if (DEBUG) 
		printf("\t\t\t[%s] ������ [%d], ��������� ��� ����� [%s]\n", __func__, tmp, name);

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
	printf(" �������� ����� �����:\n\n\t��� = %s\n\ttop_level = %s\n\t��������� = ", folder->name, folder->top_level);

	for (int i = 0; i < folder->subitem_count; i++) {
		printf("%s ", folder->subitem[i]);
	}

	printf("\n\t���������� ���������� = %d\n", folder->subitem_count);
	printf(" -----------------------------\n");
	free(folder);
}

void print_file(char* name) {

	file_type* file = malloc(BLOCK_SIZE);
	int block_index = find_block(name, false);
	memcpy(file, disk + block_index * BLOCK_SIZE, BLOCK_SIZE);

	printf(" -----------------------------\n");
	printf(" ����� �������� �����:\n\n\t��� = %s\n\ttop_level = %s\n\t������ ����� = %d\n\tblock count = %d\n", file->name, file->top_level, file->size, file->data_block_count);
	printf(" -----------------------------\n");
	free(file);
}
/*--------------------------------------------------------------------------------*/