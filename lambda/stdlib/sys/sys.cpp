#include "sys.h"

extern Value* sys_get_sys_metrics(std::vector<Value*>* args);

Value* calls_stdlib_sys_func(uint64_t offset, std::vector<Value*>* args)
{
	Value* result = NULL;
	switch (offset)
	{
	case SYS_GET_SYS_METRICS:
		result = sys_get_sys_metrics(args);
	default:
		break;
	}
	return result;
}

