#include "../built_in.h"
#include "../../../configure/error_log.h"

Value* built_in_caar(std::vector<Value*>* args)
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

		Value* second_pair_value = pair->front;
		ValuePair* second_pair = (ValuePair*)second_pair_value->value.pair;

		result = DBG_NEW Value(second_pair->front->flag);
		result->value.bits = second_pair->front->value.bits;
		second_pair->front->flag = VALUE_IS_NIL;

	}

	return result;
}