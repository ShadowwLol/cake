#ifndef _MEMORY_H
#define _MEMORY_H

#include "cake.h"

#define GROW_CAPACITY(capacity) (((capacity) < 8) ? 8 : (capacity) * 2)
#define GROW_ARRAY(type, array, o_size, n_size)\
	(type*)allocate(array, sizeof(type) * o_size, sizeof(type) * n_size)

#define FREE_ARRAY(type, array, o_size) allocate(array, sizeof(type) * o_size, 0)

void * allocate(void * array, size_t o_size, size_t n_size);

#endif
