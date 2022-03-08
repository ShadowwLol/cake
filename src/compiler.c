#include "../include/cake.h"
#include "../include/compiler.h"
#include "../include/lexer.h"

void compile(const char * source){
	init_lexer(source);
	uint64_t line = 0;
	while (1){
		token_t token = scan_token();
		if (token.line != line){
			printf("\t%4lu ", token.line);
			line = token.line;
		}else{
			printf("\t%4c ", '^');
		}
		printf("%2d '%.*s'\n", token.type, (int)token.length, token.start);

		if (token.type == TOKEN_EOF){
			break;
		}
	}
}
