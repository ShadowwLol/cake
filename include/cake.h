#ifndef _CAKE_H
#define _CAKE_H

#define CK_FN unsigned char
#define EX_S (0)
#define EX_F (255)

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "../include/MEL_logs.h"

#define CK_USAGE(){\
    MEL_log(LOG_INFORMATION, "CAKE USAGE:\n"\
                             "./cake -i <filename>");\
}

static bool debug = false;

inline static bool get_debug(){
    return debug;
}

inline static void set_debug(bool d){
    debug = d;
}

#define FILE_OK 0
#define FILE_NOT_EXIST 1
#define FILE_TOO_LARGE 2
#define FILE_READ_ERROR 3

CK_FN interpret_file(const char * filename);
CK_FN lex_string(const char * fstr, const size_t sz, char * _fstr);
CK_FN parse_string(const char * curr_file, char * _fstr, const size_t sz);
CK_FN parse_file(const char * filename);

size_t CK_strc(char * str, int c, size_t index);
char * CK_substr(char * src, int offset, int len);
size_t CK_get_file_size(const char * f_name);
char * CK_read_file(const char * f_name, size_t * f_size);

#endif
