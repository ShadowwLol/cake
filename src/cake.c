#include "../include/cake.h"
#include "../include/chunk.h"
#include "../include/debug.h"

int32_t main(int32_t argc, char * argv[]){
	chunk_t chunk = init_chunk();

	write_constant(&chunk, 68, 12);
	write_chunk(&chunk, OP_RETURN, 12);

	disassemble_chunk(&chunk, "test");
	free_chunk(&chunk);
	return EX_S;
}
