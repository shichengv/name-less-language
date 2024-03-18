#include "../built_in.h"

Value* built_in_set_car(std::vector<Value*>* args)
{
	// 获取源序对
	Value* ref_value = args->at(0);
	Value* value = ref_value->value.ref;
	ValuePair* value_pair = (ValuePair*)value->value.pair;

	// 获取尾项新值
	Value* new_value = args->at(1);

	// 删除源数据
	delete value_pair->front;

	value_pair->front = DBG_NEW Value(new_value->flag);
	value_pair->front->value.bits = new_value->value.bits;

	// 更改参数标记，不进行“深”回收
	new_value->flag = VALUE_IS_NIL;

	Value* result = DBG_NEW Value(VALUE_IS_LOGICAL);
	result->value.logical = 0x1;

	return result;
}