#include "../built_in.h"

Value* built_in_pair(std::vector<Value*>* args)
{
	Value* front = args->at(0);
	Value* rear = args->at(1);
	ValuePair* pair = DBG_NEW ValuePair;

	pair->front = DBG_NEW Value(front->flag);
	pair->front->value.bits = front->value.bits;
	front->flag = VALUE_IS_NIL;

	pair->rear = DBG_NEW Value(rear->flag);
	pair->rear->value.bits = rear->value.bits;
	rear->flag = VALUE_IS_NIL;

	Value* result = DBG_NEW Value(VALUE_IS_PAIR);
	result->value.pair = pair;

	return result;
}