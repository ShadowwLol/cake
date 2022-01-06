#include "../include/cake.h"

char * CK_substr(char * src, int offset, int len){
    char * out = src+offset;
    out[len] = '\0';

    return out;
}

size_t CK_get_file_size(const char * f_name){
    size_t length;
    FILE * f = fopen(f_name, "rb");
    if (f){
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);

        /* 1 GiB; best not to load a whole large file in one string */
        if (length > 1073741824) {
            return EX_F;
        }
    }else{
        return EX_F;
    }
    fclose(f);
    return length;
}

char * CK_read_file(const char * f_name, int * err, size_t * f_size) {
    char * buffer;
    size_t length;
    FILE * f = fopen(f_name, "rb");
    size_t read_length;

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);

        /* 1 GiB; best not to load a whole large file in one string */
        if (length > 1073741824) {
            *err = FILE_TOO_LARGE;

            return NULL;
        }

        buffer = (char *)malloc(length + 1);

        if (length) {
            read_length = fread(buffer, 1, length, f);

            if (length != read_length) {
                 free(buffer);
                 *err = FILE_READ_ERROR;

                 return NULL;
            }
        }

        fclose(f);

        *err = FILE_OK;
        buffer[length] = '\0';
        *f_size = length;
    }
    else {
        *err = FILE_NOT_EXIST;

        return NULL;
    }

    return buffer;
}
