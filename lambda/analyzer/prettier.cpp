#include "../framework.h"
#include "./token/token.h"

static size_t deepin = 0;
static char buffer[0x1000];

// Horizontal_Tab
#define HTAB		for (size_t i = 0; i < deepin; i++) { ast += "\t"; }

static void construct_token(Token* token, std::string& ast);
static void construct_token_prog(std::vector<Token*>* progs, std::string& ast);

static void construct_token_lrvalue(TokenLRvalue* body, std::string& ast)
{
	HTAB
		ast += "\"left value\": \n";

	HTAB
		ast += "{\n";

	deepin++;
	construct_token(body->left, ast);
	deepin--;
	HTAB
		ast += "}\n";

	HTAB
		ast += "\"right value\": \n";

	HTAB
		ast += "{\n";

	deepin++;
	construct_token(body->right, ast);
	deepin--;
	HTAB
		ast += "}\n";
}

static void construct_token_binary(TokenBinary* body, std::string& ast)
{
	HTAB

#ifdef WINDOWS
		sprintf_s(buffer, "\"operator\": \"%s\", \n", body->op->c_str());
#else
		sprintf(buffer, "\"operator\": \"%s\", \n", body->op->c_str());
#endif

	ast += buffer;

	construct_token_lrvalue(body->lrvalue, ast);

}

static void construct_token_call(TokenCalls* token_call, std::string& ast)
{
	HTAB
		ast += "\"func\": \n";
	HTAB
		ast += "{\n";
	deepin++;
	construct_token(token_call->func, ast);
	deepin--;

	HTAB
		ast += "},\n";

	HTAB
		ast += "\"args\": \n";
	HTAB
		ast += "[\n";

	deepin++;
	construct_token_prog(token_call->args, ast);
	deepin--;

	HTAB
		ast += "]\n";

}

static void construct_token_lambda(TokenLambda* token_lambda, std::string& ast)
{
	HTAB
		ast += "\"args\": \n";
	HTAB
		ast += "[\n";
	deepin++;

	std::vector<std::string*>::iterator arg = token_lambda->args->begin();
	while (arg != token_lambda->args->end())
	{
		HTAB
			ast += "\"" + *(*arg) + "\", \n";
		arg++;
	}
	deepin--;
	HTAB
		ast += "]\n";

	// lambda body
	HTAB
		ast += "\"body\": \n";

	HTAB
		ast += "{\n";

	deepin++;
	construct_token(token_lambda->body, ast);
	deepin--;

	HTAB
		ast += "}\n";

}

static void construct_token_if(TokenIf* token_if, std::string& ast)
{

	HTAB
		ast += "\"cond\": \n";
	HTAB
		ast += "{\n";

	deepin++;
	construct_token(token_if->cond, ast);
	deepin--;

	HTAB
		ast += "},\n";


	HTAB
		ast += "\"then\": \n";
	HTAB
		ast += "{\n";

	deepin++;
	construct_token(token_if->branch->then_branch, ast);
	deepin--;

	HTAB
		ast += "},\n";

	if (token_if->branch->else_branch)
	{

		HTAB
			ast += "\"else\": \n";
		HTAB
			ast += "{\n";

		deepin++;
		construct_token(token_if->branch->else_branch, ast);
		deepin--;

		HTAB
			ast += "}\n";

	}

}

//static void construct_token_let_func(TokenDef* letfunc, std::string& ast)
//{
//	HTAB
//		ast += "\"function name\": ";
//	ast += *letfunc->var->value.str;
//	ast += "\n";
//	HTAB
//		ast += "\"body\": \n";
//	HTAB
//		ast += "{\n";
//	deepin++;
//
//	construct_token(letfunc->value, ast);
//	deepin--;
//
//	HTAB
//		ast += "}\n";
//
//}

static void construct_token_let(TokenDef* letvar, std::string& ast)
{
	construct_token(letvar->var, ast);
	ast += "\n";
	construct_token(letvar->value, ast);
}

static void construct_token(Token* token, std::string& ast)
{
	HTAB

#ifdef WINDOWS
	sprintf_s(buffer, "\"type\": \"%s\", \n", token->type);
#else
	sprintf(buffer, "\"type\": \"%s\", \n", token->type);
#endif

	ast += buffer;

	if ((token->flag == TOKEN_FLAG_IS_ASSIGN) || token->flag == TOKEN_FLAG_IS_BINARY)
	{
		construct_token_binary(reinterpret_cast<TokenBinary*>(token->value.body), ast);
	}
	else if (token->flag == TOKEN_FLAG_IS_STRING || token->flag == TOKEN_FLAG_IS_NUM || token->flag == TOKEN_FLAG_IS_VAR)
	{
		HTAB
#ifdef WINDOWS
			sprintf_s(buffer, "\"value\": \"%s\"\n", token->value.str->c_str());
#else
			sprintf(buffer, "\"value\": \"%s\"\n", token->value.str->c_str());
#endif
		ast += buffer;
	}
	else if (token->flag == TOKEN_FLAG_IS_BOOL)
	{
		HTAB
			ast += "\"value\": \"";
		if (token->value.bits == 1)
		{
			ast += "true";
		}
		else
		{
			ast += "false";
		}
		ast += "\"\n";
	}
	else if (token->flag == TOKEN_FLAG_IS_LAMBDA)
	{
		construct_token_lambda((TokenLambda*)token->value.body, ast);
	}
	else if (token->flag == TOKEN_FLAG_IS_IF)
	{
		construct_token_if((TokenIf*)token->value.body, ast);
	}
	else if (token->flag == TOKEN_FLAG_IS_CALLS)
	{
		construct_token_call((TokenCalls*)token->value.body, ast);
	}
	//else if (token->flag == TOKEN_FLAG_IS_LIST)
	//{
	//	HTAB
	//		ast += "\"list\": \n";
	//	HTAB
	//		ast += "[\n";

	//	deepin++;
	//	construct_token_prog(token->value.items, ast);
	//	deepin--;

	//	HTAB
	//		ast += "]\n";
	//}
	else if (token->flag == TOKEN_FLAG_IS_REF)
	{
		HTAB
			ast += "\"ref\": \n";
		deepin++;
		construct_token(token->value.ref, ast);
		deepin--;
	}
	else if (token->flag == TOKEN_FLAG_IS_DEF)
	{
		HTAB
#ifdef WINDOWS
			sprintf_s(buffer, "\"value\": {\n");
#else
			sprintf(buffer, "\"value\": {\n");
#endif
		ast += buffer;
		deepin++;
		construct_token_let((TokenDef*)token->value.body, ast);
		deepin--;
		HTAB
			ast += "}\n";

	}
	else if (token->flag == TOKEN_FLAG_IS_PROG)
	{
		HTAB
			ast += "[\n";
		deepin++;
		construct_token_prog(token->value.prog, ast);
		deepin--;

		HTAB
			ast += "],\n";
	}

}

static void construct_token_prog(std::vector<Token*>* progs, std::string& ast)
{
	std::vector<Token*>::iterator tokens = progs->begin();
	while (tokens != progs->end())
	{
		HTAB
			ast += "{\n";

		deepin++;
		construct_token(*tokens, ast);
		deepin--;

		HTAB
			ast += "},\n";

		tokens++;
	}

}


std::string* construct_ast(Token* root)
{
	std::string* p_ast = DBG_NEW std::string;
	std::string& ast = *p_ast;

	ast = "The AST:\n";

	HTAB
		ast += "{\n";

	deepin++;
	construct_token(root, ast);
	deepin--;

	HTAB
		ast += "}\n";

	return p_ast;
}
