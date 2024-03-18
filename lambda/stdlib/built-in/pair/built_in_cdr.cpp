#include "../built_in.h"

Value* built_in_cdr(std::vector<Value*>* args) 
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

		result = DBG_NEW Value(pair->rear->flag);
		result->value.bits = pair->rear->value.bits;
		pair->rear->flag = VALUE_IS_NIL;
	}

	return result;

}