#include "built_in.h"

Value* built_in_exit(std::vector<Value*>* args)
{
	exit(0);
}