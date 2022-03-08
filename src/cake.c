#include <getopt.h>
#include "../include/cake.h"
#include "../include/chunk.h"
#include "../include/vm.h"

#include "../include/debug.h"

static FN repl(bool debug){
	char line[1024];
	while (1){
		printf("> ");

		if (!fgets(line, sizeof(line), stdin)){
			putchar('\n');
			break;
		}

		interpret(line);
	}
	return EX_S;
}

static FN interpret_file(bool debug, const char * path){
	String file = init_str();
	file = MEL_read_file(path);
	interpret_result result = interpret(file.buffer);
	remove_str(&file);

	if (result == INTERPRET_OK){
		return EX_S;
	}

	return EX_F;
}

int32_t main(int32_t argc, char * argv[]){
	static bool debug = false;

	init_vm();

		static struct option long_options[] = {
		/* These options set a flag. */
		{"help",        no_argument,            0,      'h'},
		{"debug",       no_argument,      (int*)&debug,  1 },
		{"interpret",   required_argument,      0,      'i'},
		{"compile",     required_argument,      0,      'c'},
		{0, 0, 0, 0}
	};

	int32_t option_index = 0;
	int32_t c = getopt_long(argc, argv, "hi:c:", long_options, &option_index);

	if (debug){
		printf("Debugging\n");
	}

	switch (c){
		case -1:
			/* Repl  */
			printf("repl\n");
			repl(debug);
			/* * * * */
			return EX_F;

		case 'i':
			/* Interpreting  */
			printf("interpreting\n");
			interpret_file(debug, optarg);
			/* * * * * * * * */
			break;

		case 'c':
			/* Compiling */
			printf("compiling\n");
			/* * * * * * */
			break;

		default:
		case 'h':
			MEL_log(LOG_INFORMATION, "help menu:\n"
									 "\t--help, -h\n"
									 "\t\tDisplay this help menu\n"
									 "\t--debug\n"
									 "\t\tEnable debugging/verbose\n"
									 "\t--interpret, -i <file>\n"
									 "\t\tInterpret a file\n"
									 "\t--compile, -c <file>\n"
									 "\t\tCompile a file\n");
			break;
	}
	free_vm();
	return EX_S;

#if 0
	chunk_t chunk = init_chunk();

	write_constant(&chunk, 22, 13);
	write_chunk(&chunk, OP_NEGATE, 13);
	write_constant(&chunk, 68, 14);

	write_chunk(&chunk, OP_ADD, 14);
	write_constant(&chunk, 1, 15);
	write_chunk(&chunk, OP_INCREMENT, 15);
	write_chunk(&chunk, OP_MODULUS, 16);

	write_chunk(&chunk, OP_RETURN, 16);

	disassemble_chunk(&chunk, "test");
	interpret(&chunk);

	free_chunk(&chunk);
	free_vm();
	return EX_S;
#endif
}
