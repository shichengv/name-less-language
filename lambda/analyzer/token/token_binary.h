#pragma once
#include "../../framework.h"
#include "token_atom.h"
#include "token_lrvalue.h"

class TokenBinary
{
public:
	TokenBinary();
	~TokenBinary();

	std::string* op;
	TokenLRvalue* lrvalue;

private:

};
