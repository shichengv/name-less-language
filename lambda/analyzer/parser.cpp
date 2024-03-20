#include "parser.h"
#include "token_stream.h"

#define NO_RETURN_NEXT_TOEKN				Token* tmp = token_next(); if (tmp) delete tmp;

static std::unordered_map<std::string, int>* Precedence;

static std::vector<Token*>* delimited_parameter(char start, char stop, char separator, Token* (*parser)(bool* skip_expression_separator));
static std::vector<Token*>* delimited_expression(char start, char stop, char separator, Token* (*parser)(bool* skip_expression_separator));
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
		1. ��ǰ��token����Ϊ��
		2. ��ǰtoken�����ͱ���Ϊ"op"
		3. �����ֵ�Ƿ�ΪNULL
		4. ��ǰtoken��ֵ���ڴ����ֵ
	*/
	return token && token->flag == TOKEN_FLAG_IS_OP && (!op || !strcmp(token->value.str->c_str(), op) ) && token;
}
static bool is_punc(const char ch)
{
	Token* token = token_peek();
	/*
		1. ��ǰ��token����Ϊ��
		2. ��ǰtoken�����ͱ���Ϊ"punc"
		3. �����ֵ����Ϊ��(!ch ��ʾ�����Ϊ�գ���Ϊtrue)
		4. ��ǰtoken��ֵ���ڴ����ֵ
	*/
	return token && token->flag == TOKEN_FLAG_IS_PUNC && (!ch || ( token->value.str->size() == 1 && token->value.str->at(0) == ch)) && token;
}
static bool is_kw(const char* kw)
{
	Token* token = token_peek();
	/*
		1. ��ǰ��token����Ϊ��
		2. ��ǰtoken�����ͱ���Ϊ"punc"
		3. �����ֵ����Ϊ��(kw.empty() ��ʾ�����Ϊ�գ���Ϊtrue)
		4. ��ǰtoken��ֵ���ڴ����ֵ
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
#ifdef WINDOWS
			std::wstring error = L"����\x2661~���ǲ���������ӱ��ʽ�ָ���\";\"��~";
#else
			std::string error = "����\u2661~���ǲ���������ӱ��ʽ�ָ���\";\"��~";
#endif
			input_croak(error);
			break;
		}
		case ')':
		case '}':
		{
#ifdef WINDOWS
			std::wstring error = L"�汿\x2661~���ǲ���������\x2661һ\x2661��\x2661��\x2661~";
#else
			std::string error = "�汿\u2661~���ǲ���������\u2661һ\u2661��\u2661��\u2661~";
#endif
			input_croak(error);
			break;
		}
		default:
		{
#ifdef WINDOWS
			std::wstring error = L"����Ԥ�ڽ����ַ�: \"" + ch + '\"';
#else
			std::string error = "����Ԥ�ڽ����ַ�: \"" + ch + '\"';
#endif
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
		std::string error = "Ԥ�ڽ��ܹؼ��֣�" + *kw + '\"';
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
		std::string error = "Ԥ�ڽ��ܲ�������" + *op + '\"';
		input_croak(error);
	}
}
static Token* unexpected_token()
{
	std::string error = "δ�����Token";
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
			//NO_RETURN_NEXT_TOEKN ����token��������token_peek()�õ��ģ���ʱcurrent_tokenָ���˵�ǰ��op token����ʱ����token��current_token����ΪNULL������������read_next()
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
			// ���ӵ���token
			token_predict_binary->value.body = token_binary;
			// �����������ŵ�binary��ԭop token���Ա�ɾ��
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

static std::vector<Token*>* delimited_expression(char start, char stop, char separator, Token * (*parser)(bool *skip_expression_separator))
{
	std::vector<Token*>* args = DBG_NEW std::vector<Token*>;

	/*
		�Ƿ���Ҫ�������ʽ�ָ�����������ʽ��һ�����ʽ���� prog if lambda�Ķ��ر��ʽ��ʽ�������ø�ֵ����ʾ��Ҫ���� spearator��
		��������� lambda ���ʽ���Ҿ��д���飬��Ӧ�ø� �ָ�����
	*/
	bool skip_expression_separator = false;
	/*
		��Ŀ�ͷ	
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

static std::vector<Token*>* delimited_parameter(char start, char stop, char separator, Token * (*parser)(bool *skip_expression_separator))
{
	std::vector<Token*>* args = DBG_NEW std::vector<Token*>;

	/*
		�Ƿ���Ҫ�������ʽ�ָ�����������ʽ��һ�����ʽ���� prog if lambda�Ķ��ر��ʽ��ʽ�������ø�ֵ����ʾ��Ҫ���� spearator��
		��������� lambda ���ʽ���Ҿ��д���飬��Ӧ�ø� �ָ�����
	*/
	bool skip_expression_separator = false;
	/*
		��Ŀ�ͷ	
	*/
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
	body_call->args = delimited_parameter('(', ')', ',', parse_expression);

	return token_call;
}

static std::string* parse_varname()
{
	Token* name = token_next();
	std::string* varname = name->value.str;
	if (name->flag != TOKEN_FLAG_IS_VAR)
	{
		std::string error = "������~";
		input_croak(error);
	}
	// ����ֻ��Ҫ���Ĳ�������������Ҫtoken�����Դ�ʱ����Ҫ����flag���߻��������������token��value
	// Ȼ��ɾ����token
	SET_TOKEN_FLAG_TO_BITS(*name);
	delete name;
	return varname;
}

/*

����������

	��������������һ�� if ���ʽ

	then�ؼ��ֵ����������������������ʽ�����ţ����Ϊ�������ʽ�������ţ������ʡ��then�ؼ��֣����Զ�������
�﷨��if���Ҳ�ǳ����ģ�����ޱȣ�������ʹ��ͳһ�ķ��
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
	// then �ؼ��ֿ�ʡ��
	bool ignore_then_kw = false;
	// ��ȡ����token
	Token* cond = NULL;
	// then��֧token
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

	// �½�һ��token����������if
	Token* token = DBG_NEW Token(TOKEN_FLAG_IS_IF);
	// if token
	TokenIf* token_if = DBG_NEW TokenIf;
	// ���ӵ�token�ڵ�
	token->value.body = token_if;
	// ����if���������ʽ
	token_if->cond = cond;

	// �½�tokenbranch
	TokenBranch* if_branch = DBG_NEW TokenBranch;
	// tokenif���ӷ�֧token
	token_if->branch = if_branch;
	if_branch->then_branch = then_branch;

	if (is_kw("else"))
	{
		// ����else�ؼ���
		NO_RETURN_NEXT_TOEKN

		// ����д����ţ��������ָ���
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
	// ���valueΪtrue���͸�ֵΪ1������0
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
	token_prog->value.prog = delimited_expression('{', '}', ';', parse_expression);

	// ���{ } ��δ�κα��ʽ����Ϊ ��
	if (token_prog->value.prog->size() == 0)
	{
#ifdef WINDOWS
		std::wstring error = L"{ } ��δ�����κα��ʽ�������Լ�����Ҹ�ʲô����֪����\x2661~";
#else
		std::string error = "{ } ��δ�����κα��ʽ�������Լ�����Ҹ�ʲô����֪����\u2661~";
#endif
		input_croak(error);
		// return NULL;
	}
	// ����һ���� { } �����ı��ʽ���У���������Ҫ�ָ�������������Ϊtrue
	*skip_expression_separator = true;
	return token_prog;
}

static Token* parse_def(bool* skip_expression_separator)
{
	skip_kw("def");
	Token* def = DBG_NEW Token(TOKEN_FLAG_IS_DEF);
	// �����ǰ��token��Ϊ�գ�������Ϊpunc������ֵΪ '('������÷�֧
	Token* tok = token_next();

	// Ϊ��ʡ��lambda�ؼ���
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
		// �����ǰ��token��Ϊ�գ����ַ���Ϊ�ؼ��ֵ�def
		if (is_kw("def"))
		{
			return parse_def(skip_expression_separator);
		}

		if (is_kw("true") || is_kw("false"))
		{
			return parse_bool();
		}

		if (is_kw("lambda") || is_kw("��"))
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
		�Ƿ���Ҫ�������ʽ�ָ�����������ʽ��һ�����ʽ���� prog if lambda�Ķ��ر��ʽ��ʽ�������ø�ֵ����ʾ��Ҫ���� " ; "
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