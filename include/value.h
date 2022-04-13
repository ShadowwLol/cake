#ifndef _VALUE_H
#define _VALUE_H

#include "cake.h"

typedef struct obj_t obj_t;
typedef struct ostr_t ostr_t;

typedef enum{
	VAL_BOOL,
	VAL_NIL,
	VAL_NUMBER,
	VAL_OBJ,

	VAL_UNDEFINED,
} vtype_t;

typedef struct{
	vtype_t type;
	union{
		bool boolean;
		double number;
		obj_t * obj;
	} as;
} value_t;

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)
#define IS_OBJ(value) ((value).type == VAL_OBJ)
#define IS_UNDEFINED(value) ((value).type == VAL_UNDEFINED)

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)
#define AS_OBJ(value) ((value).as.obj)

#define BOOL_VAL(value) ((value_t){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((value_t){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((value_t){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(value) ((value_t){VAL_OBJ, {.obj = (obj_t *)value}})
#define UNDEFINED_VAL ((value_t){VAL_UNDEFINED, {.number = 0}})

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
