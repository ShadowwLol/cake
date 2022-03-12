#include <stdlib.h>
#include "../include/chunk.h"

chunk_t init_chunk(void){
	chunk_t chunk = {
		.count = 0,
		.capacity = 0,
		.bytes = NULL,
		.constants = init_value_array(),

		.line_count = 0,
		.line_capacity = 0,
		.lines = NULL,
	};
	return chunk;
}

void write_chunk(chunk_t * chunk, uint8_t byte, uint64_t line){
	if (chunk->capacity < (chunk->count + 1)){
		uint64_t o_capacity = chunk->capacity;
		chunk->capacity = GROW_CAPACITY(o_capacity);
		chunk->bytes = GROW_ARRAY(uint8_t, chunk->bytes, o_capacity, chunk->capacity);
	}
	chunk->bytes[chunk->count] = byte;
	++chunk->count;

	if  (chunk->line_count > 0 && chunk->lines[chunk->line_count - 1].line == line){
		/* same line as before */
		return;
	}

	if (chunk->line_capacity < (chunk->line_count + 1)){
		uint64_t o_capacity = chunk->line_capacity;
		chunk->line_capacity = GROW_CAPACITY(o_capacity);
		chunk->lines = GROW_ARRAY(line_t, chunk->lines, o_capacity, chunk->line_capacity);
	}

	chunk->lines[chunk->line_count].offset = chunk->count - 1 ;
	chunk->lines[chunk->line_count].line = line;
	++chunk->line_count;
}

uint64_t append_constant(chunk_t * chunk, value_t value){
	write_value_array(&chunk->constants, value);
	return chunk->constants.count - 1;
}

void write_constant(chunk_t * chunk, value_t value, uint64_t line){
	uint64_t constant = append_constant(chunk, value);
	if (constant < 256){
		write_chunk(chunk, OP_CONSTANT, line);
		write_chunk(chunk, (uint8_t)constant, line);
	}else{
		write_chunk(chunk, OP_CONSTANT_LONG, line);
		write_chunk(chunk, (uint8_t)(constant & 0xff), line);
		write_chunk(chunk, (uint8_t)((constant >> 8) & 0xff), line);
		write_chunk(chunk, (uint8_t)((constant >> 16) & 0xff), line);
	}
}

void free_chunk(chunk_t * chunk){
	FREE_ARRAY(uint8_t, chunk->bytes, chunk->capacity);
	FREE_ARRAY(line_t, chunk->lines, chunk->line_capacity);
	free_value_array(&chunk->constants);
	*chunk = init_chunk();
}

uint64_t get_line(chunk_t * chunk, uint64_t instruction){
	uint64_t start = 0;
	uint64_t end = chunk->line_count - 1;

	while (1){
		uint64_t mid = (start + end) / 2;
		if (instruction < chunk->lines[mid].offset){
			end = mid - 1;
		}else if (mid == chunk->line_count - 1 || instruction < chunk->lines[mid + 1].offset){
			return chunk->lines[mid].line;
		}else{
			start = mid + 1;
		}
	}
}
