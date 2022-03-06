#ifndef _MEL_IO_H
#define _MEL_IO_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "MEL_logs.h"

#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#if __WIN32
#define mkdir(A, B) mkdir(A)
#endif

#define FN uint8_t
#define EX_S (0)
#define EX_F (255)

typedef struct {
	char * buffer;
	size_t length;
	size_t cap;
} String;

typedef struct{
	char * buffer;
	size_t length;
} String_View;

/* char * operations */
static size_t MEL_strlen(const char *);
static char * MEL_strlcpy(char *, const char *, size_t);
static char * MEL_strlcat(char *, const char *, size_t);
static int32_t MEL_strcmp(const char *, const char *);
static char * MEL_memcpy(char * dest, const char * src, size_t n);
/* * * * * * * * * * */

/* String operations */
static String init_str();
static FN realloc_str(String *, uint64_t);
static FN set_str(String *, const char *);
static FN append_str(String *, const char *);
static FN remove_str(String *);
/* * * * * * * * * * */

/* String_View operations  */
static String_View set_strv(char *);
static String_View substr_to_strv(char *, size_t, size_t);
static String MEL_read_file(const char *);
/* * * * * * * * * * * * * */

/* Return length of `src` string */
inline static size_t MEL_strlen(const char * src){
	size_t i = 0;
	for (; src[i] != '\0'; ++i){}
	return i;
}

/* Copy string `src` to buffer `dest` with length `n` */
inline static char * MEL_strlcpy(char * dest, const char * src, size_t n){
	size_t i;
	for (i = 0; i < n && src[i] != '\0'; ++i){
		dest[i] = src[i];
	}
	for (; i < n; ++i){
		dest[i] = '\0';
	}
	dest[n-1] = '\0';
	return dest;
}

/* Append string `src` to buffer `dest` with length`n` */
inline static char * MEL_strlcat(char * dest, const char * src, size_t n){
	size_t dest_len = MEL_strlen(dest);
	size_t src_len = MEL_strlen(src);
	for (size_t i = 0; i < n && src[i] != '\0'; ++i){
		dest[dest_len+i] = src[i];
	}
	dest[dest_len+src_len] = '\0';
	dest[n-1] = '\0';
	return dest;
}

/* Compare two strings and return their difference */
inline static int32_t MEL_strcmp(const char * p1, const char * p2){
	const unsigned char * s1 = (const unsigned char *) p1;
	const unsigned char * s2 = (const unsigned char *) p2;
	unsigned char c1, c2;
	do{
			c1 = (unsigned char) *++s1;
			c2 = (unsigned char) *++s2;
			if (c1 == '\0')
				break;
	}while (c1 == c2);
	return c1 - c2;
}

/* Copy string `src` to buffer `dest` with length `n` */
inline static char * MEL_memcpy(char * dest, const char * src, size_t n){
	size_t i;
	for (i = 0; i < n; ++i){
		dest[i] = src[i];
	}
	dest[n-1] = '\0';
	return dest;
}

/* Initialize a String */
inline static String init_str(){
	String str = {
	.buffer = (char *)calloc(1, sizeof(char)*1),
	.length = 0,
	.cap = sizeof(char) * 1,
	};
	return str;
}

/* Reallocate a String */
inline static FN realloc_str(String * s, uint64_t bytes){
	s->buffer = (char *)realloc(s->buffer, sizeof(char) * bytes);
	s->cap = sizeof(char) * bytes;
	return 0;
}

/* Set String `str`s buffer to `src` and update String */
inline static FN set_str(String * str, const char * src){
	size_t src_len = MEL_strlen(src);
	str->length = src_len;
	if (str->cap < str->length){
		str->cap = str->length+1;
		str->buffer = (char*)realloc(str->buffer, sizeof(char) * str->cap);
	}
	MEL_strlcpy(str->buffer, src, str->cap);
	return 0;
}

/* Append String `str`s buffer with `src` and update String */
inline static FN append_str(String * str, const char * src){
	size_t src_len = MEL_strlen(src);
	str->length += src_len;
	if (str->cap < str->length) {
		str->cap = str->length+1;
		str->buffer = (char *)realloc(str->buffer, sizeof(char) * str->cap);
	}
	MEL_strlcat(str->buffer, src, str->cap);
	return 0;
}

/* Remove String */
inline static FN remove_str(String * str){
	free(str->buffer);
	str->length = 0;
	str->cap = 0;
	return 0;
}

/* Set String_View `src`s buffer and update String_View */
inline static String_View set_strv(char * src){
	String_View sv = {
		.buffer = src,
		.length = MEL_strlen(src)
	};
	return sv;
}

/* Return substring of `src`s buffer with offset `offset` and length `len` */
inline static String_View substr_to_strv(char * src, size_t offset, size_t len){
	size_t src_len = MEL_strlen(src);
	if (src_len < (offset+len)){
		return (String_View){NULL, 0};
	}
	String_View sv = {
		.buffer = (src+offset),
		.length = len,
	};
	sv.buffer[len] = '\0';
	return sv;
}

/* Return String with contents of file `f_name` */
inline static String MEL_read_file(const char * f_name) {
	String str = init_str();
	FILE * f = fopen(f_name, "rb");
	size_t read_length;

	if (f) {
		fseek(f, 0, SEEK_END);
		str.length = ftell(f);
		fseek(f, 0, SEEK_SET);

		/* 1 GiB; best not to load a whole large file in one string */
		if (str.length > 1073741824) {
			remove_str(&str);
			MEL_log(LOG_WARNING, "File {%s} too large", f_name);
			return (String){NULL};
		}

		realloc_str(&str, str.length+1);

		if (str.length) {
			read_length = fread(str.buffer, 1, str.length, f);

			if (str.length != read_length) {
				remove_str(&str);
				MEL_log(LOG_WARNING, "Failed reading file {%s}", f_name);
				return (String){NULL};
			}
		}

		fclose(f);

		str.buffer[str.length] = '\0';
	}
	else {
		remove_str(&str);
		MEL_log(LOG_WARNING, "File does not exist {%s}", f_name);
		return (String){NULL};
	}

	return str;
}

static FN mkpath(char* file_path, mode_t mode);
inline static FN mkpath(char* file_path, mode_t mode){
	if (!file_path || !*file_path){
		return EX_F;
	}

	for (char* p = strchr(file_path + 1, '/'); p; p = strchr(p + 1, '/')) {
		*p = '\0';
		if (mkdir(file_path, mode) == -1) {
			if (errno != EEXIST) {
				*p = '/';
				MEL_log(LOG_ERROR, "Failed creating path");
				return EX_F;
			}
		}
		*p = '/';
	}
	return EX_S;
}

static FN get_input(char * str, size_t sz, FILE * fd);
inline static FN get_input(char * str, size_t sz, FILE * fd){
	if (!fgets(str, sz, fd)){
		MEL_log(LOG_ERROR, "Failed getting input");
		return EX_F;
	}
	str[MEL_strlen(str)-1] = '\0';
	return EX_S;
}

#endif
