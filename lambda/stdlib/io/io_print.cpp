#include "io.h"

static uint64_t print(Value* value)
{

	switch (value->flag)
	{
	case VALUE_IS_NIL:
		fflush(stdout);
		std::cout << "nil";
		break;
	case VALUE_IS_NUM:
		fflush(stdout);
		std::cout << value->value.number;
		break;
	case VALUE_IS_BITS:
		fflush(stdout);
		std::cout << value->value.bits;
		break;
	case VALUE_IS_LOGICAL:
		if (value->value.logical)
		{
			fflush(stdout);
			std::cout << "true";
		}
		else
		{
			fflush(stdout);
			std::cout << "false";
		}
		break;
	case VALUE_IS_STRING:
		std::cout << *value->value.str;
		break;
	case VALUE_IS_PAIR:
	{
		std::cout << "( ";
		ValuePair* pair = (ValuePair*)value->value.pair;
		auto p = [](Value* value) {
			if (value->flag == VALUE_IS_STRING)
			{
				std::cout << "\"";
				print(value);
				std::cout << "\"";
			}
			else
				print(value);
		};
		p(pair->front);
		std::cout << ", ";
		p(pair->rear);
		std::cout << " )";
		break;
	}
	default:
		break;
	}
	fflush(stdout);

	return 1;
}

Value* io_print(std::vector<Value*>* args_list)
{
	Value* result = DBG_NEW Value;
	if (args_list->size() != 1)
		throw "print function error...";
	Value* value = args_list->at(0);

	result->value.bits = print(value);
	return result;
}
