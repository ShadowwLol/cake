#ifndef _OBJECT_H
#define _OBJECT_H

#include "cake.h"
#include "value.h"

#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_STRING(value) is_obj_type(value, OBJ_STRING)

#define AS_STRING(value) ((ostr_t *)AS_OBJ(value))
#define AS_CSTRING(value) (((ostr_t *)AS_OBJ(value))->buffer)

typedef enum{
	OBJ_STRING,
}otype_t;

struct obj_t{
	otype_t type;
	struct obj_t * next;
};

struct ostr_t{
	obj_t obj;
	uint64_t length;
	char buffer[];
};

static inline bool is_obj_type(value_t value, otype_t type){
	return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

ostr_t * heap_str(uint64_t length);
ostr_t * copy_str(const char * buffer, uint64_t length);

void print_object(value_t value);

#endif /* _OBJECT_H */
