#include "../include/lexer.h"
#include "../include/cake.h"

typedef struct{
	const char * start;
	const char * current;
	uint64_t line;
} lexer_t;

lexer_t lexer;

void init_lexer(const char * source){
	lexer.start = source;
	lexer.current = source;
	lexer.line = 1;
}

static char advance(void){
	++lexer.current;
	return lexer.current[-1];
}

static bool match(char expected){
	if (*lexer.current == '\0' || *lexer.current != expected){
		return false;
	}

	++lexer.current;
	return true;
}

static token_t make_token(ttype_t type){
	token_t token = {
		.type = type,
		.start = lexer.start,
		.length = (lexer.current - lexer.start),
		.line = lexer.line,
	};
	return token;
}

static token_t error_token(const char * message){
	token_t token = {
		.type = TOKEN_ERROR,
		.start = message,
		.length = strlen(message),
		.line = lexer.line,
	};
	return token;
}

static void skip_whitespace(void){
	bool commenting = false;
	while (1){
		if (isspace(*lexer.current) || commenting){
			if (*lexer.current == '\n'){
				if (commenting){
					commenting = false;
				}
				++lexer.line;
			}
			advance();
		}else{
			if (*lexer.current == '/' && lexer.current[1] == '/'){
				commenting = true;
				continue;
			}
			return;
		}
	}
}

static ttype_t check_keyword(uint16_t start, uint16_t length, const char * rest, ttype_t type){
	if ((lexer.current - lexer.start) == (start + length) && memcmp(lexer.start + start, rest, length) == 0){
		return type;
	}

	return TOKEN_IDENTIFIER;
}

static ttype_t identifier_type(void){
	switch(lexer.start[0]){
		case 'a': return check_keyword(1, 2, "nd",    TOKEN_AND);
		case 'c': return check_keyword(1, 2, "lass",  TOKEN_CLASS);
		case 'e': return check_keyword(1, 2, "lse",   TOKEN_ELSE);
		case 'f':
			if (lexer.current - lexer.start > 1){
				switch (lexer.start[1]){
					case 'a': return check_keyword(2, 3, "lse", TOKEN_FALSE);
					case 'o': return check_keyword(2, 1, "r", TOKEN_FOR);
					case 'n': return TOKEN_FN;
				}
			}
			break;
		case 'i':
			if (lexer.current - lexer.start > 1){
				switch (lexer.start[1]){
					case 'f': return TOKEN_IF;
					case 'n':
						if (lexer.current - lexer.start > 2){
							switch (lexer.start[2]){
								case 'c': return check_keyword(3, 4, "lude", TOKEN_INCLUDE);
							}
						}else{
							return TOKEN_IN;
						}
						return TOKEN_IDENTIFIER;
				}
			}
			break;
		case 'n': return check_keyword(1, 2, "il",    TOKEN_NIL);
		case 'o': return check_keyword(1, 2, "r",     TOKEN_OR);
		case 'p': return check_keyword(1, 2, "rint",  TOKEN_PRINT);
		case 'r': return check_keyword(1, 2, "eturn", TOKEN_RETURN);
		case 's': return check_keyword(1, 2, "uper",  TOKEN_SUPER);
		case 't':
			if (lexer.current - lexer.start > 1){
				switch (lexer.start[1]){
					case 'h': return check_keyword(2, 2, "is", TOKEN_THIS);
					case 'r': return check_keyword(2, 2, "ue", TOKEN_TRUE);
				}
			}
			break;
		case 'v': return check_keyword(1, 2, "ar",    TOKEN_VAR);
		case 'w': return check_keyword(1, 2, "hile",  TOKEN_WHILE);
	}
	return TOKEN_IDENTIFIER;
}

static token_t identifier(void){
	while (isalnum(*lexer.current)){
		advance();
	}
	return make_token(identifier_type());
}

static token_t number(void){
	while (isdigit(*lexer.current)){
		advance();
	}

	if (*lexer.current == '.' && isdigit(lexer.current[1])){
		advance();

		while (isdigit(*lexer.current)){
			advance();
		}
	}

	return make_token(TOKEN_NUMBER);
}

static token_t string(void){
	while (*lexer.current != '"' && *lexer.current != '\0'){
		if (*lexer.current == '\n'){
			++lexer.line;
		}
		advance();
	}

	if (*lexer.current == '\0'){
		return error_token("Unterminated string.");
	}

	/* " */
	advance();
	return make_token(TOKEN_STRING);
}

token_t scan_token(void){
	skip_whitespace();
	lexer.start = lexer.current;
	if (*lexer.current == '\0'){
		return make_token(TOKEN_EOF);
	}

	char c = advance();

	if (isalpha(c)){
		return identifier();
	}

	if (isdigit(c)){
		return number();
	}

	switch(c){
		case '(': return make_token(TOKEN_LEFT_PAREN);
		case ')': return make_token(TOKEN_RIGHT_PAREN);
		case '{': return make_token(TOKEN_LEFT_BRACE);
		case '}': return make_token(TOKEN_RIGHT_BRACE);
		case ';': return make_token(TOKEN_SEMICOLON);
		case ',': return make_token(TOKEN_COMMA);
		case '.': return make_token(TOKEN_DOT);
		case '-': return make_token(TOKEN_MINUS);
		case '+': return make_token(TOKEN_PLUS);
		case '/': return make_token(TOKEN_SLASH);
		case '*': return make_token(TOKEN_STAR);
		case '%': return make_token(TOKEN_PERCENT);
		case '|': return make_token(TOKEN_COLUMN);
		case '!': return make_token(match('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
		case '=': return make_token(match('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
		case '<': return make_token(match('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
		case '>': return make_token(match('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
		case '"': return string();
	}

	return error_token("Unexpected character");
}
