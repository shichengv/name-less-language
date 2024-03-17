#pragma once

#include "../../framework.h"
#include "token_atom.h"

class TokenLambda
{
public:
	TokenLambda();
	~TokenLambda();

	std::vector<std::string*>* args;
	Token* body;

private:

};
