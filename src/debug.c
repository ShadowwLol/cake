#include "../include/debug.h"
#include "../include/value.h"

static uint64_t simple_instruction(const char * name, uint64_t offset){
	printf("%s\n", name);
	return offset + 1;
}
static uint64_t constant_instruction(const char * name, chunk_t * chunk, uint64_t offset){
	uint64_t constant = chunk->bytes[offset + 1];
	printf("%-16s %4lu `", name, constant);
	print_value(chunk->constants.values[constant]);
	printf("`\n");
	return offset + 2;
}

static uint64_t long_constant_instruction(const char * name, chunk_t * chunk, uint64_t offset){
	uint64_t constant =\
		chunk->bytes[offset + 1] | (chunk->bytes[offset + 2] << 8) | (chunk->bytes[offset + 3] << 16);
	printf("%-16s %4lu `", name, constant);
	print_value(chunk->constants.values[constant]);
	printf("`\n");
	return offset + 4;
}

void disassemble_chunk(chunk_t * chunk, const char * name){
	printf("### %s ###\n", name);

	for (uint64_t offset = 0; offset < chunk->count;){
		offset = disassemble_instruction(chunk, offset);
	}

	printf("### ");
	for (size_t i = 0; i < strlen(name); ++i){
		putchar('#');
	}
	printf(" ###\n");
}

uint64_t disassemble_instruction(chunk_t * chunk, uint64_t offset){
	printf("%04lu", offset);

	uint64_t line = get_line(chunk, offset);
	if (offset > 0 && line == get_line(chunk, offset - 1)){
		printf("\t%4c ", '^');
	}else{
		printf("\t%4lu ", line);
	}

	uint8_t instruction = chunk->bytes[offset];
	switch(instruction){
		case OP_CONSTANT:
			return constant_instruction("OP_CONSTANT", chunk, offset);
		case OP_CONSTANT_LONG:
			return long_constant_instruction("OP_CONSTANT_LONG", chunk, offset);
		case OP_NIL:
			return simple_instruction("OP_NIL", offset);
		case OP_TRUE:
			return simple_instruction("OP_TRUE", offset);
		case OP_FALSE:
			return simple_instruction("OP_FALSE", offset);
		case OP_POP:
			return simple_instruction("OP_POP", offset);
		case OP_GET_GLOBAL:
			return constant_instruction("OP_GET_GLOBAL", chunk, offset);
		case OP_SET_GLOBAL:
			return constant_instruction("OP_SET_GLOBAL", chunk, offset);
		case OP_DEFINE_GLOBAL:
			return constant_instruction("OP_DEFINE_GLOBAL", chunk, offset);
		case OP_NOT:
			return simple_instruction("OP_NOT", offset);
		case OP_NOT_EQUAL:
			return simple_instruction("OP_NOT_EQUAL", offset);
		case OP_EQUAL:
			return simple_instruction("OP_EQUAL", offset);
		case OP_GREATER:
			return simple_instruction("OP_GREATER", offset);
		case OP_GREATER_EQUAL:
			return simple_instruction("OP_GREATER_EQUAL", offset);
		case OP_LESS:
			return simple_instruction("OP_LESS", offset);
		case OP_LESS_EQUAL:
			return simple_instruction("OP_LESS_EQUAL", offset);
		case OP_ADD:
			return simple_instruction("OP_ADD", offset);
		case OP_SUBTRACT:
			return simple_instruction("OP_SUBTRACT", offset);
		case OP_MULTIPLY:
			return simple_instruction("OP_MULTIPLY", offset);
		case OP_DIVIDE:
			return simple_instruction("OP_DIVIDE", offset);
		case OP_REMAINDER:
			return simple_instruction("OP_REMAINDER", offset);
		case OP_MODULUS:
			return simple_instruction("OP_MODULUS", offset);
		case OP_INCREMENT:
			return simple_instruction("OP_INCREMENT", offset);
		case OP_DECREMENT:
			return simple_instruction("OP_DECREMENT", offset);
		case OP_NEGATE:
			return simple_instruction("OP_NEGATE", offset);
		case OP_PRINT:
			return simple_instruction("OP_PRINT", offset);
		case OP_RETURN:
			return simple_instruction("OP_RETURN", offset);
		default:
			MEL_log(LOG_WARNING, "Unknown opcode %lu", instruction);
			return offset + 1;
	}
}
