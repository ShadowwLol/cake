#ifndef _VALUE_H
#define _VALUE_H

#include "cake.h"

typedef enum{
	VAL_BOOL,
	VAL_NIL,
	VAL_NUMBER,
} vtype_t;

typedef struct{
	vtype_t type;
	union{
		bool boolean;
		double number;
	} as;
} value_t;

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)

#define BOOL_VAL(value) ((value_t){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((value_t){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((value_t){VAL_NUMBER, {.number = value}})

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
