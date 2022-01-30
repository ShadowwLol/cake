#include "../include/cake.h"

bool is_from_A_to_Z(const char c){
    switch(tolower(c)){
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
            return true;
        default:
            return false;
    }
}

size_t lex_string(const char * filename, char * fstr, const size_t sz){
    size_t j = 0;
    bool commenting = false;
    bool inside_quotes = false;

    /* Lexing */
    for (size_t i = 0; i < sz; ++i){
        if (fstr[i] == '/' && fstr[++i] == '/'){
            commenting = true;
        }
        if (fstr[i-1] == '\n' && commenting){
            commenting = false;
        }

        if (fstr[i] == '\"' && !commenting){
            inside_quotes = !inside_quotes;
        }

        if (!commenting){
            if (!inside_quotes){
                if (!isspace(fstr[i])){
                    fstr[j] = fstr[i];
                    ++j;
                }
            }else{
                fstr[j] = fstr[i];
                ++j;
            }
        }
    }

    for (size_t l = j; l < sz; ++l){
        fstr[l] = '\0';
    }
    ++j;
    CK_replace_str(fstr, "_here()", filename);
    /* * * * * */

    return j;
}

CK_FN parse_file(const char * filename){
    size_t f_sz;
    FILE * fp = fopen(filename, "rb");
    if (!fp){
        MEL_log(LOG_ERROR, "Failed reading file => {%s}", filename);
        return EX_F;
    }

    fseek(fp, 0, SEEK_END);
    f_sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    /* 1 GiB; best not to load a whole large file in one string */
    if (f_sz > 1073741824) {
        MEL_log(LOG_ERROR, "File too large => {%s}", filename);
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
            const char * file_to_parse = CK_substr(file, 5, i-5);
            if (0 == strcmp(file_to_parse, filename)){
                MEL_log(LOG_WARNING, "Recursively looping through itself => {%s}", filename);
            }
            parse_file(file_to_parse);
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
                                MEL_log(LOG_ERROR, "Invalid escape sequence => {%s}", filename);
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
