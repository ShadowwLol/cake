#include "../include/cake.h"
#include "../include/compiler.h"
#include "../include/lexer.h"

#ifdef _CAKE_DEBUG
#include "../include/debug.h"
#endif

typedef struct{
	token_t previous;
	token_t current;

	bool error;
	bool panic;
} parser_t;

typedef enum{
	PREC_NONE,
	PREC_ASSIGNMENT,  /* =         */
	PREC_OR,          /* or        */
	PREC_AND,         /* and       */
	PREC_EQUALITY,    /* == !=     */
	PREC_COMPARISON,  /* < > <= >= */
	PREC_TERM,        /* + -       */
	PREC_FACTOR,      /* * /       */
	PREC_UNARY,       /* ! - |     */
	PREC_CALL,        /* . ()      */
	PREC_PRIMARY
} prec_t;

typedef void (*parsefn)();

typedef struct{
	parsefn prefix;
	parsefn infix;
	prec_t prec;
} prule_t;

parser_t parser;
chunk_t * compiled_chunk;

static void expression();
static prule_t * get_rule(ttype_t type);
static void parse_prec(prec_t prec);

static chunk_t * current_chunk(void){
	return compiled_chunk;
}

static void error_at(token_t * token, const char * message){
	if (parser.panic){
		return;
	}

	parser.panic = true;
	switch(token->type){
		case TOKEN_EOF:
			MEL_log(LOG_ERROR, "[line %lu] Error at end: %s", token->line, message);
			break;
		case TOKEN_ERROR:
			/* TODO: Error management */
			break;
		default:
			MEL_log(LOG_ERROR, "[line %lu] Error at `%.*s`: %s", token->line, token->length, token->start, message);
			break;
	}

	parser.error = true;
}

static void error_previous(const char * message){
	error_at(&parser.previous, message);
}

static void error_current(const char * message){
	error_at(&parser.current, message);
}

static void advance(void){
	parser.previous = parser.current;

	while (1){
		parser.current = scan_token();
		if (parser.current.type != TOKEN_ERROR){
			break;
		}

		error_current(parser.current.start);
	}
}

static void consume(ttype_t type, const char * message){
	if (parser.current.type == type){
		advance();
		return;
	}

	error_current(message);
}

static void emit_byte(uint8_t byte){
	write_chunk(current_chunk(), byte, parser.previous.line);
}

static void emit_bytes(uint8_t byte1, uint8_t byte2){
	emit_byte(byte1);
	emit_byte(byte2);
}

static void emit_return(void){
	emit_byte(OP_RETURN);
}

static void emit_constant(value_t value){
	uint64_t constant = append_constant(current_chunk(), value);
	if (constant < 256){
		emit_bytes(OP_CONSTANT, constant);
	}else{
		emit_bytes(OP_CONSTANT_LONG, (uint8_t)(constant & 0xff));
		emit_bytes((uint8_t)((constant >> 8) & 0xff), (uint8_t)((constant >> 16) & 0xff));
	}
}

static void end_compiler(void){
	emit_return();
#ifdef _CAKE_DEBUG
	if (!parser.error){
		disassemble_chunk(current_chunk(), "code");
	}
#endif
}

static void binary(void){
	ttype_t op_type = parser.previous.type;
	prule_t * rule = get_rule(op_type);

	parse_prec((prec_t)(rule->prec + 1));

	switch(op_type){
		case TOKEN_PLUS:
			emit_byte(OP_ADD);
			break;
		case TOKEN_MINUS:
			emit_byte(OP_SUBTRACT);
			break;
		case TOKEN_STAR:
			emit_byte(OP_MULTIPLY);
			break;
		case TOKEN_SLASH:
			emit_byte(OP_DIVIDE);
			break;
		case TOKEN_PERCENT:
			emit_byte(OP_REMAINDER);
			break;
		case TOKEN_COLUMN:
			emit_byte(OP_MODULUS);
			break;
		default:
			return;
	}
}

static void grouping(void){
	expression();
	consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static void number(void){
	emit_constant(strtod(parser.previous.start, NULL));
}

static void unary(void){
	ttype_t op = parser.previous.type;

	switch(op){
		case TOKEN_MINUS:
			parse_prec(PREC_UNARY);
			emit_byte(OP_NEGATE);
			break;
		case TOKEN_COLUMN:
			expression();
			consume(TOKEN_COLUMN, "Expect '|' after expression.");
			emit_byte(OP_MODULUS);
			break;
		default:
			break;
	}
}

prule_t rules[] = {
	[TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
	[TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
	[TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
	[TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
	[TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
	[TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
	[TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
	[TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
	[TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
	[TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
	[TOKEN_PERCENT]       = {NULL,     binary, PREC_FACTOR},
	[TOKEN_COLUMN]        = {unary,    NULL,   PREC_NONE},
	[TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
	[TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
	[TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
	[TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
	[TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
	[TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
	[TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
	[TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
	[TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
	[TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
	[TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
	[TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
	[TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
	[TOKEN_FN]            = {NULL,     NULL,   PREC_NONE},
	[TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
	[TOKEN_IN]            = {NULL,     NULL,   PREC_NONE},
	[TOKEN_INCLUDE]       = {NULL,     NULL,   PREC_NONE},
	[TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
	[TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
	[TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
	[TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
	[TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
	[TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
	[TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
	[TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

static void parse_prec(prec_t prec){
	advance();
	parsefn prefix_rule = get_rule(parser.previous.type)->prefix;
	if (!prefix_rule){
		error_previous("Expect expression.");
		return;
	}

	prefix_rule();

	while (prec <= get_rule(parser.current.type)->prec){
		advance();
		parsefn infix_rule = get_rule(parser.previous.type)->infix;

		infix_rule();
	}
}

static prule_t * get_rule(ttype_t type){
	return &rules[type];
}

static void expression(void){
	parse_prec(PREC_ASSIGNMENT);
}

bool compile(const char * source, chunk_t * chunk){
	init_lexer(source);
	compiled_chunk = chunk;

	parser.error = false;
	parser.panic = false;

	advance();
	expression();
	consume(TOKEN_EOF, "Expect end of expression");

	end_compiler();
	return !parser.error;
}
