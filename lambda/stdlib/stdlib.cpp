#include "stdlib.h"
#include "built-in/built_in.h"
#include "io/io.h"

/*

例程描述：
	注册库函数指定环境中，当环境被销毁时，这些函数也会被销毁

*/
static void register_func(std::unordered_map<std::string, Value*>* scope, const char* funcname, uint32_t stdlib_offset, uint32_t func_offset)
{
	std::string func_name;

	func_name = funcname;
	Value* func = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
	func->value.bits = stdlib_offset;
	func->value.bits <<= 32;
	func->value.bits |= func_offset;
	(*scope)[func_name] = func;

}

void stdlib_init(std::unordered_map<std::string, Value*>* scope)
{
		register_func(scope, "print", STDLIB_IO, IO_PRINT);

		register_func(scope, "error", STDLIB_BUILT_IN, BUILT_IN_ERROR);
		register_func(scope, "exit", STDLIB_BUILT_IN, BUILT_IN_EXIT);

		register_func(scope, "list", STDLIB_BUILT_IN, BUILT_IN_LIST);
		register_func(scope, "pair", STDLIB_BUILT_IN, BUILT_IN_PAIR);
		register_func(scope, "car", STDLIB_BUILT_IN, BUILT_IN_CAR);
		register_func(scope, "caar", STDLIB_BUILT_IN, BUILT_IN_CAAR);
		register_func(scope, "cadr", STDLIB_BUILT_IN, BUILT_IN_CADR);
		register_func(scope, "cddr", STDLIB_BUILT_IN, BUILT_IN_CDDR);
		register_func(scope, "cdr", STDLIB_BUILT_IN, BUILT_IN_CDR);
		register_func(scope, "append", STDLIB_BUILT_IN, BUILT_IN_APPEND);
		register_func(scope, "append!", STDLIB_BUILT_IN, BUILT_IN_SET_APPEND);
		register_func(scope, "set-car!", STDLIB_BUILT_IN, BUILT_IN_SET_CAR);
		register_func(scope, "set-cdr!", STDLIB_BUILT_IN, BUILT_IN_SET_CDR);

		register_func(scope, "number?", STDLIB_BUILT_IN, BUILT_IN_IS_NUMBER);
		register_func(scope, "string?", STDLIB_BUILT_IN, BUILT_IN_IS_STRING);
		register_func(scope, "pair?", STDLIB_BUILT_IN, BUILT_IN_IS_PAIR);

		register_func(scope, "typeid", STDLIB_BUILT_IN, BUILT_IN_TYPEID);

}

void stdlib_clean()
{

}

Value* calls_stdlib_func(uint64_t func_addr, std::vector<Value*>* args_list)
{
	Value* stdlib_result = NULL;
	switch (GET_STDLIB_INDEX(func_addr))
	{
	case STDLIB_BUILT_IN:
		stdlib_result = calls_stdlib_built_in_func(GET_FUNC_INDEX(func_addr), args_list);
		break;
	case STDLIB_IO:
		stdlib_result = calls_stdlib_io_func(GET_FUNC_INDEX(func_addr), args_list);
		break;
	default:
		break;
	}
	return stdlib_result;
}
