#pragma once
#include "../../framework.h"
#include "../../eval/value/value.h"

enum ENUM_SYS_FUNC_ADDR
{
	SYS_GET_SYS_METRICS = 1,
};

Value* calls_stdlib_sys_func(uint64_t offset, std::vector<Value*>* args);

