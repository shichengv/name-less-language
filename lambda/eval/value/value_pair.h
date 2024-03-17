#pragma once

#include "../../framework.h"
#include "value_atom.h"

class ValuePair
{
public:
	ValuePair();
	ValuePair(ValuePair* pair);
	ValuePair(Value* front, Value* rear);
	~ValuePair();

	Value* front;
	Value* rear;

private:

};

