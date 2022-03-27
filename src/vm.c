#include "../include/cake.h"
#include "../include/compiler.h"
#include "../include/vm.h"
#include "../include/table.h"

#include "../include/object.h"
#include "../include/debug.h"

vm_t vm;

static void reset_stack(void){
	vm.stack_count = 0;
}

void reset_vm(void){
	vm.stack = NULL;
	reset_stack();
	vm.stack_capacity = 0;

	vm.strings = init_table();
	vm.objects = NULL;
}

void init_vm(void){
	reset_vm();
	vm.stack_capacity = 1024;
	vm.stack = GROW_ARRAY(value_t, vm.stack, 0, vm.stack_capacity);
}

static bool is_false(value_t value){
	return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate(void){
	/* TODO: Optimize */
	ostr_t * b = AS_STRING(pop());
	ostr_t * a = AS_STRING(pop());

	uint64_t length = a->length + b->length;
	char * buffer = ALLOCATE(char, length + 1);

	memcpy(buffer, a->buffer, a->length);
	memcpy(buffer + a->length, b->buffer, b->length);
	buffer[length] = '\0';

	ostr_t * result = copy_str(true, buffer, length);
	push(OBJ_VAL(result));
}

static value_t peek(int64_t distance){
	return vm.stack[vm.stack_count -1 - distance];
}

#define runtime_error(msg)\
	do{\
		MEL_log(LOG_ERROR, "[line %lu]: %s", vm.chunk->lines[vm.ip - vm.chunk->bytes - 1], msg);\
		reset_stack();\
	}while(false);

static interpret_result run(void){
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(type, op)\
	do{\
		if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))){\
			runtime_error("Operand must be a number.");\
			return INTERPRET_RUNTIME_ERROR;\
		}\
		value_t b = pop();\
		vm.stack[vm.stack_count - 1] =\
			type(AS_NUMBER(vm.stack[vm.stack_count - 1]) op AS_NUMBER(b));\
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
			case OP_NIL:
				push(NIL_VAL);
				break;
			case OP_TRUE:
				push(BOOL_VAL(true));
				break;
			case OP_FALSE:
				push(BOOL_VAL(false));
				break;
			case OP_NOT:
				vm.stack[vm.stack_count - 1] = BOOL_VAL(is_false(vm.stack[vm.stack_count - 1]));
				break;
			case OP_NOT_EQUAL:{
				value_t b = pop();
				vm.stack[vm.stack_count - 1] = BOOL_VAL(!values_equal(vm.stack[vm.stack_count - 1], b));
				break;
			}
			case OP_EQUAL:{
				value_t b = pop();
				vm.stack[vm.stack_count - 1] = BOOL_VAL(values_equal(vm.stack[vm.stack_count - 1], b));
				break;
			}
			case OP_GREATER:
				BINARY_OP(BOOL_VAL, >);
				break;
			case OP_GREATER_EQUAL:
				BINARY_OP(BOOL_VAL, >=);
				break;
			case OP_LESS:
				BINARY_OP(BOOL_VAL, <);
				break;
			case OP_LESS_EQUAL:
				BINARY_OP(BOOL_VAL, <=);
				break;
			case OP_ADD:{
				if (IS_STRING(peek(0)) && IS_STRING(peek(1))){
					concatenate();
				}else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))){
					value_t b = pop();
					vm.stack[vm.stack_count - 1] =\
						NUMBER_VAL(AS_NUMBER(vm.stack[vm.stack_count - 1]) + AS_NUMBER(b));
				}else{
					runtime_error("Invalid operands.");
					return INTERPRET_RUNTIME_ERROR;
				}
				break;
			}
			case OP_SUBTRACT:
				BINARY_OP(NUMBER_VAL, -);
				break;
			case OP_MULTIPLY:
				BINARY_OP(NUMBER_VAL, *);
				break;
			case OP_DIVIDE:
				BINARY_OP(NUMBER_VAL, /);
				break;
			case OP_REMAINDER:{
				if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))){
					runtime_error("Operand must be a number.");
					return INTERPRET_RUNTIME_ERROR;
				}
				value_t b = pop();
				vm.stack[vm.stack_count - 1] =\
					NUMBER_VAL((int32_t)AS_NUMBER(vm.stack[vm.stack_count - 1]) % (int32_t)AS_NUMBER(b));
				break;
			}
			case OP_MODULUS:{
				if (!IS_NUMBER(peek(0))){
					runtime_error("Operand must be a number.");
					return INTERPRET_RUNTIME_ERROR;
				}
				vm.stack[vm.stack_count - 1] = NUMBER_VAL(fabs(AS_NUMBER(vm.stack[vm.stack_count - 1])));
				break;
			}
			case OP_INCREMENT:
				if (!IS_NUMBER(peek(0))){
					runtime_error("Operand must be a number.");
					return INTERPRET_RUNTIME_ERROR;
				}
				++AS_NUMBER(vm.stack[vm.stack_count - 1]);
				break;
			case OP_DECREMENT:
				if (!IS_NUMBER(peek(0))){
					runtime_error("Operand must be a number.");
					return INTERPRET_RUNTIME_ERROR;
				}
				--AS_NUMBER(vm.stack[vm.stack_count - 1]);
				break;
			case OP_NEGATE:
				if (!IS_NUMBER(peek(0))){
					runtime_error("Operand must be a number.");
					return INTERPRET_RUNTIME_ERROR;
				}
				vm.stack[vm.stack_count - 1] = NUMBER_VAL(-AS_NUMBER(vm.stack[vm.stack_count - 1]));
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
	free_objects();
	free_table(&vm.strings);
	reset_vm();
}

bool values_equal(value_t val1, value_t val2){
	if (val1.type != val2.type){
		return false;
	}

	switch(val1.type){
		case VAL_NIL:
			return true;
		case VAL_BOOL:
			return AS_BOOL(val1) == AS_BOOL(val2);
		case VAL_NUMBER:
			return AS_NUMBER(val1) == AS_NUMBER(val2);
		case VAL_OBJ:
			return AS_OBJ(val1) == AS_OBJ(val2);
		default:
			return false;
	}
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
