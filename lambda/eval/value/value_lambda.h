#pragma once

#include "../../framework.h"
#include "../../analyzer/token/token.h"
#include "../env.h"

class ValueLambda
{
public:
	ValueLambda();
	~ValueLambda();

	Environment* env;
	TokenLambda* func_ptr;

private:

};
