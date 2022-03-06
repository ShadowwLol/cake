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
}

uint64_t disassemble_instruction(chunk_t * chunk, uint64_t offset){
	printf("%04lu", offset);

	uint64_t line = get_line(chunk, offset);
	if (offset > 0 && line == get_line(chunk, offset - 1)){
		printf("\t^ ");
	}else{
		printf("%4lu ", line);
	}

	uint8_t instruction = chunk->bytes[offset];
	switch(instruction){
		case OP_CONSTANT:
			return constant_instruction("OP_CONSTANT", chunk, offset);
		case OP_CONSTANT_LONG:
			return long_constant_instruction("OP_CONSTANT_LONG", chunk, offset);
		case OP_RETURN:
			return simple_instruction("OP_RETURN", offset);
		default:
			MEL_log(LOG_WARNING, "Unknown opcode %lu", instruction);
			return offset + 1;
	}
}
