#include "../built_in.h"

Value* built_in_car(std::vector<Value*>* args)
{

	Value* result = NULL;
	Value* value = args->at(0);

	if (value->flag == VALUE_IS_NIL)
	{
		result = DBG_NEW Value(VALUE_IS_NIL);
	}
	else
	{
		ValuePair* pair = (ValuePair*)value->value.pair;

		result = DBG_NEW Value(pair->front->flag);
		result->value.bits = pair->front->value.bits;
		pair->front->flag = VALUE_IS_NIL;
	}

	return result;
}