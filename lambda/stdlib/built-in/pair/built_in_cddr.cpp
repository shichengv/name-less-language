#include "../built_in.h"

Value* built_in_cddr(std::vector<Value*>* args)
{

	Value* result = NULL;

	Value* value = args->at(0);
	if (value->flag != VALUE_IS_PAIR)
	{
		result = DBG_NEW Value(VALUE_IS_NIL);
	}
	else
	{
		ValuePair* pair = (ValuePair*)value->value.pair;

		Value* second_pair_value = pair->rear;
		ValuePair* second_pair = (ValuePair*)second_pair_value->value.pair;

		result = DBG_NEW Value(second_pair->rear->flag);
		result->value.bits = second_pair->rear->value.bits;
		second_pair->rear->flag = VALUE_IS_NIL;

	}

	return result;
}