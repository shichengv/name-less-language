
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
	// 清除当前环境的缓存数据
	std::for_each(constant.rbegin(), constant.rend(), [](Value* value) {
			delete value;
	});

	constant.erase(constant.begin(), constant.end());

	// 清除当前环境的符号
	std::unordered_map<std::string, Value*>::iterator scope_iter = scope.begin();
	while (scope_iter != scope.end())
	{
		Value* value = scope_iter->second;
		scope_iter = this->scope.erase(scope_iter);

		delete value;

	}

	// 清楚当前环境中所有子环境
	auto env_iter = env_sequence.begin();
	while (env_iter != env_sequence.end())
	{
		Environment* env = *env_iter;
		env_iter = env_sequence.erase(env_iter);
		delete env;
	}

	// 下面这种形式更简洁，不过懒~
	//std::for_each(sequence_value_lambda.begin(), sequence_value_lambda.end(), [](ValueLambda* value) {delete value; });

	// 清除当前环境的所有lambda表达式
	auto seq_lambda = sequence_value_lambda.begin();
	while (seq_lambda != sequence_value_lambda.end())
	{
		ValueLambda* value = (ValueLambda*)*seq_lambda;
		seq_lambda = sequence_value_lambda.erase(seq_lambda);

		delete value;
	}

}

/*

例程描述：
	set 用来更改一个变量的值。

		该例程首先通过变量名来在该环境以及该环境的父环境序列查找该变量，如果未找到该变量
	那么就在该环境定义该变量。如果找到了该变量，并且该变量所属的环境就是当前环境，重新设
	置该值。否则就调用找到的那个环境的set方法，重新设置改变量。
	set 例程简单的拷贝value参数，调用时需要注意传入参数的内存回收
*/
Value* Environment::set(std::string* name, Value* value)
{

	Environment* where_env = lookup(name);
	Value* data = NULL;
	if (where_env)
	{
		// 如果该变量就存在于当前环境
		if (reinterpret_cast<uint64_t>(where_env) == reinterpret_cast<uint64_t>(this))
		{
			data = DBG_NEW Value(value);
			Value* original = where_env->get(name);
			// 自计算的值可以被强制重新定义
			if (original->flag != VALUE_IS_LAMBDA)
			{
				this->scope[*name] = data;
				// 删除原先在环境中的数据
				delete original;
			}
			else
				throw* name + "variable can't redefine...";
		}
		else
			// 去到变量存在的环境中更新该变量
			data = where_env->set(name, value);
	}
	else 
	{
		// 定义该变量
		data = DBG_NEW Value(value);
		this->scope[*name] = data;
	}

	return data;
}
/*

例程描述：
	def 用来定义一个新的变量，def允许重定义，这时候def的语义等于set[仅作用于当前局部环境]

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
			// 去父环境中寻找
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
		// 拷贝位向量
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
		// 拷贝位向量
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
		// 不需要设置为bits，存储数字的字符串需要被释放
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
	// 对于flag == VALUE_IS_LAMBDA 这种情况，这里不进行回收，由当前环境的父环境进行管理

	switch (flag)
	{
	case VALUE_IS_STRING:
		// 删除字符串对象
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