#include "../include/object.h"
#include "../include/memory.h"
#include "../include/vm.h"

#define ALLOCATE_OBJ(type, obj_type)\
	(type*)allocate_obj(sizeof(type), obj_type)

static obj_t * allocate_obj(size_t size, otype_t type){
	obj_t * obj = (obj_t *)allocate(NULL, 0, size);
	obj->type = type;
	obj->next = vm.objects;
	vm.objects = obj;
	return obj;
}

ostr_t * heap_str(uint64_t length){
	ostr_t * str = (ostr_t *)allocate_obj(sizeof(ostr_t) + length + 1, OBJ_STRING);
	str->length = length;
	return str;
}

ostr_t * copy_str(const char * buffer, uint64_t length){
	ostr_t * str = heap_str(length);
	memcpy(str->buffer, buffer, length);
	str->buffer[length] = '\0';
	return str;
}

void print_object(value_t value){
	switch(OBJ_TYPE(value)){
		case OBJ_STRING:
			printf("%s", AS_CSTRING(value));
			break;
	}
}
