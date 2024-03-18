#include "parser.h"
#include "token_stream.h"

#define NO_RETURN_NEXT_TOEKN				Token* tmp = token_next(); if (tmp) delete tmp;

static std::unordered_map<std::string, int>* Precedence;

static std::vector<Token*>* delimited(char start, char stop, char separator, Token* (*parser)(bool* skip_expression_separator));
static Token* parse_prog(bool* skip_expression_separator);
static Token* parse_atom(bool* skip_expression_separator);
static Token* maybe_call(Token* (*expr)(bool* skip_expression_separator), bool* skip_expression_separator);
static Token* maybe_binary(Token* left, int my_prec, bool* skip_expression_separator);
static Token* parse_expression(bool* skip_expression_separator);
static Token* parse_if(bool* skip_expression_separator);

void parser_init()
{
	Precedence = DBG_NEW std::unordered_map<std::string, int>;
	// = 
	(*Precedence)["="] = 1;
	// ||
	(*Precedence)["||"] = 2;
	// &&
	(*Precedence)["&&"] = 3;
	// < > <= >= == !=
	(*Precedence)["<"] = 7;
	(*Precedence)[">"] = 7;
	(*Precedence)["<="] = 7;
	(*Precedence)[">="] = 7;
	(*Precedence)["=="] = 7;
	(*Precedence)["!="] = 7;
	// + -
	(*Precedence)["+"] = 10;
	(*Precedence)["-"] = 10;
	// * / %
	(*Precedence)["*"] = 20;
	(*Precedence)["/"] = 20;
	(*Precedence)["%"] = 20;

}
void parser_clean()
{
	delete Precedence;
}
static bool is_op(const char* op)
{
	Token* token = token_peek();

	/*
		1. 当前的token不能为空
		2. 当前token的类型必须为"op"
		3. 传入的值是否为NULL
		4. 当前token的值等于传入的值
	*/
	return token && token->flag == TOKEN_FLAG_IS_OP && (!op || !strcmp(token->value.str->c_str(), op) ) && token;
}
static bool is_punc(const char ch)
{
	Token* token = token_peek();
	/*
		1. 当前的token不能为空
		2. 当前token的类型必须为"punc"
		3. 传入的值不能为空(!ch 表示：如果为空，则为true)
		4. 当前token的值等于传入的值
	*/
	return token && token->flag == TOKEN_FLAG_IS_PUNC && (!ch || ( token->value.str->size() == 1 && token->value.str->at(0) == ch)) && token;
}
static bool is_kw(const char* kw)
{
	Token* token = token_peek();
	/*
		1. 当前的token不能为空
		2. 当前token的类型必须为"punc"
		3. 传入的值不能为空(kw.empty() 表示：如果为空，则为true)
		4. 当前token的值等于传入的值
	*/
	return token && token->flag == TOKEN_FLAG_IS_KW && (!kw || !strcmp(token->value.str->c_str(), kw) ) && token;
}
static void skip_punc(const char ch)
{
	if (is_punc(ch))
	{
		NO_RETURN_NEXT_TOEKN
	}
	else
	{
		switch (ch)
		{
		case ';':
		{
			std::wstring error = L"杂鱼\x2661~，是不是忘记添加表达式分隔符\";\"了~";
			input_croak(error);
			break;
		}
		case ')':
		case '}':
		{
			std::wstring error = L"真笨\x2661~，是不是忘了另\x2661一\x2661半\x2661了\x2661~";
			input_croak(error);
			break;
		}
		default:
		{
			std::wstring error = L"这里预期接收字符: \"" + ch + '\"';
			input_croak(error);
			break;
		}
		}
	}
}
static void skip_kw(const char* kw)
{
	if (is_kw(kw))
	{
		NO_RETURN_NEXT_TOEKN
	}
	else
	{
		std::wstring error = L"预期接受关键字：" + *kw + '\"';
		input_croak(error);
	}
}
static void skip_op(const char* op)
{
	if (is_op(op))
	{
		//NO_RETURN_NEXT_TOEKN
		token_next();
	}
	else
	{
		std::wstring error = L"预期接受操作符：" + *op + '\"';
		input_croak(error);
	}
}
static Token* unexpected_token()
{
	std::wstring error = L"未定义的Token";
	input_croak(error);
	return NULL;
}

static Token* maybe_binary(Token* left, int my_prec, bool *skip_expression_separator)
{
	Token* token_predict_op = NULL;
	if (is_op(NULL))
	{
		token_predict_op = token_peek();
	}
	if (token_predict_op)
	{
		int his_prec = (*Precedence)[*(token_predict_op->value.str)];
		if (his_prec > my_prec)
		{
			//NO_RETURN_NEXT_TOEKN 由于token变量是由token_peek()得到的，此时current_token指向了当前的op token，此时调用token将current_token设置为NULL，它并不调用read_next()
			token_next();
			Token* token_predict_binary;
			if (*token_predict_op->value.str == "=")
			{
				token_predict_binary = DBG_NEW Token(TOKEN_FLAG_IS_ASSIGN);
			}
			else
			{
				token_predict_binary = DBG_NEW Token(TOKEN_FLAG_IS_BINARY);
			}

			TokenBinary* token_binary = DBG_NEW TokenBinary;
			// 连接到根token
			token_predict_binary->value.body = token_binary;
			// 拷贝操作符号到binary，原op token可以被删除
			token_binary->op = DBG_NEW std::string(*token_predict_op->value.str);
			delete token_predict_op;
			token_binary->lrvalue = DBG_NEW TokenLRvalue;
			token_binary->lrvalue->left = left;
			token_binary->lrvalue->right = maybe_binary(parse_atom(skip_expression_separator), his_prec, skip_expression_separator);

			return maybe_binary(token_predict_binary, my_prec, skip_expression_separator);

		}
	}

	return left;
}

static std::vector<std::string*>* delimited_varname(char start, char stop, char separator, std::string * (*parser)())
{
	std::vector<std::string*>* args = DBG_NEW std::vector<std::string*>;

	bool first = true;

	skip_punc(start);

	while (!token_eof())
	{
		if (is_punc(stop))
		{
			break;
		}

		if (first)
		{
			first = false;
		}
		else
		{
			skip_punc(separator);
		}

		if (is_punc(stop))
		{
			break;
		}

		args->push_back(parser());
	}

	skip_punc(stop);

	return args;
}

static std::vector<Token*>* delimited(char start, char stop, char separator, Token * (*parser)(bool *skip_expression_separator))
{
	std::vector<Token*>* args = DBG_NEW std::vector<Token*>;

	/*
		是否需要跳过表达式分隔符，如果表达式上一个表达式存在 prog if lambda的多重表达式形式，则设置改值，表示需要跳过 " ; "
	*/
	bool skip_expression_separator = false;
	/*
		块的开头	
	*/
	bool first = true;

	skip_punc(start);

	while (!token_eof())
	{
		if (is_punc(stop))
		{
			break;
		}

		if (first || skip_expression_separator)
		{
			first = false;
			skip_expression_separator = false;
		}
		else
		{
			skip_punc(separator);
		}

		if (is_punc(stop))
		{
			break;
		}

		args->push_back(parser(&skip_expression_separator));
	}

	skip_punc(stop);

	return args;
}

static Token* parse_calls(Token* func) 
{
	Token* token_call = DBG_NEW Token(TOKEN_FLAG_IS_CALLS);

	TokenCalls* body_call = DBG_NEW TokenCalls;
	token_call->value.body = body_call;
	body_call->func = func;
	body_call->args = delimited('(', ')', ',', parse_expression);

	return token_call;
}

static std::string* parse_varname()
{
	Token* name = token_next();
	std::string* varname = name->value.str;
	if (name->flag != TOKEN_FLAG_IS_VAR)
	{
		std::wstring error = L"猪";
		input_croak(error);

	}
	// 由于只需要它的参数名，而不需要token，所以此时仅需要设置flag告诉回收器并不清理该token的value
	// 然后删除该token
	SET_TOKEN_FLAG_TO_BITS(*name);
	delete name;
	return varname;
}

/*

例程描述：

	该例程用来解析一个 if 表达式

	then关键字的作用是用来忽略条件表达式的括号，如果为条件表达式增加括号，则可以省略then关键字，所以对于如下
语法，if语句也是成立的（奇丑无比），建议使用统一的风格。
	if (a > 5)
		a = 3;
	else if a < 2 then
		a = 9;
	else if a > 9
	{
		a = a + 13;
	}
	else
		a = 0;

*/
static Token* parse_if(bool *skip_expression_separator)
{
	// then 关键字可省略
	bool ignore_then_kw = false;
	// 获取条件token
	Token* cond = NULL;
	// then分支token
	Token* then_branch = NULL;

	skip_kw("if");
	if (is_punc('('))
	{
		skip_punc('(');
		cond = parse_expression(skip_expression_separator);
		skip_punc(')');
		ignore_then_kw = true;
	}
	else
		cond = parse_expression(skip_expression_separator);

	if (is_punc('{'))
		then_branch = parse_expression(skip_expression_separator);
	else if (ignore_then_kw)
	{
		then_branch = parse_expression(skip_expression_separator);
		skip_punc(';');
	}
	else
	{
		skip_kw("then");
		then_branch = parse_expression(skip_expression_separator);
		skip_punc(';');
	}

	// 新建一个token，用来连接if
	Token* token = DBG_NEW Token(TOKEN_FLAG_IS_IF);
	// if token
	TokenIf* token_if = DBG_NEW TokenIf;
	// 连接到token节点
	token->value.body = token_if;
	// 设置if的条件表达式
	token_if->cond = cond;

	// 新建tokenbranch
	TokenBranch* if_branch = DBG_NEW TokenBranch;
	// tokenif连接分支token
	token_if->branch = if_branch;
	if_branch->then_branch = then_branch;

	if (is_kw("else"))
	{
		// 跳过else关键字
		NO_RETURN_NEXT_TOEKN

		// 如果有大括号，就跳过分隔符
		if (is_punc('{'))
			*skip_expression_separator = true;
		else
			*skip_expression_separator = false;

		if_branch->else_branch = parse_expression(skip_expression_separator);
	}

	return token;
}

static Token* parse_lambda(bool *skip_expression_separator)
{
	Token* token_lambda = DBG_NEW Token(TOKEN_FLAG_IS_LAMBDA);
	TokenLambda* body_lambda = DBG_NEW TokenLambda;
	token_lambda->value.body = body_lambda;

	body_lambda->args = delimited_varname('(', ')', ',', parse_varname);
	if (is_punc('{'))
	{
		body_lambda->body = parse_prog(skip_expression_separator);
		*skip_expression_separator = true;
	}
	else
	{
		body_lambda->body = parse_expression(skip_expression_separator);
	}
	return token_lambda;
}

static Token* parse_bool() 
{
	Token* token_bool = DBG_NEW Token(TOKEN_FLAG_IS_BOOL);
	Token* next_token = token_next();
	// 如果value为true，就赋值为1，否则0
	token_bool->value.bits = (!strcmp(next_token->value.str->c_str(), "true"));
	delete next_token;

	return token_bool;
}

static Token* maybe_call(Token* (*expr)(bool *skip_expression_separator), bool *skip_expression_separator)
{
	Token* expr_token = expr(skip_expression_separator);

	if (is_punc('('))
	{
		return parse_calls(expr_token);
	}
	else
	{
		return expr_token;
	}

}

static Token* parse_prog(bool *skip_expression_separator)
{
	Token* token_prog = DBG_NEW Token(TOKEN_FLAG_IS_PROG);
	token_prog->value.prog = delimited('{', '}', ';', parse_expression);

	// 如果{ } 并未任何表达式，则为 空
	if (token_prog->value.prog->size() == 0)
	{
		std::wstring error = L"{ } 并未包含任何表达式！你连自己想对我干什么都不知道嘛\x2661~";
		input_croak(error);
		// return NULL;
	}
	// 对于一个由 { } 包裹的表达式序列，它并不需要分隔符，所以设置为true
	*skip_expression_separator = true;
	return token_prog;
}

static Token* parse_list(bool* skip_expression_separator)
{
	Token* list = DBG_NEW Token(TOKEN_FLAG_IS_LIST);
	list->value.items = delimited('(', ')', ',', parse_expression);
	
	return list;
}

static Token* parse_def(bool* skip_expression_separator)
{
	skip_kw("def");
	Token* def = DBG_NEW Token(TOKEN_FLAG_IS_DEF);
	// 如果当前的token不为空，且类型为punc，并且值为 '('，进入该分支
	Token* tok = token_next();

	// 其实就是为了省略lambda关键字，当然你想加lambda关键字也可以
	if (is_punc('('))
	{
		TokenDef* body_def_func = DBG_NEW TokenDef;
		def->value.body = body_def_func;

		body_def_func->var = tok;
		body_def_func->value = parse_lambda(skip_expression_separator);

		return def;
	}
	else
	{
		TokenDef* body_def_var = DBG_NEW TokenDef;
		def->value.body = body_def_var;
		body_def_var->var = tok;
		body_def_var->value = parse_expression(skip_expression_separator);
		return def;
	}

}

static Token* parse_expression(bool* skip_expression_separator)
{
	auto expr = [](bool* skip_expression_separator) -> Token* {
		return maybe_binary(parse_atom(skip_expression_separator), 0, skip_expression_separator);
		};

	return maybe_call(expr, skip_expression_separator);

}

static Token* parse_atom(bool* skip_expression_separator)
{
	auto expr = [](bool* skip_expression_separator) -> Token* {
		if (is_punc('('))
		{
			NO_RETURN_NEXT_TOEKN
			Token* exp = parse_expression(skip_expression_separator);
			skip_punc(')');
			return exp;
		}
		if (is_punc('{'))
		{
			return parse_prog(skip_expression_separator);
		}
		if (is_kw("if"))
		{
			return parse_if(skip_expression_separator);
		}
		// 如果当前的token不为空，且字符串为关键字的def
		if (is_kw("def"))
		{
			return parse_def(skip_expression_separator);
		}

		// 如果当前的token不为空，且字符串为关键字的list
		if (is_kw("list"))
		{
			NO_RETURN_NEXT_TOEKN
			return parse_list(skip_expression_separator);
		}

		if (is_kw("true") || is_kw("false"))
		{
			return parse_bool();
		}

		if (is_kw("lambda") || is_kw("λ"))
		{
			NO_RETURN_NEXT_TOEKN
			return parse_lambda(skip_expression_separator);
		}

		Token* token = token_next();

		if (token->flag == TOKEN_FLAG_IS_VAR 
			|| token->flag == TOKEN_FLAG_IS_NUM 
			|| token->flag == TOKEN_FLAG_IS_STRING
			|| token->flag == TOKEN_FLAG_IS_REF
			)
			return token;


		return unexpected_token();
	};
	return maybe_call(expr, skip_expression_separator);
}

void parse_toplevel(Token* prog_root)
{
	prog_root->value.prog = DBG_NEW std::vector<Token*>;
	/*
		是否需要跳过表达式分隔符，如果表达式上一个表达式存在 prog if lambda的多重表达式形式，则设置改值，表示需要跳过 " ; "
	*/
	bool skip_expression_separator = false;

	while (!token_eof())
	{
		prog_root->value.prog->push_back(parse_expression(&skip_expression_separator));
		if (skip_expression_separator)
		{
			skip_expression_separator = false;
		}
		else
		{
			skip_punc(';');
		}
	}
}
