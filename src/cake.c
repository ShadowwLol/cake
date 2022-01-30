#include <getopt.h>
#include "../include/cake.h"

CK_FN interpret_file(const char * filename){
    int err;
    size_t sz;
    char * file = CK_read_file(filename, &sz);

    if (!file){
        return EX_F;
    }

    size_t s = lex_string(filename, file, sz);

    int64_t ret = parse_string(filename, file, s);
    free(file);
    return ret;
}

int main(int argc, char * argv[]){
    if (argc < 2){
        return EX_F;
    }

	/* Parsing arguments */
	int option = 0;
	if ((option = getopt(argc, argv, "di:")) != -1){
        /* FIXME: getopt only handles one arg at a time */
		switch(option){
            case 'd':
                /* Debugging */
                if (argc < 3){
                    return EX_F;
                }
                set_debug(true);
                MEL_log(LOG_INFORMATION, "Debugging");
                break;
			case 'i':
				/* Interpret */
                return interpret_file(optarg);
			default:
				CK_USAGE();
				return EX_F;
		}
	}
	/* * * * * * * * * * */

    //parse_file(argv[1]);

    return EX_S;
}
