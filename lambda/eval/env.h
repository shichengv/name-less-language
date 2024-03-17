#pragma once

#include "../framework.h"
#include "./value/value_atom.h"

class Environment
{
public:
	Environment();
	Environment(Environment* parent);
	~Environment();

	// 父环境，如果在当前环境找不到就从他的父环境中寻找
	Environment* parent;
	// 环境符号表
	std::unordered_map<std::string, Value*> scope;
	// 临时数据序列，环境释放的过程中会自动删除这些实例
	std::vector<Value*> constant;
	// 局部环境序列
	std::vector<Environment*> env_sequence;
	// lambda实例序列，新建的lambda实例会添加到该列表，环境释放的过程中会自动删除这些实例
	std::vector<void*> sequence_value_lambda;

	// 布隆过滤器，判断函数

	// 操作方法
	Environment* lookup(std::string* name);
	Value* get(std::string* name);
	Value* def(std::string* name, Value* value);
	Value* set(std::string* name, Value* value);


private:

};
