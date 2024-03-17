#pragma once
#include "../framework.h"
#include "../eval/value/value.h"

enum ENUM_STDLIB_FUNC
{
	STDLIB_BUILT_IN = 0,
	STDLIB_IO,
	STDLIB_FS,
};

#define GET_STDLIB_INDEX(func_addr)				(func_addr >> 32)
#define GET_FUNC_INDEX(func_addr)				(func_addr << 32 >> 32)

void stdlib_init(std::unordered_map<std::string, Value*>* scope);
void stdlib_clean();
Value* calls_stdlib_func(uint64_t func_addr, std::vector<Value*>* args_list);
