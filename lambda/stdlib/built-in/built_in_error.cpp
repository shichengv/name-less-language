#include "built_in.h"

extern Value* io_print(std::vector<Value*>* args_list);

Value* built_in_error(std::vector<Value*>* args)
{
	io_print(args);
	exit(0);
}