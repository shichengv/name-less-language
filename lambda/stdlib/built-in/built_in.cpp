#include "built_in.h"

extern Value* built_in_append(std::vector<Value*>* args);
extern Value* built_in_set_append(std::vector<Value*>* args);
extern Value* built_in_caar(std::vector<Value*>* args);
extern Value* built_in_cadr(std::vector<Value*>* args);
extern Value* built_in_exit(std::vector<Value*>* args);
extern Value* built_in_error(std::vector<Value*>* args);
extern Value* built_in_set_car(std::vector<Value*>* args);
extern Value* built_in_set_cdr(std::vector<Value*>* args);
extern Value* built_in_cdr(std::vector<Value*>* args);
extern Value* built_in_car(std::vector<Value*>* args);
extern Value* built_in_pair(std::vector<Value*>* args);
extern Value* built_in_typeid(std::vector<Value*>* args);

Value* calls_stdlib_built_in_func(uint64_t offset, std::vector<Value*>* args)
{
	Value* result = NULL;
	switch (offset)
	{
	case BUILT_IN_EXIT:
		result = built_in_exit(args);
		break;
	case BUILT_IN_ERROR:
		result = built_in_error(args);
		break;
	case BUILT_IN_SET_CAR:
		result = built_in_set_car(args);
		break;
	case BUILT_IN_SET_CDR:
		result = built_in_set_cdr(args);
		break;
	case BUILT_IN_APPEND:
		result = built_in_append(args);
		break;
	case BUILT_IN_SET_APPEND:
		result = built_in_set_append(args);
		break;
	case BUILT_IN_CAR:
		result = built_in_car(args);
		break;
	case BUILT_IN_CAAR:
		result = built_in_caar(args);
		break;
	case BUILT_IN_CADR:
		result = built_in_cadr(args);
		break;
	case BUILT_IN_CDR:
		result = built_in_cdr(args);
		break;
	case BUILT_IN_PAIR:
		result = built_in_pair(args);
		break;
	case BUILT_IN_TYPEID:
		result = built_in_typeid(args);
		break;
	default:
		break;
	}
	return result;
}
