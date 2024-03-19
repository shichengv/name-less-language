#include "token.h"

Token::Token()
{
	flag = 0;
	memset(type, 0, 7);
	value.bits = 0;
}
Token::Token(uint8_t to_flag)
{
	switch (to_flag)
	{
	case TOKEN_FLAG_IS_BITS:
		SET_TOKEN_TYPE(*this, "bits");
		break;
	case TOKEN_FLAG_IS_BOOL:
		SET_TOKEN_TYPE(*this, "bool");
		break;
	case TOKEN_FLAG_IS_NUM:
		SET_TOKEN_TYPE(*this, "num");
		break;
	case TOKEN_FLAG_IS_STRING:
		SET_TOKEN_TYPE(*this, "str");
		break;
	case TOKEN_FLAG_IS_ASSIGN:
		SET_TOKEN_TYPE(*this, "assign");
		break;
	case TOKEN_FLAG_IS_BINARY:
		SET_TOKEN_TYPE(*this, "binary");
		break;
	case TOKEN_FLAG_IS_PROG:
		SET_TOKEN_TYPE(*this, "prog");
		break;
	case TOKEN_FLAG_IS_IF:
		SET_TOKEN_TYPE(*this, "if");
		break;
	case TOKEN_FLAG_IS_CALLS:
		SET_TOKEN_TYPE(*this, "calls");
		break;
	case TOKEN_FLAG_IS_PUNC:
		SET_TOKEN_TYPE(*this, "punc");
		break;
	case TOKEN_FLAG_IS_KW:
		SET_TOKEN_TYPE(*this, "kw");
		break;
	case TOKEN_FLAG_IS_VAR:
		SET_TOKEN_TYPE(*this, "var");
		break;
	case TOKEN_FLAG_IS_OP:
		SET_TOKEN_TYPE(*this, "op");
		break;
	case TOKEN_FLAG_IS_LAMBDA:
		SET_TOKEN_TYPE(*this, "lambda");
		break;
	case TOKEN_FLAG_IS_DEF:
		SET_TOKEN_TYPE(*this, "def");
		break;
	//case TOKEN_FLAG_IS_LIST:
	//	SET_TOKEN_TYPE(*this, "list");
	//	break;
	case TOKEN_FLAG_IS_REF:
		SET_TOKEN_TYPE(*this, "ref");
		break;
	default:
		std::cout << "[ANALYZER:ERROR]    未定义的Token类型，请检查您的解析器" << std::endl;
		exit(0);
	}
	flag = to_flag;
	value.bits = 0;
}

Token::~Token()
{
	switch (flag)
	{
	case TOKEN_FLAG_IS_BITS:
	case TOKEN_FLAG_IS_BOOL:
		break;
	case TOKEN_FLAG_IS_ASSIGN:
	{
		TokenAssign* assign = (TokenAssign*)value.body;
		delete assign;
		break;
	}
	case TOKEN_FLAG_IS_BINARY:
	{
		TokenBinary* binary = (TokenBinary*)value.body;
		delete binary;
		break;
	}
	case TOKEN_FLAG_IS_PROG:
	{
		std::for_each(value.prog->begin(), value.prog->end(), [](Token* token) {
			if (token) delete token;
			});
		delete value.prog;
		break;
	}
	case TOKEN_FLAG_IS_IF:
	{
		TokenIf* body_if = (TokenIf*)value.body;
		delete body_if;
		break;
	}
	case TOKEN_FLAG_IS_CALLS:
	{
		TokenCalls* calls = (TokenCalls*)value.body;
		delete calls;
		break;
	}
	case TOKEN_FLAG_IS_LAMBDA:
	{
		TokenLambda* lambda = (TokenLambda*)value.body;
		delete lambda;
		break;
	}
	case TOKEN_FLAG_IS_DEF:
	{
		TokenDef* def = (TokenDef*)value.body;
		delete def;
		break;
	}
	case TOKEN_FLAG_IS_REF:
	{
		delete value.ref;
		break;
	}
	//case TOKEN_FLAG_IS_LIST:
	//{
	//	std::for_each(value.items->begin(), value.items->end(), [](Token* token) {
	//		if (token)
	//			delete token;
	//	});
	//	delete value.items;
	//	break;	
	//}
	default:
		delete value.str;
		break;
	}
}

TokenLRvalue::TokenLRvalue() : left(NULL), right(NULL)
{
}

TokenLRvalue::~TokenLRvalue()
{
	if (left)
	{
		delete left;
	}
	if (right)
	{
		delete right;
	}
}

TokenAssign::TokenAssign(): op(NULL), lrvalue(NULL)
{
}

TokenAssign::~TokenAssign()
{
	if (op)
	{
		delete op;
	}
	if (lrvalue)
	{
		delete lrvalue;
	}
}


TokenBinary::TokenBinary(): op(NULL), lrvalue(NULL)
{
}

TokenBinary::~TokenBinary()
{
	if (op)
	{
		delete op;
	}
	if (lrvalue)
	{
		delete lrvalue;
	}
}


TokenCalls::TokenCalls() : func(NULL), args(NULL)
{
}

TokenCalls::~TokenCalls()
{
	if (func)
	{
		delete func;
	}
	if (args)
	{
		std::for_each(args->begin(), args->end(), [](Token* token) {if (token) delete token; });
		delete args;
	}

}



TokenLambda::TokenLambda():args(NULL), body(NULL)
{
}

TokenLambda::~TokenLambda()
{
	if (body)
	{
		delete body;
	}
	if (args)
	{
		std::for_each(args->begin(), args->end(), [](std::string* arg) {if (arg) delete arg; });
		delete args;
	}

}


TokenBranch::TokenBranch() : then_branch(NULL), else_branch(NULL)
{
}

TokenBranch::~TokenBranch()
{
	if (then_branch)
	{
		delete then_branch;
	}

	if (else_branch)
	{
		delete else_branch;
	}
}


TokenIf::TokenIf() : cond(NULL), branch(NULL)
{
}

TokenIf::~TokenIf()
{
	if (cond)
	{
		delete cond;
	}

	if (branch)
	{
		delete branch;
	}
}

TokenDef::TokenDef() : var(NULL), value(NULL)
{
}

TokenDef::~TokenDef()
{
	if (var)
	{
		delete var;
	}
	if (value)
	{
		delete value;
	}

}
