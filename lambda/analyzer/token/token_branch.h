#pragma once

#include "../../framework.h"
#include "token_atom.h"

class TokenBranch
{
public:
	TokenBranch();
	~TokenBranch();

	Token* then_branch;
	Token* else_branch;

private:

};
