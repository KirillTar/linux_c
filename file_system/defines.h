#ifndef DEFINES_H
#define DEFINES_H 1
/************************** Defining Constants for fs *******************/
#define DEBUG 1
#define LINESIZE 128
#define DISK_PARTITION 4000000
#define BLOCK_SIZE 5000
#define BLOCKS 4000000 / 5000
#define MAX_STRING_LENGTH 20
#define MAX_FILE_DATA_BLOCKS 3776// (BLOCK_SIZE-64*59)
#define MAX_SUBDIRECTORIES 3640//(BLOCK_SIZE-136)
#endif // DEFINES_H