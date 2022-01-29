#include "../include/cake.h"

CK_FN lex_string(const char * filename, const char * fstr, const size_t sz, char * _fstr){
    //char _fstr[sz];
    size_t j = 0;
    bool commenting = false;
    bool inside_quotes = false;

    /* LEXING */
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
                if (!isspace(fstr[i]) && fstr[i] != 0){
                    _fstr[j] = fstr[i];
                    ++j;
                }
            }else{
                _fstr[j] = fstr[i];
                ++j;
            }
        }
    }
    _fstr[j] = '\0';
    CK_replace_str(_fstr, "_here()", filename);
    /* * * * * */

    return EX_S;
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
