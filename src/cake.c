#include "../include/cake.h"
#include <ctype.h>

CK_FN parse_file(const char * filename){
    size_t f_sz;
    FILE * fp = fopen(filename, "rb");
    if (!fp){
        return EX_F;
    }

    fseek(fp, 0, SEEK_END);
    f_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    /* 1 GiB; best not to load a whole large file in one string */
    if (f_sz > 1073741824) {
        return EX_F;
    }

    char file[f_sz];
    while(fgets(file, f_sz, fp)) {
        if (0 == strncmp(file, "//", 2)){
            continue;
        }else if (0 == strncmp(file, "use(\"", 5)){
            size_t i = 5;
            while (file[i] != '\"' && file[i] != '\0'){
                ++i;
            }
            printf("acessing file => [%s]\n", CK_substr(file, 5, i-5));
            parse_file(CK_substr(file, 5, i-5));
        }else if (0 == strncmp(file, "print(\"", 7)){
            size_t i = 7;
            while (file[i] != '\"' && file[i] != '\0'){
                if (file[i] == '\\'){
                    ++i;
                    switch (file[i]){
                        case 'n':
                            putchar('\n');
                            break;
                        case '\\':
                            putchar('\\');
                            break;
                        default:
                            if (isalnum(file[i])){
                                putchar(file[i]);
                            }else{
                                printf("\n%s\n", file);
                                printf("%0*c^\n", i, ' ');
                                printf("ERROR: invalid escape sequence\n");
                                return EX_F;
                            }
                    }
                }else{
                    putchar(file[i]);
                }
                ++i;
            }
        }
    }
    fclose(fp);
    return EX_S;
}

int main(int argc, char * argv[]){
    if (argc < 2){
        return EX_F;
    }

    parse_file(argv[1]);

    return EX_S;
}
