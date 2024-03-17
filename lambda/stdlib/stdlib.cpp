#include "stdlib.h"
#include "built-in/built_in.h"
#include "io/io.h"

static inline void construct_func_addr(Value* value, uint32_t stdlib_offset, uint32_t func_offset)
{
	value->value.bits = stdlib_offset;
	value->value.bits <<= 32;
	value->value.bits |= func_offset;
}

void stdlib_init(std::unordered_map<std::string, Value*>* scope)
{
		std::string func_name;

		Value* io_print = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
		construct_func_addr(io_print, STDLIB_IO, IO_PRINT);
		func_name = "print";
		(*scope)[func_name] = io_print;

		Value* built_in_error = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
		construct_func_addr(built_in_error, STDLIB_BUILT_IN, BUILT_IN_ERROR);
		func_name = "error";
		(*scope)[func_name] = built_in_error;

		Value* built_in_exit = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
		construct_func_addr(built_in_exit, STDLIB_BUILT_IN, BUILT_IN_EXIT);
		func_name = "exit";
		(*scope)[func_name] = built_in_exit;

		Value* built_in_pair = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
		construct_func_addr(built_in_pair, STDLIB_BUILT_IN, BUILT_IN_PAIR);
		func_name = "pair";
		(*scope)[func_name] = built_in_pair;

		Value* built_in_car = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
		construct_func_addr(built_in_car, STDLIB_BUILT_IN, BUILT_IN_CAR);
		func_name = "car";
		(*scope)[func_name] = built_in_car;

		Value* built_in_caar = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
		construct_func_addr(built_in_caar, STDLIB_BUILT_IN, BUILT_IN_CAAR);
		func_name = "caar";
		(*scope)[func_name] = built_in_caar;

		Value* built_in_cadr = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
		construct_func_addr(built_in_cadr, STDLIB_BUILT_IN, BUILT_IN_CADR);
		func_name = "cadr";
		(*scope)[func_name] = built_in_cadr;

		Value* built_in_cdr = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
		construct_func_addr(built_in_cdr, STDLIB_BUILT_IN, BUILT_IN_CDR);
		func_name = "cdr";
		(*scope)[func_name] = built_in_cdr;


		Value* built_in_set_car = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
		construct_func_addr(built_in_set_car, STDLIB_BUILT_IN, BUILT_IN_SET_CAR);
		func_name = "set-car!";
		(*scope)[func_name] = built_in_set_car;

		Value* built_in_set_cdr = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
		construct_func_addr(built_in_set_cdr, STDLIB_BUILT_IN, BUILT_IN_SET_CDR);
		func_name = "set-cdr!";
		(*scope)[func_name] = built_in_set_cdr;

		Value* built_in_typeid = DBG_NEW Value(VALUE_IS_STD_FUNC_OFFSET);
		construct_func_addr(built_in_typeid, STDLIB_BUILT_IN, BUILT_IN_TYPEID);
		func_name = "typeid";
		(*scope)[func_name] = built_in_typeid;

}

void stdlib_clean()
{
	//delete stdlib_funcs;
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
