
#include "env.h"
#include "value/value.h"

Environment::Environment() :parent(NULL)
{

}
Environment::Environment(Environment* parent)
{
	this->parent = parent;
}

Environment::~Environment()
{
	// �����ǰ�����Ļ�������
	std::for_each(constant.rbegin(), constant.rend(), [](Value* value) {
			delete value;
	});

	constant.erase(constant.begin(), constant.end());

	// �����ǰ�����ķ���
	std::unordered_map<std::string, Value*>::iterator scope_iter = scope.begin();
	while (scope_iter != scope.end())
	{
		Value* value = scope_iter->second;
		scope_iter = this->scope.erase(scope_iter);

		delete value;

	}

	// �����ǰ�����������ӻ���
	auto env_iter = env_sequence.begin();
	while (env_iter != env_sequence.end())
	{
		Environment* env = *env_iter;
		env_iter = env_sequence.erase(env_iter);
		delete env;
	}

	// ����������ʽ����࣬������~
	//std::for_each(sequence_value_lambda.begin(), sequence_value_lambda.end(), [](ValueLambda* value) {delete value; });

	// �����ǰ����������lambda���ʽ
	auto seq_lambda = sequence_value_lambda.begin();
	while (seq_lambda != sequence_value_lambda.end())
	{
		ValueLambda* value = (ValueLambda*)*seq_lambda;
		seq_lambda = sequence_value_lambda.erase(seq_lambda);

		delete value;
	}

}

/*

����������
	set ��������һ��������ֵ��

		����������ͨ�����������ڸû����Լ��û����ĸ��������в��Ҹñ��������δ�ҵ��ñ���
	��ô���ڸû�������ñ���������ҵ��˸ñ��������Ҹñ��������Ļ������ǵ�ǰ������������
	�ø�ֵ������͵����ҵ����Ǹ�������set�������������øı�����
	set ���̼򵥵Ŀ���value����������ʱ��Ҫע�⴫��������ڴ����
*/
Value* Environment::set(std::string* name, Value* value)
{

	Environment* where_env = lookup(name);
	Value* data = NULL;
	if (where_env)
	{
		// ����ñ����ʹ����ڵ�ǰ����
		if (reinterpret_cast<uint64_t>(where_env) == reinterpret_cast<uint64_t>(this))
		{
			data = DBG_NEW Value(value);
			Value* original = where_env->get(name);
			// �Լ����ֵ���Ա�ǿ�����¶���
			if (original->flag != VALUE_IS_LAMBDA)
			{
				this->scope[*name] = data;
				// ɾ��ԭ���ڻ����е�����
				delete original;
			}
			else
				throw* name + "variable can't redefine...";
		}
		else
			// ȥ���������ڵĻ����и��¸ñ���
			data = where_env->set(name, value);
	}
	else 
	{
		// ����ñ���
		data = DBG_NEW Value(value);
		this->scope[*name] = data;
	}

	return data;
}
/*

����������
	def ��������һ���µı�����def�����ض��壬��ʱ��def���������set[�������ڵ�ǰ�ֲ�����]

*/
Value* Environment::def(std::string* name, Value* value)
{
	Value* data = DBG_NEW Value(value);
	this->scope[*name] = data;
	//delete value;
	return data;
}

Value* Environment::get(std::string* name)
{
	if (this->scope.find(*name) != this->scope.end())
	{
		return scope[*name];
	}
	else if (parent)
	{
		return parent->get(name);
	}
	throw "Undefined variable: " + *name;
}

Environment* Environment::lookup(std::string* name)
{
	if (this->scope.find(*name) == this->scope.end())
	{
		if (this->parent)
		{
			// ȥ��������Ѱ��
			return this->parent->lookup(name);
		}
		return NULL;
	}
	return this;
}


Value::Value()
{
	flag = VALUE_IS_BITS;
	ref_count = 1;
	value.bits = 0;
}

Value::Value(const Value& value)
{
	this->flag = value.flag;
	this->ref_count = 1;
	//this->ref_count = value.ref_count;

	if (value.flag == VALUE_IS_STRING)
	{
		this->value.str = DBG_NEW std::string(*value.value.str);
	}
	else if (value.flag == VALUE_IS_PAIR)
	{
		this->value.pair = DBG_NEW ValuePair((ValuePair*)value.value.pair);
	}
	else
	{
		// ����λ����
		this->value.bits = value.value.bits;
	}
}

Value::Value(uint8_t flag)
{
	this->flag = flag;
	ref_count = 1;
	value.bits = 0;
}

Value::Value(Value* value)
{
	ref_count = 1;
	this->flag = value->flag;

	if (value->flag == VALUE_IS_STRING)
	{
		this->value.str = DBG_NEW std::string(*value->value.str);
	}
	else if (value->flag == VALUE_IS_PAIR)
	{
		this->value.pair = DBG_NEW ValuePair((ValuePair*)value->value.pair);
	}
	else
	{
		// ����λ����
		this->value.bits = value->value.bits;
	}
}
Value::Value(Token* tok)
{
	ref_count = 1;
	switch (tok->flag)
	{
	case TOKEN_FLAG_IS_STRING:
		this->flag = VALUE_IS_STRING;
		this->value.str = DBG_NEW std::string(*tok->value.str);
		break;

	case TOKEN_FLAG_IS_NUM:
		this->flag = VALUE_IS_NUM;
		this->value.number = std::stod(*tok->value.str);
		// ����Ҫ����Ϊbits���洢���ֵ��ַ�����Ҫ���ͷ�
		break;
	case TOKEN_FLAG_IS_BOOL:
		this->flag = VALUE_IS_LOGICAL;
		this->value.logical = tok->value.bits;
		break;
	default:
		throw "Value Object can't inherit from this Token Object...";
		break;
	}
}

Value::~Value()
{
	// ����flag == VALUE_IS_LAMBDA ������������ﲻ���л��գ��ɵ�ǰ�����ĸ��������й���

	switch (flag)
	{
	case VALUE_IS_STRING:
		// ɾ���ַ�������
		delete this->value.str;
		break;
	case VALUE_IS_PAIR:
	{
		ValuePair* pair = (ValuePair*)this->value.pair;
		delete pair;
		break;
	}
	default:
		break;
	}

}

ValuePair::ValuePair() : front(NULL), rear(NULL)
{
}

ValuePair::ValuePair(ValuePair* pair)
{
	front = DBG_NEW Value(pair->front);
	rear = DBG_NEW Value(pair->rear);
}

ValuePair::ValuePair(Value* front, Value* rear)
{
	this->front = DBG_NEW Value(front);
	this->rear = DBG_NEW Value(rear);
}

ValuePair::~ValuePair()
{
	if (front)
	{
		delete front;
	}
	if (rear)
	{
		delete rear;
	}
}

ValueLambda::ValueLambda() : env(NULL), func_ptr(NULL)
{
}

ValueLambda::~ValueLambda()
{
}