#include "../include/cake.h"
#include "../include/chunk.h"
#include "../include/vm.h"

#include "../include/debug.h"

int32_t main(int32_t argc, char * argv[]){
	init_vm();
	chunk_t chunk = init_chunk();

	write_constant(&chunk, 22, 13);
	write_chunk(&chunk, OP_NEGATE, 13);
	write_constant(&chunk, 68, 14);

	write_chunk(&chunk, OP_ADD, 14);
	write_constant(&chunk, 1, 15);
	write_chunk(&chunk, OP_INCREMENT, 15);
	write_chunk(&chunk, OP_MODULUS, 16);

	write_chunk(&chunk, OP_RETURN, 16);

	disassemble_chunk(&chunk, "test");
	interpret(&chunk);

	free_chunk(&chunk);
	free_vm();
	return EX_S;
}
