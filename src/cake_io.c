#include "../include/cake.h"

void CK_replace_str(char *target, const char *needle, const char *replacement){
    char buffer[1024] = { 0 };
    char *insert_point = &buffer[0];
    const char *tmp = target;
    size_t needle_len = strlen(needle);
    size_t repl_len = strlen(replacement);

    while (1) {
        const char *p = strstr(tmp, needle);

        // walked past last occurrence of needle; copy remaining part
        if (p == NULL) {
            strcpy(insert_point, tmp);
            break;
        }

        // copy part before needle
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        // copy replacement string
        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        // adjust pointers, move on
        tmp = p + needle_len;
    }

    // write altered string back to target
    strcpy(target, buffer);
}

size_t CK_strc(char * str, int c, size_t index){
    size_t o = 0;
    for (size_t i = 0; str[i] != '\0'; ++i){
        if (str[i] == c){
            if (o == index){
                return i;
            }
            ++o;
        }
    }
    return EX_F;
}

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

char * CK_read_file(const char * f_name, size_t * f_size) {
    char * buffer;
    size_t length;
    FILE * f = fopen(f_name, "r");
    size_t read_length;

    if (f) {
        fseek(f, 0, SEEK_END);
        length = ftell(f);
        fseek(f, 0, SEEK_SET);

        /* 1 GiB; best not to load a whole large file in one string */
        if (length > 1073741824) {
            MEL_log(LOG_ERROR, "File too large: {%s}", f_name);
            return NULL;
        }

        buffer = (char *)calloc(length + 1, sizeof(char));

        if (length) {
            read_length = fread(buffer, 1, length, f);

            if (length != read_length) {
                free(buffer);
                MEL_log(LOG_ERROR, "File read error: {%s}", f_name);
                return NULL;
            }
        }

        fclose(f);

        buffer[length] = '\0';
        *f_size = length;
    }
    else {
        MEL_log(LOG_ERROR, "File does not exist: {%s}", f_name);
        return NULL;
    }

    return buffer;
}
