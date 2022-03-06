#ifndef _VALUE_H
#define _VALUE_H

#include "cake.h"

typedef double value_t;

typedef struct{
	uint64_t count;
	uint64_t capacity;
	value_t * values;
} value_array_t;

value_array_t init_value_array(void);
void write_value_array(value_array_t * array, value_t value);
void free_value_array(value_array_t * array);

void print_value(value_t value);

#endif
