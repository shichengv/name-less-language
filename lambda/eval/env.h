#pragma once

#include "../framework.h"
#include "./value/value_atom.h"

class Environment
{
public:
	Environment();
	Environment(Environment* parent);
	~Environment();

	// ������������ڵ�ǰ�����Ҳ����ʹ����ĸ�������Ѱ��
	Environment* parent;
	// �������ű�
	std::unordered_map<std::string, Value*> scope;
	// ��ʱ�������У������ͷŵĹ����л��Զ�ɾ����Щʵ��
	std::vector<Value*> constant;
	// �ֲ���������
	std::vector<Environment*> env_sequence;
	// lambdaʵ�����У��½���lambdaʵ������ӵ����б������ͷŵĹ����л��Զ�ɾ����Щʵ��
	std::vector<void*> sequence_value_lambda;

	// ��¡���������жϺ���

	// ��������
	Environment* lookup(std::string* name);
	Value* get(std::string* name);
	Value* def(std::string* name, Value* value);
	Value* set(std::string* name, Value* value);


private:

};
