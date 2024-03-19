#include "../built_in.h"

Value* built_in_list(std::vector<Value*>* args)
{
	Value* result = NULL;
	if (args->size() == 0)
	{
		result = DBG_NEW Value(VALUE_IS_PAIR);

		ValuePair* root_pair = DBG_NEW ValuePair;
		root_pair->front = DBG_NEW Value(VALUE_IS_NIL);
		root_pair->rear = DBG_NEW Value(VALUE_IS_NIL);
		result->value.pair = root_pair;
	}
	else
	{

		result = DBG_NEW Value(VALUE_IS_NIL);
		ValuePair* last_pair = NULL;
		auto note_value = result;

		for (auto iter_value = args->begin(); iter_value != args->end(); iter_value++)
		{
			Value* value = *iter_value;

			note_value->flag = VALUE_IS_PAIR;
			auto tmp_pair = DBG_NEW ValuePair;
			note_value->value.pair = tmp_pair;
			last_pair = tmp_pair;

			tmp_pair->front = DBG_NEW Value(value->flag);
			tmp_pair->front->value.bits = value->value.bits;
			value->flag = VALUE_IS_NIL;

			tmp_pair->rear = DBG_NEW Value(VALUE_IS_NIL);
			note_value = tmp_pair->rear;
		}
	}

	return result;
}