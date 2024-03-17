#pragma once

#include "../../framework.h"
#include "token_atom.h"
#include "token_branch.h"

class TokenIf
{
public:
	TokenIf();
	~TokenIf();

	Token* cond;
	TokenBranch* branch;

private:

};

