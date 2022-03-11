#ifndef _COMPILER_H
#define _COMPILER_H

#include "vm.h"
#include "chunk.h"

bool compile(const char * source, chunk_t * chunk);

#endif
