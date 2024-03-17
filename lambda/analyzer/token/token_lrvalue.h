#pragma once

#include "../../framework.h"
#include "token_atom.h"

class TokenLRvalue
{
public:
	TokenLRvalue();
	~TokenLRvalue();

	Token* left;
	Token* right;

private:

};
