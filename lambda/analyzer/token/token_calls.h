#pragma once

#include "../../framework.h"
#include "token_atom.h"

class TokenCalls
{
public:
	TokenCalls();
	~TokenCalls();

	Token* func;
	std::vector<Token*>* args;

private:

};
