#include "../include/value.h"
#include "../include/memory.h"

value_array_t init_value_array(void){
	value_array_t array = {
		.count = 0,
		.capacity = 0,
		.values = NULL,
	};
	return array;
}

void write_value_array(value_array_t * array, value_t value){
	if (array->capacity < (array->count + 1)){
		uint64_t o_capacity = array->capacity;
		array->capacity = GROW_CAPACITY(o_capacity);
		array->values = GROW_ARRAY(value_t, array->values, o_capacity, array->capacity);
	}
	array->values[array->count] = value;
	++array->count;
}

void free_value_array(value_array_t * array){
	FREE_ARRAY(value_t, array->values, array->capacity);
	*array = init_value_array();
}

void print_value(value_t value){
	switch(value.type){
		case VAL_NIL:
			printf("nil");
			break;
		case VAL_BOOL:
			printf(AS_BOOL(value) ? "true" : "false");
			break;
		case VAL_NUMBER:
			printf("%g", AS_NUMBER(value));
			break;
		default:
			return;
	}
}
