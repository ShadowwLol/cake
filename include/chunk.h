#ifndef _CHUNK_H
#define _CHUNK_H

#include "cake.h"
#include "memory.h"
#include "value.h"

typedef enum{
	OP_CONSTANT,
	OP_CONSTANT_LONG,
	OP_NIL,
	OP_TRUE,
	OP_FALSE,
	OP_POP,
	OP_GET_GLOBAL,
	OP_SET_GLOBAL,
	OP_DEFINE_GLOBAL,
	OP_NOT,
	OP_NOT_EQUAL,
	OP_EQUAL,
	OP_GREATER,
	OP_GREATER_EQUAL,
	OP_LESS,
	OP_LESS_EQUAL,
	OP_ADD,
	OP_SUBTRACT,
	OP_MULTIPLY,
	OP_DIVIDE,
	OP_REMAINDER,
	OP_MODULUS,
	OP_INCREMENT,
	OP_DECREMENT,
	OP_NEGATE,
	/* built in functions  */
	OP_PRINT,
	/* * * * * * * * * * * */
	OP_RETURN,
} opcode_t;

typedef struct{
	uint64_t offset;
	uint64_t line;
} line_t;

typedef struct{
	uint64_t count;
	uint64_t capacity;
	uint8_t * bytes;
	value_array_t constants;

	uint64_t line_count;
	uint64_t line_capacity;
	line_t * lines;
} chunk_t;

chunk_t init_chunk(void);
void write_chunk(chunk_t * chunk, uint8_t byte, uint64_t line);
uint64_t append_constant(chunk_t * chunk, value_t value);
void write_constant(chunk_t * chunk, value_t value, uint64_t line);
void free_chunk(chunk_t * chunk);
uint64_t get_line(chunk_t * chunk, uint64_t instruction);

#endif
