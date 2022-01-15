#ifndef _CAKE_H
#define _CAKE_H

#define CK_FN unsigned char
#define EX_S (0)
#define EX_F (255)

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../include/MEL_logs.h"

#define FILE_OK 0
#define FILE_NOT_EXIST 1
#define FILE_TOO_LARGE 2
#define FILE_READ_ERROR 3

CK_FN parse_file(const char * filename);

char * CK_substr(char * src, int offset, int len);
size_t CK_get_file_size(const char * f_name);
char * CK_read_file(const char * f_name, int * err, size_t * f_size);

#endif
