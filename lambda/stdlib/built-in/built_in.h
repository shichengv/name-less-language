#pragma once

#include "../../framework.h"
#include "../../eval/value/value.h"

enum ENUM_BUILT_IN_FUNC
{
	BUILT_IN_NULL = 0,
	BUILT_IN_EXIT,
	BUILT_IN_ERROR,
	BUILT_IN_TYPEID,
	BUILT_IN_PAIR,
	BUILT_IN_CAR,
	BUILT_IN_CAAR,
	BUILT_IN_CADR,
	BUILT_IN_CDR,
	BUILT_IN_SET_CAR,
	BUILT_IN_SET_CDR,
};

Value* calls_stdlib_built_in_func(uint64_t offset, std::vector<Value*>* args);
