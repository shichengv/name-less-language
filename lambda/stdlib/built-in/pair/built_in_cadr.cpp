#include "../built_in.h"
#include "../../../configure/error_log.h"

Value* built_in_cadr(std::vector<Value*>* args)
{

	Value* result = NULL;

	Value* value = args->at(0);
	if (value->flag != VALUE_IS_PAIR)
	{
		result = DBG_NEW Value(VALUE_IS_NIL);
		// ERROR
		std::wstring error = L"\x63\x61\x64\x72\x20\x51fd\x6570\x53ea\x80fd\x5bf9\x20\x70\x61\x69\x72\x20\x7c7b\x578b\x7684\x6570\x636e\x8fdb\x884c\x64cd\x4f5c\x54e6\xff5e\x28\x2220\x30fb\x3c9\x3c\x20\x29\x2312\x266a\x7e\xff0c\x7f5a\x4f60\x53bb\x6284\x5e93\x51fd\x6570\x624b\x518c\xff01";
		throw_eval_exception(error);
	}
	else
	{
		ValuePair* pair = (ValuePair*)value->value.pair;
		Value* second_pair_value = pair->rear;
		ValuePair* second_pair = (ValuePair*)second_pair_value->value.pair;
		result = DBG_NEW Value(second_pair->front);
	}

	return result;
}