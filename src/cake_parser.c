#include "../include/cake.h"

size_t get_line_count(const char * _fstr, const size_t sz){
    size_t line_count = 0;

    size_t i = 0;
    for (; i < sz; ++i){
        if (_fstr[i] == ';'){
            ++line_count;
        }
    }

    return line_count;
}

char * get_line(const char * _fstr, const size_t sz, const size_t index){
    if (index > get_line_count(_fstr, sz)){
        return NULL;
    };

    char * line;
    size_t lc = 0;

    size_t i = 0;
    for (; i < sz; ++i){
        if (_fstr[i] == ';'){
            if (lc+1 == index){
                line = (char *)(_fstr+i);
            }else if (lc == index){
                line[i] = '\0';
                break;
            }
            ++lc;
        }
    }

    //char * line = (char *)_fstr;
    //line[i] = '\0';

    return line;
}

CK_FN parse_string(const char * curr_file, char * _fstr, const size_t sz){
    for (size_t i = 0; i <= sz && _fstr[0] != '\0'; ++i, ++_fstr){
        if (_fstr[0] == ';'){
            //printf("%s\n", _fstr);
            ++_fstr;
        }

        if (0 == strncmp("print(\"", _fstr, 7)){
            i+=7;
            _fstr+=7;

            //printf("\n#####\nline:\"%s\"\n#####\n", _fstr);

            while (_fstr[0] != '\"' && _fstr[0] != '\0'){
                if (_fstr[0] == '\\'){
                    ++i;
                    ++_fstr;
                    switch (_fstr[0]){
                        case 'n':
                            putchar('\n');
                            break;
                        case 'r':
                            putchar('\r');
                            break;
                        case 't':
                            putchar('\t');
                            break;
                        case '\\':
                            putchar('\\');
                            break;
                        default:
                            if (isalnum(_fstr[0])){
                                putchar(_fstr[0]);
                            }else{
                                printf("\n%s\n", _fstr);
                                printf("%0*c^\n", i, ' ');
                                MEL_log(LOG_ERROR, "Invalid escape sequence");
                                return EX_F;
                            }
                    }
                }else{
                    putchar(_fstr[0]);
                }
                ++i;
                ++_fstr;
            }
            //printf("printing\n");
        }else if (0 == strncmp("use(\"", _fstr, 5)){
            i+=5;
            _fstr+=5;

            //printf("\n#####\nline:\"%s\"\n#####\n", _fstr);

            char filename[sz];
            size_t o = 0;
            while (_fstr[0] != '\"' && _fstr[0] != '\0'){
                filename[o] = _fstr[0];
                ++o;
                ++i;
                ++_fstr;
            }

            //printf("FILENAME: \"%s\";\n", filename);
            if (0 == strcmp(filename, curr_file)){
                MEL_log(LOG_WARNING, "Unsafe recursive loop through file");
                return EX_F;
            }
            interpret_file(filename);

            //printf("printing\n");
        }
    }

#if 0
    //printf("\n\n%s", get_line(_fstr, sz));

    size_t line_count = get_line_count(_fstr, sz);

    for (size_t i = 0; i < line_count; ++i){
        char * line = get_line(_fstr, sz, i);
        printf("%s\n", line);
    }
    char line[sz];
    for (size_t i = 0; i < sz; ++i){
        printf("%s\n", CK_substr((char *)_fstr, i, 7));
        if (strncmp("print(\"", CK_substr((char *)_fstr, i, 7), 7) == 0){
            i += 7;
            while (_fstr[i] != '\"' && _fstr[i] != '\0'){
                putchar(_fstr[i]);
                ++i;
            }
        }
    }
#endif
}
