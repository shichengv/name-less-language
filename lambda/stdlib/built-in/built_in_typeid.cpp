#include "built_in.h"

Value* built_in_typeid(std::vector<Value*>* args)
{
	Value* result = DBG_NEW Value(VALUE_IS_TYPEID);
	result->value.bits = args->at(0)->flag;
	return result;
}