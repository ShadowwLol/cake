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
    CK_UINT8_T = 0, CK_UINT32_T, CK_UINT64_T,
    CK_INT8_T,  CK_INT32_T,  CK_INT64_T,
    CK_STR_T, CK_CHAR_T,
    CK_BOOL_T,
} CK_DATATYPES;

char * CK_DATATYPES_TO_STR(int8_t datatype){
    switch (datatype){
        case CK_UINT8_T:
            return "ui8";
        case CK_UINT32_T:
            return "ui32";
        case CK_UINT64_T:
            return "ui64";

        case CK_INT8_T:
            return "i8";
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

bool is_i32(const char * input){
    for (size_t i = 0; i < strlen(input); ++i){
        if (!isdigit(input[i])){ /* filters floats and strings */
            return false;
        }
    }
    if (atoll(input) >= INT32_MIN && atoll(input) <= INT32_MAX){
        return true;
    }
    return false;
}

char * check_function(char ** str, size_t * index, const char * fn){
    if (0 != strncmp(*str, fn, strlen(fn))){
        return NULL;
    }

    /* Using function */
    advance_parser(*str, index, strlen(fn));
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

#if 0
   kh_set(khStrInt, h, "jimbo", 99);
   kh_set(khStrInt, h, "john haze", 98);
   kh_set(khStrInt, h, "jaki joiner", 97);

   // Retrieve the value for key "apple"
   k = kh_get(khStrInt, h, "apple");  // first have to get ieter
   if (k == kh_end(h)) {  // k will be equal to kh_end if key not present
      printf("no key found for apple");
   } else {
      printf ("key at apple=%d\n", kh_val(h,k)); // next have to fetch  the actual value
   }

   // Retrieve the value for key "apple"
   k = kh_get(khStrInt, h, "john haze");  // first have to get ieter
   if (k == kh_end(h)) {  // k will be equal to kh_end if key not present
      printf("no key found for john haze\n");
   } else {
      printf ("key at john haze=%d\n", kh_val(h,k)); // next have to fetch  the actual value
   }

   // retreive key for the key "not_present"
   // which should print the error message
   k = kh_get(khStrInt, h, "not_present");  // first have to get ieter
   if (k == kh_end(h)) {  // k will be equal to kh_end if key not present
      printf("no key found for not_present\n");
   } else {
      printf ("key at not_present=%d\n", kh_val(h,k)); // next have to fetch  the actual value
   }

   // shorthand method to get value that returns value found
   // or specified default value if not present.
   int tval = kh_get_val(khStrInt, h, "apple", -1);
   printf ("shortcut tval for apple = %d\n", tval);

   tval = kh_get_val(khStrInt, h, "john haze", -1);
   printf ("shortcut tval for john haze = %d\n", tval);

   // missing key should return default value of -1
   tval = kh_get_val(khStrInt, h, "not_present", -1);
   printf ("shortcut tval for not_present = %d\n", tval);

   // Try to delete a key and then retrieve it
   // to see if it is really gone.
   k = kh_get(khStrInt, h, "john haze"); // get the ieterator
   if (k != kh_end(h)) {  // if it is found
      printf("deleting key john_haze\n");
      kh_del(khStrInt, h, k);  // then delete it.
   }
   // now see if it is really gone
   tval = tval = kh_get_val(khStrInt, h, "john haze", -1);
   printf ("after delete tval for john haze = %d\n", tval);

   // Ieterate the hash table by key, value and print out
   // the values found.
   printf("\nIeterate all keys\n");
   for (k = kh_begin(h); k != kh_end(h); ++k) {
      if (kh_exist(h, k)) {
         const char *key = kh_key(h,k);
         tval = kh_value(h, k);
         printf("key=%s  val=%d\n", key, tval);
      }
   }

#endif

    char * args;
    for (size_t i = 0; i <= sz && _fstr[0] != '\0'; ++i, ++_fstr){
        if (_fstr[0] == ';' || _fstr[0] == ')'){
            advance_parser(_fstr, i, 1);
        }

        if (0 == strncmp("i32", _fstr, 3)){
            bool assigned = false;

            advance_parser(_fstr, i, 3);

            char * var_n = _fstr;
            size_t index = 0;
            while (_fstr[0] != ';' && _fstr[0] != '\0'){
                if (_fstr[0] == '='){
                    /* Assignment to a variable */
                    advance_parser(_fstr, i, 1);
                    var_n[index] = '\0';

                    char * value = _fstr;
                    index = 0;
                    while (_fstr[0] != ';' && _fstr[0] != '\0'){
                        ++index;
                    advance_parser(_fstr, i, 1);
                    }
                    value[index] = '\0';
                    if (!is_i32(value)){
                        MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] assigned wrong type in file {%s}", var_n, CK_DATATYPES_TO_STR(kh_val(h_datatype, k)), curr_file);
                        return EX_F;
                    }

                    k = kh_get(khstri, h_datatype, var_n);  // first have to get ieter
                    if (k == kh_end(h_datatype)) {  // k will be equal to kh_end if key not present
                        kh_set(khstri, h_datatype, var_n, CK_INT32_T);
                        kh_set(khstrs, h_value, var_n, value);
                    } else {
                        MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] already exists in file {%s}", var_n, CK_DATATYPES_TO_STR(kh_val(h_datatype, k)), curr_file);
                        return EX_F;
                    }

                    assigned = true;
                    break;
                }
                ++index;
                advance_parser(_fstr, i, 1);
            }
            //k = kh_get(khstri, h_datatype, var_n);  // first have to get ieter
            //if (k == kh_end(h_datatype)) {  // k will be equal to kh_end if key not present
            //    kh_set(khstri, h_datatype, var_n, CK_INT32_T);
            //} else {
            //    MEL_log(LOG_ERROR, "Variable \"%s\" of type [%s] already exists in file {%s}", var_n, CK_DATATYPES_TO_STR(kh_val(h_datatype, k)), curr_file);
            //    return EX_F;
            //}

            if (!assigned){
                var_n[index] = '\0';
                kh_set(khstrs, h_value, var_n, "0"); // default all i32 to 0
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
                    printf("%s", kh_val(h_value,k)); // next have to fetch  the actual value
                }
                /* * * * * * * * * * */
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
        }else{
            /* variable name? */
            char * var_n = _fstr;
            size_t index = 0;
            while (_fstr[0] != '+' && _fstr[0] != '=' && _fstr[0] != ';' && _fstr[0] != '\0'){
                ++index;
                advance_parser(_fstr, i, 1);
            }
            var_n[index] = '\0';

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

                       bool a_failed = false;
                       switch(kh_val(h_datatype, k)){
                           case CK_INT32_T:
                                if (!is_i32(val)){a_failed = true;}
                                break;
                           default:
                               break;
                        }
                        if (a_failed){
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
   // cleanup and remove our hashtable
   kh_destroy(khstri, h_datatype);
   kh_destroy(khstrs, h_value);

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
    return EX_S;
}
