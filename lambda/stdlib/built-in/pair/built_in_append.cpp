#include "../built_in.h"

Value* built_in_append(std::vector<Value*>* args)
{

	// Դ�б��copy
	Value* value = args->at(0);

	// �����µ���
	Value* append_value = DBG_NEW Value(VALUE_IS_PAIR);
	ValuePair* new_pair = DBG_NEW ValuePair;
	append_value->value.pair = new_pair;

	Value* parameter = args->at(1);
	Value* front_value = DBG_NEW Value(parameter->flag);
	front_value->value.bits = parameter->value.bits;
	new_pair->front = front_value;
	parameter->flag = VALUE_IS_NIL;

	// �б������һ��Pair�ṹ��rear��
	ValuePair* note_pair = (ValuePair*)value->value.pair;
	Value* list_pair_rear = note_pair->rear;

	// ������Խṹ���ҵ����һ����Խṹ������rear�ΪPAIR��
	for (; list_pair_rear->flag == VALUE_IS_PAIR; list_pair_rear = note_pair->rear)
		note_pair = (ValuePair*)list_pair_rear->value.pair;

	// ������ӵ���Ե�β��ָ��Դ�б��β���NIL�����⿽��
	new_pair->rear = note_pair->rear;
	// ���ӵ�Դ�б���
	note_pair->rear = append_value;

	Value* result = DBG_NEW Value(VALUE_IS_PAIR);
	// ���������������ظ�����
	result->value.bits = value->value.bits;

	value->flag = VALUE_IS_NIL;

	return result;

}