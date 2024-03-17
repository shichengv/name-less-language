#include "../built_in.h"
#include "../../configure/error_log.h"

Value* built_in_caar(std::vector<Value*>* args)
{

	Value* result = NULL;

	Value* value = args->at(0);
	if (value->flag != VALUE_IS_PAIR)
	{
		result = DBG_NEW Value(VALUE_IS_NIL);
		// ERROR
		std::wstring error = L"caar 函数只能对 pair 类型的数据进行操作哦\xff5e\x28\x2220\x30fb\x3c9\x3c\x20\x29\x2312\x266a~，罚你去抄库函数手册！";
		throw_eval_exception(error);
	}
	else
	{
		ValuePair* pair = (ValuePair*)value->value.pair;
		Value* second_pair_value = pair->front;
		ValuePair* second_pair = (ValuePair*)second_pair_value->value.pair;
		result = DBG_NEW Value(second_pair->front);
	}

	return result;
}