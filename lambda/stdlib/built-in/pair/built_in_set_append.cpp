#include "../built_in.h"

Value* built_in_set_append(std::vector<Value*>* args)
{
	Value* ref_value = args->at(0);

	// 源列表
	Value* value = ref_value->value.ref;


	Value* parameter = args->at(1);
	if (parameter->flag != VALUE_IS_NIL)
	{
		// 构造新的项
		Value* append_value = DBG_NEW Value(VALUE_IS_PAIR);
		ValuePair* new_pair = DBG_NEW ValuePair;
		append_value->value.pair = new_pair;

		Value* front_value = DBG_NEW Value(parameter->flag);
		front_value->value.bits = parameter->value.bits;
		new_pair->front = front_value;
		parameter->flag = VALUE_IS_NIL;


		// 列表中最后一个Pair结构的rear项
		ValuePair* note_pair = (ValuePair*)value->value.pair;
		Value* list_pair_rear = note_pair->rear;

		// 遍历序对结构，找到最后一个序对结构，它的rear项不为PAIR；
		for (; list_pair_rear->flag == VALUE_IS_PAIR; list_pair_rear = note_pair->rear)
			note_pair = (ValuePair*)list_pair_rear->value.pair;

		// 将新添加的序对的尾项指向源列表的尾项的NIL，避免拷贝
		new_pair->rear = note_pair->rear;
		// 链接到源列表中
		note_pair->rear = append_value;

	}


	Value* result = DBG_NEW Value(VALUE_IS_LOGICAL);
	result->value.logical = 0x1;

	return result;

}