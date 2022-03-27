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

static uint32_t hash_str(const char* key, uint64_t length) {
	uint32_t hash = 2166136261u;
	for (uint64_t i = 0; i < length; i++) {
		hash ^= (uint8_t)key[i];
		hash *= 16777619;
	}
	return hash;
}

static ostr_t * alloc_str(bool owns_buffer, char * buffer, uint64_t length, uint32_t hash){
	ostr_t * str = ALLOCATE_OBJ(ostr_t, OBJ_STRING);
	str->owns_buffer = owns_buffer;
	str->length = length;
	str->buffer = buffer;
	str->hash = hash;
	set_table(&vm.strings, str, NIL_VAL);
	return str;
}

ostr_t * copy_str(bool owns_buffer, const char * buffer, uint64_t length){
	uint32_t hash = hash_str(buffer, length);
	ostr_t * interned = table_find_str(&vm.strings, buffer, length, hash);
	if (interned != NULL){
		return interned;
	}

	return alloc_str(owns_buffer, (char *)buffer, length, hash);
}

void print_object(value_t value){
	switch(OBJ_TYPE(value)){
		case OBJ_STRING:
			printf("%.*s", (int)AS_STRING(value)->length, AS_CSTRING(value));
			break;
	}
}
