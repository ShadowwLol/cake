#ifndef _OBJECT_H
#define _OBJECT_H

#include "cake.h"
#include "value.h"
#include "table.h"

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
	bool owns_buffer;
	uint64_t length;
	const char * buffer;
	uint32_t hash;
};

static inline bool is_obj_type(value_t value, otype_t type){
	return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

ostr_t * copy_str(bool owns_buffer, const char * buffer, uint64_t length);

void print_object(value_t value);

#endif /* _OBJECT_H */
