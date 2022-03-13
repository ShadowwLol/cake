#include <stdlib.h>
#include "../include/memory.h"
#include "../include/vm.h"
#include "../include/object.h"

void * allocate(void * array, size_t o_size, size_t n_size){
	if (!n_size){
		free(array);
		return NULL;
	}

	void * result = realloc(array, n_size);
	if (!result){
		MEL_log(LOG_ERROR, "Failed allocating %lu bytes", n_size);
		exit(EX_F);
	}
	return result;
}

static void free_object(obj_t * obj){
	switch(obj->type){
		case OBJ_STRING:{
			ostr_t * str = (ostr_t *)obj;
			allocate(obj, sizeof(ostr_t) + str->length + 1, 0);
			break;
		}
	}
}

void free_objects(void){
	obj_t * obj = vm.objects;
	while(obj){
		obj_t * next = obj->next;
		free_object(obj);
		obj = next;
	}
}
