#include "io.h"

extern Value* io_print(std::vector<Value*>* args_list);

Value* calls_stdlib_io_func(uint64_t offset, std::vector<Value*>* args)
{
	Value* result = NULL;
	switch (offset)
	{
	case IO_PRINT:
		result = io_print(args);
	default:
		break;
	}
	return result;
}
