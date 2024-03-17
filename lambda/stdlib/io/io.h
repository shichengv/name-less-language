#pragma once

#include "../../framework.h"
#include "../../eval/value/value.h"

enum ENUM_IO_FUNC_ADDR
{
	IO_PRINT = 1,
};

Value* calls_stdlib_io_func(uint64_t offset, std::vector<Value*>* args);
