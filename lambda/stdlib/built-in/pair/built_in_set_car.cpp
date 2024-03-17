#include "../built_in.h"

Value* built_in_set_car(std::vector<Value*>* args)
{
	Value* ref_value = args->at(0);

	Value* origianl_value = ref_value->value.ref;
	Value* new_pair = args->at(1);

	ValuePair* pair = (ValuePair*)origianl_value->value.pair;
	// É¾³ýÔ´Êý¾Ý
	delete pair->front;

	pair->front = DBG_NEW Value(new_pair);

	Value* result = DBG_NEW Value(VALUE_IS_LOGICAL);
	result->value.logical = 0x1;

	return result;
}