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

static ostr_t * alloc_str(char * buffer, uint64_t length, uint32_t hash){
	ostr_t * str = ALLOCATE_OBJ(ostr_t, OBJ_STRING);
	str->length = length;
	str->buffer = buffer;
	str->hash = hash;
	set_table(&vm.strings, str, NIL_VAL);
	return str;
}

ostr_t * copy_str(const char * buffer, uint64_t length){
	uint32_t hash = hash_str(buffer, length);
	ostr_t * interned = table_find_str(&vm.strings, buffer, length, hash);
	if (interned != NULL){
		return interned;
	}

	char * heap = ALLOCATE(char, length + 1);
	memcpy(heap, buffer, length);
	heap[length] = '\0';
	return alloc_str(heap, length, hash);
}

ostr_t * take_str(char * buffer, uint64_t length){
	uint32_t hash = hash_str(buffer, length);
	ostr_t * interned = table_find_str(&vm.strings, buffer, length, hash);
	if (interned != NULL){
		FREE_ARRAY(char, buffer, length + 1);
		return interned;
	}
	return alloc_str(buffer, length, hash);
}

void print_object(value_t value){
	switch(OBJ_TYPE(value)){
		case OBJ_STRING:
			printf("%s", AS_CSTRING(value));
			break;
	}
}
