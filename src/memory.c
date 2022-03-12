#include <stdlib.h>
#include "../include/memory.h"

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
