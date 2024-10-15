#ifndef CORE_H
#define CORE_H 1
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../types.h"
#include "../defines.h"

extern bool disk_allocated; // гарантирует, что do_root вызывается первым делом и вызывается только один раз
extern char* disk;
extern working_directory current;
extern struct action table[];

int initFileSystem();
int allocate_block(char* name, bool directory);
int find_block(char* name, bool directory);
int add_descriptor(char* name);
int edit_descriptor(int free_index, bool free, int name_index, char* name);
int edit_descriptor_name(int index, char* new_name);
int add_directory(char* name);
int remove_directory(char* name);
int rename_directory(char* name, char* new_name);
int edit_directory(char* name, char* subitem_name, char* new_name, bool name_change, bool directory);
int add_file(char* name, int size);
int edit_file(char* name, int size, char* new_name);
int remove_file(char* name);
int edit_directory_subitem(char* name, char* sub_name, char* new_sub_name);
int get_directory_subitem_count(char* name);
int get_file_size(char* name);

char* get_directory_name(char* name);
char* get_directory_top_level(char* name);
char* get_directory_subitem(char* name, int subitem_index, char* subitem);
char* get_file_name(char* name);
char* get_file_top_level(char* name);

void print_file(char* name);
void printing(char* name, int deep);
void print_descriptor();
void parse(char* buf, int* argc, char* argv[]);
void unallocate_block(int offset);
void print_directory(char* name);

#endif // CORE_H