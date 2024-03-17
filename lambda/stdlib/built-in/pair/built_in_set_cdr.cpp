#include "../built_in.h"

Value* built_in_set_cdr(std::vector<Value*>* args)
{
	Value* ref_value = args->at(0);

	Value* value = ref_value->value.ref;
	Value* new_value = args->at(1);

	ValuePair* value_pair = (ValuePair*)value->value.pair;
	// É¾³ýÔ´Êý¾Ý
	delete value_pair->rear;

	value_pair->rear = DBG_NEW Value(new_value);

	Value* result = DBG_NEW Value(VALUE_IS_LOGICAL);
	result->value.logical = 0x1;

	return result;

}