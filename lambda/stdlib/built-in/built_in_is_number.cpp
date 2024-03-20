#include "built_in.h"

Value* built_in_is_number(std::vector<Value*>* args)
{
	Value* result = DBG_NEW Value(VALUE_IS_LOGICAL);
	result->value.logical = (args->at(0)->flag == VALUE_IS_NUM);
	return result;
}