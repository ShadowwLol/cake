#include "../include/cake.h"

/*
keywords:
    i32, _here(), print(...), use("..."), err("..."), warn("..."), info("...")
*/

#define advance_parser(str, c, n){\
    c += n;\
    str += n;\
}

typedef enum{
    CK_UINT8_T = 0, CK_UINT16_T, CK_UINT32_T, CK_UINT64_T,
    CK_INT8_T,      CK_INT16_T, CK_INT32_T,  CK_INT64_T,
    CK_STR_T,       CK_CHAR_T,
    CK_BOOL_T,
} CK_DATATYPES;

char * CK_DATATYPES_TO_STR(int8_t datatype){
    switch (datatype){
        case CK_UINT8_T:
            return "ui8";
        case CK_UINT16_T:
            return "ui16";
        case CK_UINT32_T:
            return "ui32";
        case CK_UINT64_T:
            return "ui64";

        case CK_INT8_T:
            return "i8";
        case CK_INT16_T:
            return "i16";
        case CK_INT32_T:
            return "i32";
        case CK_INT64_T:
            return "i64";

        case CK_STR_T:
            return "str";
        case CK_CHAR_T:
            return "char";
        case CK_BOOL_T:
            return "bool";
        default:
            return "NULL";
    }
}

bool is_int(const char * input){
    for (size_t i = 0; i < strlen(input); ++i){
        if (!isdigit(input[i])){ /* filters floats and strings */
            return false;
        }
    }
    return true;
}

bool is_intd(const char * input, uint8_t type){
    if (!is_int(input)){
        return false;
    }

    int64_t sz = atoll(input);
    switch (type){
        case CK_INT8_T:
            if (sz >= INT8_MIN && sz <= INT8_MAX){
                return true;
            }
            break;
        case CK_INT16_T:
            if (sz >= INT16_MIN && sz <= INT16_MAX){
                return true;
            }
            break;
        case CK_INT32_T:
            if (sz >= INT32_MIN && sz <= INT32_MAX){
                return true;
            }
            break;
        case CK_INT64_T:
            if (sz >= INT64_MIN && sz <= INT64_MAX){
                return true;
            }
            break;
        default:
            break;
    }

    return false;
}

size_t is_in_str(const char * str, const char * cmp){
    const size_t str_len = strlen(str);
    const size_t cmp_len = strlen(cmp);

    if (cmp_len > str_len){
        return 0;
    }

    char * tmp = (char *)str;
    for (size_t i = 0; i < str_len; ++i){
        if (0 == strncmp(tmp, cmp, cmp_len)){
            return i;
        }
        ++tmp;
    }
    return 0;
}

char * check_statement(char ** str, size_t * index, const char * statement){
    if (0 != strncmp(*str, statement, strlen(statement))){
        return NULL;
    }

    /* Using statement */
    advance_parser(*str, index, strlen(statement));

    char * next = *str;
    size_t i = 0;
    while (*str[0] != ';' && *str[0] != '\0'){
        ++i;
        advance_parser(*str, index, 1);
    }
    next[i] = '\0';

    return next;
}

char * check_function(char ** str, size_t * index, const char * fn){
    const size_t fn_len = strlen(fn);
    if (0 != strncmp(*str, fn, fn_len)){
        return NULL;
    }

    /* Using function */
    advance_parser(*str, index, fn_len);
    if (*str[0] != '('){
        return NULL;
    }
    advance_parser(*str, index, 1);

    char * args = *str;
    size_t i = 0;
    while (0 != strncmp(*str, ");", 2) && *str[0] != '\0'){
        advance_parser(*str, index, 1);
        ++i;
    }
    args[i] = '\0';
    return args;
}

/* Data Type hashtable */
const int khstri = 33;
KHASH_MAP_INIT_STR(khstri, int);
/* * * * * * * * * * * */

/* Value hashtable */
const int khstrs = 32;
KHASH_MAP_INIT_STR(khstrs, char*);
/* * * * * * * * * * * */

CK_FN parse_string(const char * curr_file, char * _fstr, const size_t sz){
   khiter_t k = 0;
   khash_t(khstri) * h_datatype = kh_init(khstri); // create a hashtable
   khash_t(khstrs) * h_value = kh_init(khstrs); // create a hashtable

    char * statement;
    char * args;
    for (size_t i = 0; i < sz && _fstr[0] != '\0';){
        if (_fstr[0] == ';' || _fstr[0] == ')'){
            advance_parser(_fstr, i, 1);
        }

        if ((statement = check_statement(&_fstr, &i, "i8"))){
            size_t eq = is_in_str(statement, "=");
            char * var = statement;

            if (eq){
                /* Assignment */
                char * val = statement+eq+1;
                var[eq] = '\0';

                k = kh_get(khstri, h_datatype, var);
                if (k == kh_end(h_datatype)){
                    /* Var not found */
                    if (!is_intd(val, CK_INT8_T)){
                        MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] assigned wrong type in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT8_T), curr_file);
                        return EX_F;
                    }
                    kh_set(khstri, h_datatype, var, CK_INT8_T);
                    kh_set(khstrs, h_value, var, val);
                } else {
                    /* Var found */
                    MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] already exists in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT8_T), curr_file);
                    return EX_F;
                }
            }else{
                k = kh_get(khstri, h_datatype, var);
                if (k == kh_end(h_datatype)){
                    /* Var not found */
                    kh_set(khstri, h_datatype, var, CK_INT8_T);
                    kh_set(khstrs, h_value, var, "0");
                } else {
                    /* Var found */
                    MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] already exists in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT8_T), curr_file);
                    return EX_F;
                }
            }

        }else if ((statement = check_statement(&_fstr, &i, "i16"))){
            size_t eq = is_in_str(statement, "=");
            char * var = statement;

            if (eq){
                /* Assignment */
                char * val = statement+eq+1;
                var[eq] = '\0';

                k = kh_get(khstri, h_datatype, var);
                if (k == kh_end(h_datatype)){
                    /* Var not found */
                    if (!is_intd(val, CK_INT16_T)){
                        MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] assigned wrong type in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT16_T), curr_file);
                        return EX_F;
                    }
                    kh_set(khstri, h_datatype, var, CK_INT16_T);
                    kh_set(khstrs, h_value, var, val);
                } else {
                    /* Var found */
                    MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] already exists in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT16_T), curr_file);
                    return EX_F;
                }
            }else{
                k = kh_get(khstri, h_datatype, var);
                if (k == kh_end(h_datatype)){
                    /* Var not found */
                    kh_set(khstri, h_datatype, var, CK_INT16_T);
                    kh_set(khstrs, h_value, var, "0");
                } else {
                    /* Var found */
                    MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] already exists in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT16_T), curr_file);
                    return EX_F;
                }
            }

        }else if ((statement = check_statement(&_fstr, &i, "i32"))){
            size_t eq = is_in_str(statement, "=");
            char * var = statement;

            if (eq){
                /* Assignment */
                char * val = statement+eq+1;
                var[eq] = '\0';

                k = kh_get(khstri, h_datatype, var);
                if (k == kh_end(h_datatype)){
                    /* Var not found */
                    if (!is_intd(val, CK_INT32_T)){
                        MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] assigned wrong type in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT32_T), curr_file);
                        return EX_F;
                    }
                    kh_set(khstri, h_datatype, var, CK_INT32_T);
                    kh_set(khstrs, h_value, var, val);
                } else {
                    /* Var found */
                    MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] already exists in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT32_T), curr_file);
                    return EX_F;
                }
            }else{
                k = kh_get(khstri, h_datatype, var);
                if (k == kh_end(h_datatype)){
                    /* Var not found */
                    kh_set(khstri, h_datatype, var, CK_INT32_T);
                    kh_set(khstrs, h_value, var, "0");
                } else {
                    /* Var found */
                    MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] already exists in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT32_T), curr_file);
                    return EX_F;
                }
            }

        }else if ((statement = check_statement(&_fstr, &i, "i64"))){
            size_t eq = is_in_str(statement, "=");
            char * var = statement;

            if (eq){
                /* Assignment */
                char * val = statement+eq+1;
                var[eq] = '\0';

                k = kh_get(khstri, h_datatype, var);
                if (k == kh_end(h_datatype)){
                    /* Var not found */
                    if (!is_intd(val, CK_INT64_T)){
                        MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] assigned wrong type in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT64_T), curr_file);
                        return EX_F;
                    }
                    kh_set(khstri, h_datatype, var, CK_INT64_T);
                    kh_set(khstrs, h_value, var, val);
                } else {
                    /* Var found */
                    MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] already exists in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT64_T), curr_file);
                    return EX_F;
                }
            }else{
                k = kh_get(khstri, h_datatype, var);
                if (k == kh_end(h_datatype)){
                    /* Var not found */
                    kh_set(khstri, h_datatype, var, CK_INT64_T);
                    kh_set(khstrs, h_value, var, "0");
                } else {
                    /* Var found */
                    MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] already exists in file {%s}", var, CK_DATATYPES_TO_STR(CK_INT64_T), curr_file);
                    return EX_F;
                }
            }

        /* Logging functions */
        }else if ((args = check_function(&_fstr, &i, CK_LERR_KW))){

            /* Escape sequences  */
            CK_replace_str(args, "\\n", "\n");
            CK_replace_str(args, "\\t", "\t");
            CK_replace_str(args, "\\r", "\r");
            /* * * * * * * * * * */

            MEL_log(LOG_ERROR, args);
        }else if ((args = check_function(&_fstr, &i, CK_LWRN_KW))){

            /* Escape sequences  */
            CK_replace_str(args, "\\n", "\n");
            CK_replace_str(args, "\\t", "\t");
            CK_replace_str(args, "\\r", "\r");
            /* * * * * * * * * * */

            MEL_log(LOG_WARNING, args);
        }else if ((args = check_function(&_fstr, &i, CK_LINF_KW))){

            /* Escape sequences  */
            CK_replace_str(args, "\\n", "\n");
            CK_replace_str(args, "\\t", "\t");
            CK_replace_str(args, "\\r", "\r");
            /* * * * * * * * * * */

            MEL_log(LOG_INFORMATION, args);
        /* * * * * * * * * * */

        }else if ((args = check_function(&_fstr, &i, CK_PRNT_KW))){

            if (args[0] == '\"'){
                /* Printing String Literal */

                ++args;

                /* Escape sequences  */
                CK_replace_str(args, "\\n", "\n");
                CK_replace_str(args, "\\t", "\t");
                CK_replace_str(args, "\\r", "\r");
                /* * * * * * * * * * */

                while (args[0] != '\"' && args[0] != '\0'){
                    if (0 == strncmp(args, "\\$", 2)){
                        putchar('$');
                        args += 2;
                    }else if (0 == strncmp(args, "$(", 2)){
                        args += 2;
                        char * var = args;
                        size_t index = 0;
                        while (args[0] != ')' && args[0] != '\0'){
                            ++index;
                            ++args;
                        }
                        var[index] = '\0';

                        if (strlen(var)){
                            /* Printing Variable */
                            k = kh_get(khstrs, h_value, var);  // first have to get ieter
                            if (k == kh_end(h_value)) {  // k will be equal to kh_end if key not present
                                MEL_log(LOG_ERROR, "Undefined variable named \"%s\" in file {%s}", var, curr_file);
                                return EX_F;
                            } else {
                                printf("%s", kh_val(h_value, k));
                            }
                            /* * * * * * * * * * */
                        }else{
                            MEL_log(LOG_ERROR, "Undefined variable named \"%s\" in file {%s}", var, curr_file);
                            return EX_F;
                        }
                    }
                    putchar(args[0]);
                    ++args;
                }
            }else{
                /* Printing Variable */
                k = kh_get(khstri, h_datatype, args);  // first have to get ieter
                if (k == kh_end(h_datatype)) {  // k will be equal to kh_end if key not present
                    MEL_log(LOG_ERROR, "Undefined variable named \"%s\" in file {%s}\n", args, curr_file);
                    return EX_F;
                } else {
                }
                /* * * * * * * * * * */
            }

        }else if ((args = check_function(&_fstr, &i, CK_WAIT_KW))){
            if (is_int(args)){
                sleep_ms(atoll(args));
            }else{
                MEL_log(LOG_ERROR, "Invalid \"%s()\" argument in file {%s}", CK_WAIT_KW, curr_file);
                return EX_F;
            }
        }else if ((args = check_function(&_fstr, &i, CK_CALL_KW))){
            if (args[0] == '\"' && args[strlen(args)-1] == '\"'){
                ++args;
                args[strlen(args)-1] = '\0';
                if (0 == strcmp(args, curr_file)){
                    MEL_log(LOG_WARNING, "Unsafe recursive loop through file {%s}", curr_file);
                    return EX_F;
                }
                interpret_file(args);
            }else{
                MEL_log(LOG_ERROR, "Missing \'\"\' at \"%s\" function in file {%s}", curr_file, CK_CALL_KW);
                return EX_F;
            }
        }else if ((args = check_function(&_fstr, &i, CK_RETN_KW))){
            if (is_int(args)){
                return atoll(args);
            }
            MEL_log(LOG_ERROR, "Invalid \"%s()\" return value in file {%s}", CK_RETN_KW, curr_file);
            return EX_F;
        }else{
            /* variable name? */
            char * var_n = _fstr;
            size_t index = 0;
            while (_fstr[0] != '+' && _fstr[0] != '=' && _fstr[0] != ';' && _fstr[0] != '\0'){
                ++index;
                advance_parser(_fstr, i, 1);
            }
            var_n[index] = '\0';

            if (strlen(var_n)){
                /* search through hashtable for variable name */
                k = kh_get(khstri, h_datatype, var_n);  // first have to get ieter
                if (k == kh_end(h_datatype)) {  // k will be equal to kh_end if key not present
                    MEL_log(LOG_ERROR, "Undefined keyword \"%s\"[%d] in file {%s}\n", var_n, i, curr_file);
                    return EX_F;
                } else {
                   /* Check for '=' or '+' or '-' [...] in _fstr[0] */
                   switch(_fstr[0]){
                       case 0:
                       case '=':
                            advance_parser(_fstr, i, 1);
                            char * val = _fstr;
                            size_t in = 0;
                            while (_fstr[0] != ';' && _fstr[0] != '\0'){
                                ++in;
                                advance_parser(_fstr, i, 1);
                            }
                           val[in] = '\0';

                           if (!is_intd(val, kh_val(h_datatype, k))){
                                MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] assigned \"%s\" in file {%s}", var_n, CK_DATATYPES_TO_STR(kh_val(h_datatype, k)), val, curr_file);
                                return EX_F;
                           }

                           kh_set(khstrs, h_value, var_n, val);
                           break;
                       default:
                           break;
                   }
                }
            }
        }
        advance_parser(_fstr, i, 1);
    }
   // cleanup and remove our hashtable
   kh_destroy(khstri, h_datatype);
   kh_destroy(khstrs, h_value);

    return EX_S;
}
