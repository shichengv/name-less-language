#include "../built_in.h"

Value* built_in_set_cdr(std::vector<Value*>* args)
{
	// ��ȡԴ���
	Value* ref_value = args->at(0);
	Value* value = ref_value->value.ref;
	ValuePair* value_pair = (ValuePair*)value->value.pair;

	// ��ȡβ����ֵ
	Value* new_value = args->at(1);

	// ɾ��Դ����
	delete value_pair->rear;

	value_pair->rear = DBG_NEW Value(new_value->flag);
	value_pair->rear->value.bits = new_value->value.bits;

	// ���Ĳ�����ǣ������С������
	new_value->flag = VALUE_IS_NIL;

	Value* result = DBG_NEW Value(VALUE_IS_LOGICAL);
	result->value.logical = 0x1;

	return result;

}