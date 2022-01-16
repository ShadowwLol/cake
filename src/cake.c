#include <getopt.h>
#include "../include/cake.h"

CK_FN interpret_file(const char * filename){
    int err;
    size_t sz;
    char * file = CK_read_file(filename, &sz);

    if (!file){
        return EX_F;
    }

    char _file[sz];
    lex_string(file, sz, _file);
    //printf("FSTR:\n%s\n\n_FSTR:\n%s\n\n", file, _file);
    free(file);

    parse_string(filename, _file, sz);
}

int main(int argc, char * argv[]){
    if (argc < 2){
        return EX_F;
    }

	/* Parsing arguments */
	int option = 0;
	if ((option = getopt(argc, argv, "i:")) != -1){
		switch(option){
			case 'i':
				/* Interpret */
                interpret_file(optarg);
				break;
			case '?':
				CK_USAGE();
				return EX_F;
		}
	}
	/* * * * * * * * * * */

    //parse_file(argv[1]);

    return EX_S;
}
