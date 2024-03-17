#pragma once

#include "../../framework.h"
#include "token_lrvalue.h"

class TokenAssign
{
public:
	TokenAssign();
	~TokenAssign();

	std::string* op;
	TokenLRvalue* lrvalue;
	

private:

};
