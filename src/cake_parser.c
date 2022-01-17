#include "../include/cake.h"

const int khstri = 33;
KHASH_MAP_INIT_STR(khstri, int);

CK_FN parse_string(const char * curr_file, char * _fstr, const size_t sz){
   int is_missing;
   khiter_t k;
   khash_t(khstri) *h = kh_init(khstri); // create a hashtable

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

    CK_replace_str(_fstr, "\%{here}", curr_file);
    for (size_t i = 0; i <= sz && _fstr[0] != '\0'; ++i, ++_fstr){
        if (_fstr[0] == ';'){
            //printf("%s\n", _fstr);
            ++_fstr;
        }

        if (0 == strncmp("i32", _fstr, 3)){
            bool assigned = false;

            _fstr += 3;
            char * var_n = _fstr;
            size_t index = 0;
            while (_fstr[0] != ';' && _fstr[0] != '\0'){
                if (_fstr[0] == '='){
                    /* Assignment to a variable */
                    ++_fstr;
                    var_n[index] = '\0';

                    char * value = _fstr;
                    index = 0;
                    while (_fstr[0] != ';' && _fstr[0] != '\0'){
                        ++index;
                        ++i;
                        ++_fstr;
                    }
                    value[index] = '\0';

                    char * e = NULL;
                    long number = strtol(value, &e, 10);
                    kh_set(khstri, h, var_n, number); // default all i32 to 0
                    if (e == value){
                        MEL_log(LOG_ERROR, "Invalid i32 assignment in file {%s}\n", curr_file);
                        return EX_F;
                    }
                    assigned = true;
                    break;
                }
                ++index;
                ++i;
                ++_fstr;
            }
            if (!assigned){
                var_n[index] = '\0';
                kh_set(khstri, h, var_n, 0); // default all i32 to 0
            }
        }else if (0 == strncmp("print(", _fstr, 6)){
            i += 6;
            _fstr += 6;

            if (_fstr[0] == '\"'){
                /* Printing string literal */
                ++_fstr;
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
                                    printf("%0*d^\n", (int)i, ' ');
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
                /* * * * * * * * * * * * * */
            }else{
                /* Printing variable */
                char * var_n = _fstr;
                size_t index = 0;
                while (_fstr[0] != ')' && _fstr[0] != ';' && _fstr[0] != '\0'){
                    ++index;
                    ++_fstr;
                    ++i;
                }
                var_n[index] = '\0';

                // Retrieve the value for the specified key
                k = kh_get(khstri, h, var_n);  // first have to get ieter
                if (k == kh_end(h)) {  // k will be equal to kh_end if key not present
                    MEL_log(LOG_ERROR, "Undefined variable named \"%s\" in file {%s}\n", var_n, curr_file);
                    return EX_F;
                } else {
                   printf("%d", kh_val(h,k)); // next have to fetch  the actual value
                }
                /* * * * * * * * * * */
            }
            //printf("printing\n");
        }else if (0 == strncmp("use(\"", _fstr, 5)){
            i+=5;
            _fstr+=5;

            //printf("\n#####\nline:\"%s\"\n#####\n", _fstr);

            char * filename = _fstr;
            size_t index = 0;
            while (_fstr[0] != '\"' && _fstr[0] != '\0'){
                ++index;
                ++i;
                ++_fstr;
            }
            filename[index] = '\0';

            //printf("FILENAME: \"%s\";\n", filename);
            if (0 == strcmp(filename, curr_file)){
                MEL_log(LOG_WARNING, "Unsafe recursive loop through file");
                return EX_F;
            }
            interpret_file(filename);

            //printf("printing\n");
        }
    }
   // cleanup and remove our hashtable
   kh_destroy(khstri, h);

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
