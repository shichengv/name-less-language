#include "../built_in.h"

Value* built_in_cdr(std::vector<Value*>* args) 
{
	Value* result = NULL;
	Value* value = args->at(0);
	ValuePair* pair = (ValuePair*)value->value.pair;

	if (pair->rear->flag == VALUE_IS_NIL)
		result = DBG_NEW Value(VALUE_IS_NIL);
	else
		result = DBG_NEW Value(pair->rear);

	return result;

}