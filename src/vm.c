#include "../include/cake.h"
#include "../include/compiler.h"
#include "../include/vm.h"

#include "../include/debug.h"

vm_t vm;

static void reset_stack(void){
	vm.stack_count = 0;
}

void reset_vm(void){
	vm.stack = NULL;
	vm.stack_capacity = 0;
	reset_stack();
}

void init_vm(void){
	reset_vm();
	vm.stack_capacity = 1024;
	vm.stack = GROW_ARRAY(value_t, vm.stack, 0, vm.stack_capacity);
}

static interpret_result run(void){
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op)\
	do{\
		double b = pop();\
		vm.stack[vm.stack_count - 1] = vm.stack[vm.stack_count - 1] op b;\
	}while(false)

	while (1){
#ifdef _CAKE_DEBUG
		char * prefix = "";
		for (value_t * slot = vm.stack; slot < (vm.stack + vm.stack_count); ++slot){
			printf("%s[", prefix);
			print_value(*slot);
			putchar(']');
			prefix = ", ";
		}
		putchar('\n');
		disassemble_instruction(vm.chunk, (uint64_t)(vm.ip - vm.chunk->bytes));
#endif
		uint8_t instruction;
		switch(instruction = READ_BYTE()){
			case OP_CONSTANT:
			case OP_CONSTANT_LONG:
				push(READ_CONSTANT());
				break;
			case OP_ADD:
				BINARY_OP(+);
				break;
			case OP_SUBTRACT:
				BINARY_OP(-);
				break;
			case OP_MULTIPLY:
				BINARY_OP(*);
				break;
			case OP_DIVIDE:
				BINARY_OP(/);
				break;
			case OP_REMAINDER:{
				int32_t b = pop();
				vm.stack[vm.stack_count - 1] = (int32_t)vm.stack[vm.stack_count - 1] % b;
				break;
			}
			case OP_MODULUS:{
				vm.stack[vm.stack_count - 1] = fabs(vm.stack[vm.stack_count - 1]);
				break;
			}
			case OP_INCREMENT:
				++vm.stack[vm.stack_count - 1];
				break;
			case OP_DECREMENT:
				--vm.stack[vm.stack_count - 1];
				break;
			case OP_NEGATE:
				vm.stack[vm.stack_count - 1] = -vm.stack[vm.stack_count - 1];
				break;
			case OP_RETURN:{
				print_value(pop());
				putchar('\n');
				return INTERPRET_OK;
			}
		}
	}

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

interpret_result interpret(const char * source){
	chunk_t chunk = init_chunk();

	if (!compile(source, &chunk)){
		free_chunk(&chunk);
		return INTERPRET_COMPILE_ERROR;
	}

	vm.chunk = &chunk;
	vm.ip = vm.chunk->bytes;

	interpret_result result = run();

	free_chunk(&chunk);
	return result;
}

void free_vm(void){
	FREE_ARRAY(value_t, vm.stack, vm.stack_capacity);
	reset_vm();
}

void push(value_t value){
	if (vm.stack_capacity < (vm.stack_count + 1)){
		uint64_t o_capacity = vm.stack_capacity;
		vm.stack_capacity = GROW_CAPACITY(o_capacity);
		vm.stack = GROW_ARRAY(value_t, vm.stack, o_capacity, vm.stack_capacity);
	}
	vm.stack[vm.stack_count] = value;
	++vm.stack_count;
}

value_t pop(void){
	--vm.stack_count;
	return vm.stack[vm.stack_count];
}
