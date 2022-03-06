#ifndef _DEBUG_H
#define _DEBUG_H

#include "chunk.h"

void disassemble_chunk(chunk_t * chunk, const char * name);
uint64_t disassemble_instruction(chunk_t * chunk, uint64_t offset);

#endif
