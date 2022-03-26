#ifndef _VM_H
#define _VM_H

#include "chunk.h"
#include "value.h"
#include "table.h"

typedef enum{
	INTERPRET_OK,
	INTERPRET_COMPILE_ERROR,
	INTERPRET_RUNTIME_ERROR,
} interpret_result;

typedef struct{
	chunk_t * chunk;
	uint8_t * ip;

	uint64_t stack_count;
	uint64_t stack_capacity;
	value_t * stack;

	table_t strings;
	obj_t * objects;
} vm_t;

extern vm_t vm;

void init_vm(void);
interpret_result interpret(const char * source);
void free_vm(void);

bool values_equal(value_t val1, value_t val2);
void push(value_t value);
value_t pop(void);

#endif
