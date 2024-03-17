#include "../built_in.h"

Value* built_in_pair(std::vector<Value*>* args)
{
	Value* front = args->at(0);
	Value* rear = args->at(1);
	ValuePair* pair = DBG_NEW ValuePair(front, rear);
	Value* result = DBG_NEW Value(VALUE_IS_PAIR);
	result->value.pair = pair;

	return result;
}