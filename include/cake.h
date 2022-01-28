#ifndef _CAKE_H
#define _CAKE_H

#define CK_FN uint8_t
#define EX_S (0)
#define EX_F (255)

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "MEL_logs.h"
#include "khash.h"

/* CAKE SYNTAX */
#define CK_LERR_KW "err"
#define CK_LWRN_KW "warn"
#define CK_LINF_KW "info"

#define CK_PRNT_KW "print"
#define CK_CALL_KW "use"
/* * * * * * * */

// shorthand way to get the key from hashtable or defVal if not found
#define kh_get_val(kname, hash, key, defVal) ({k=kh_get(kname, hash, key);(k!=kh_end(hash)?kh_val(hash,k):defVal);})

// shorthand way to set value in hash with single line command.  Returns value
// returns 0=replaced existing item, 1=bucket empty (new key), 2-adding element previously deleted
#define kh_set(kname, hash, key, val) ({int ret; k = kh_put(kname, hash,key,&ret); kh_value(hash,k) = val; ret;})

#define CK_USAGE(){\
    MEL_log(LOG_INFORMATION, "CAKE USAGE:\n"\
                             "\t./cake\n"\
                             "\t\t-i <filename>\t=>\tinterpret script\n"\
                             "\t\t-d           \t=>\tenable debugging\n");\
}

static bool debug = false;

inline static bool get_debug(){
    return debug;
}

inline static void set_debug(bool d){
    debug = d;
}

CK_FN interpret_file(const char * filename);
CK_FN lex_string(const char * filename, const char * fstr, const size_t sz, char * _fstr);
CK_FN parse_string(const char * curr_file, char * _fstr, const size_t sz);
CK_FN parse_file(const char * filename);

void CK_replace_str(char *target, const char *needle, const char *replacement);
size_t CK_strc(char * str, int c, size_t index);
char * CK_substr(char * src, int offset, int len);
size_t CK_get_file_size(const char * f_name);
char * CK_read_file(const char * f_name, size_t * f_size);

#endif
