#pragma once

#include "../../framework.h"
#include "../../analyzer/token/token.h"

enum ENUM_VALUE
{
	VALUE_IS_NIL = 0,
	VALUE_IS_BITS,
	VALUE_IS_LOGICAL,
	VALUE_IS_NUM,
	VALUE_IS_STRING,
	VALUE_IS_LAMBDA,
	VALUE_IS_STD_FUNC_OFFSET,
	VALUE_IS_PAIR,
	VALUE_IS_TYPEID,
	VALUE_IS_REF,
};


class Value
{
public:
	Value();
	Value(const Value& value);
	//Value(const Value&& value);
	Value(uint8_t flag);
	Value(Value* constant);
	Value(Token* tok);
	~Value();

	// Value类型
	uint64_t flag : 8;
	// 引用数量
	uint64_t ref_count : 56;

	union
	{
		uint64_t bits;
		int64_t logical;
		double number;
		std::string* str;
		void* lambda;
		void* pair;
		// std_library_functions offset
		uint64_t offset;
		Value* ref;

	} value;

private:

};
