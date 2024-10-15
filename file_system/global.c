#ifndef GLOBALS_H
#define GLOBALS_H 1

// #include "globals.h"
#include <stdbool.h>
#include "types.h"

bool disk_allocated = false; // гарантирует, что do_root вызывается первым делом и вызывается только один раз
char* disk;
working_directory current;
#endif // GLOBALS_H