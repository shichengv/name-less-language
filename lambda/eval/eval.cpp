#include "../framework.h"
#include "../analyzer/token/token.h"
#include "../stdlib/stdlib.h"
#include "value/value.h"
#include "env.h"
#include "../configure/error_log.h"
#ifdef WINDOWS
#define STR(str)		L##str
#else	
#define STR(str)		str
#endif // WINDOWS


extern Value* calls_stdlib_func(uint64_t func_addr, std::vector<Value*>* args_list);
static Environment* eval_env;
static Environment* global_env;

void eval_root_prog(Token* root);

static Value* eval(Token* exp, Environment* env);

// evaluate multiple expressions
static Value* eval_prog(std::vector<Token*>* progs, Environment* env);

static void use_typeid_error(uint8_t flag)
{
#ifdef WINDOWS
	std::wstring typeinfo;
	std::wstring error;
#else
	std::string typeinfo;
	std::string error;
#endif // WINDOWS

	switch (flag)
	{
	case VALUE_IS_NUM:
		typeinfo = STR(" ��Ԥ�����Ͳ�ƥ�䣬������� number");
		break;
	case VALUE_IS_STRING:
		typeinfo = STR(" ��Ԥ�����Ͳ�ƥ�䣬������� string");
		break;
	case VALUE_IS_LOGICAL:
		typeinfo = STR(" ��Ԥ�����Ͳ�ƥ�䣬������� bool");
		break;
	case VALUE_IS_LAMBDA:
	case VALUE_IS_STD_FUNC_OFFSET:
		typeinfo = STR(" ��Ԥ�����Ͳ�ƥ�䣬������� lambda");
		break;
	case VALUE_IS_PAIR:
		typeinfo = STR(" ��Ԥ�����Ͳ�ƥ�䣬������� pair");
		break;
	case VALUE_IS_REF:
		typeinfo = STR(" ��Ԥ�����Ͳ�ƥ�䣬������� ref");
		break;
	default:
		break;
	}
#ifdef WINDOWS
	error = L"\x2044\x2060\x28\x2060\x2044\x2060\xa0\x2060\x2044\x2060\x2022\x2060\x2044\x2060\x2d\x2060\x2044\x2060\x2022\x2060\x2044\x2060\xa0\x2060\x2044\x2060\x29\x2060\x2044\xff0c\x8bb0\x5f97\x68c0\x67e5\x53d8\x91cf\x7c7b\x578b\x5440\xff01";
	error = error + typeinfo;
#else
	error = "\u2044\u2060\u0028\u2060\u2044\u2060\u00a0\u2060\u2044\u2060\u2022\u2060\u2044\u2060\u002d\u2060\u2044\u2060\u2022\u2060\u2044\u2060\u00a0\u2060\u2044\u2060\u0029\u2060\u2044\uff0c\u8bb0\u5f97\u68c0\u67e5\u53d8\u91cf\u7c7b\u578b\u5440\uff01";
	error = error + typeinfo;
#endif // WINDOWS

	throw_eval_exception(error);

}

static inline double use_num(Value* value)
{
	if (value->flag != VALUE_IS_NUM)
		use_typeid_error(value->flag);

	return value->value.number;
};

static inline std::string* use_str(Value* value)
{
	if (value->flag != VALUE_IS_STRING)
		use_typeid_error(value->flag);
	return value->value.str;
};

static inline uint64_t use_logical(Value* value)
{
	if (value->flag != VALUE_IS_LOGICAL)
		use_typeid_error(value->flag);
	return value->value.logical;
};

static Value* apply_op(std::string* op, Value* left, Value* right, Environment* env)
{
	Value* result = DBG_NEW Value;
	// Ϊ�˱���ͳһ�����ǲ���apply_op���ӻ��ն��������ն���ֻ����eval��������
	//	env->parent->constant.push_back(value);

	if (!strcmp(op->c_str(), "+"))
	{
		if (left->flag == VALUE_IS_NUM && right->flag == VALUE_IS_NUM)
		{
			result->flag = VALUE_IS_NUM;
			result->value.number = left->value.number + right->value.number;
		}
		else if (left->flag == VALUE_IS_STRING || right->flag == VALUE_IS_STRING)
		{
			result->flag = VALUE_IS_STRING;

			if (left->flag == VALUE_IS_NUM)
				result->value.str = DBG_NEW std::string(std::to_string(left->value.number) + *right->value.str);
			else if (right->flag == VALUE_IS_NUM)
				result->value.str = DBG_NEW std::string(*left->value.str + std::to_string(right->value.number));
			else
			{
				if (left->flag == VALUE_IS_STRING && right->flag == VALUE_IS_STRING)
					result->value.str = DBG_NEW std::string(*left->value.str + *right->value.str);
				else
				{
					std::cout << "type error..." << std::endl;
					exit(0);
				}
			}
		}
		else
		{
			throw "the type of left value is not equal to type of right value...";
		}

	}
	else if (!strcmp(op->c_str(), "-"))
	{
		result->flag = VALUE_IS_NUM;
		result->value.number = use_num(left) - use_num(right);
	}
	else if (!strcmp(op->c_str(), "*"))
	{
		result->flag = VALUE_IS_NUM;
		result->value.number = use_num(left) * use_num(right);
	}
	else if (!strcmp(op->c_str(), "/"))
	{
		result->flag = VALUE_IS_NUM;
		if (use_num(right) == 0.0)
		{
			throw "Divide by zero";
		}
		result->value.number = use_num(left) / use_num(right);
	}
	else if (!strcmp(op->c_str(), "%"))
	{
		result->flag = VALUE_IS_NUM;
		if (static_cast<int64_t>(use_num(right)) == 0)
		{
			throw "Divide by zero";
		}
		result->value.number = static_cast<double>(static_cast<int64_t>(use_num(left)) % static_cast<int64_t>(use_num(right)));
	}
	else if (!strcmp(op->c_str(), "<"))
	{
		result->flag = VALUE_IS_LOGICAL;
		if (use_num(left) < use_num(right))
			result->value.logical = 0x1;
	}
	else if (!strcmp(op->c_str(), ">"))
	{
		result->flag = VALUE_IS_LOGICAL;
		if (use_num(left) > use_num(right))
			result->value.logical = 0x1;
	}
	else if (!strcmp(op->c_str(), "<="))
	{
		result->flag = VALUE_IS_LOGICAL;
		if (use_num(left) <= use_num(right))
			result->value.logical = 0x1;
	}
	else if (!strcmp(op->c_str(), ">="))
	{
		result->flag = VALUE_IS_LOGICAL;
		if (use_num(left) >= use_num(right))
			result->value.logical = 0x1;
	}
	else if (!strcmp(op->c_str(), "!=")) 
	{
		result->flag = VALUE_IS_LOGICAL;

		if (left->flag == VALUE_IS_NUM && right->flag == VALUE_IS_NUM)
		{
			if (left->value.number != right->value.number)
				result->value.logical = 0x1;
		}
		else if (left->flag == VALUE_IS_BITS && right->flag == VALUE_IS_BITS)
		{
			if (left->value.bits != right->value.bits)
				result->value.logical = 0x1;
		}
		else if (left->flag == VALUE_IS_TYPEID && right->flag == VALUE_IS_TYPEID)
		{
			if (left->value.bits != right->value.bits)
				result->value.logical = 0x1;
		}
		else if (left->flag == VALUE_IS_LOGICAL && right->flag == VALUE_IS_LOGICAL)
		{
			if (left->value.logical != right->value.logical)
				result->value.logical = 0x1;
		}
		else if (left->flag == VALUE_IS_STRING && right->flag == VALUE_IS_STRING)
		{
			if (strcmp(left->value.str->c_str(), right->value.str->c_str()))
				result->value.logical = 0x1;
		}
		else 
		{
			if (left->flag == right->flag) {
				if (left->value.bits != right->value.bits)
					result->value.logical = 0x1;
			}
			else
			{
				result->flag = VALUE_IS_LOGICAL;
				result->value.logical = 0x1;
			}
		}

	}
	else if (!strcmp(op->c_str(), "=="))
	{
		result->flag = VALUE_IS_LOGICAL;

		if (left->flag == VALUE_IS_NUM && right->flag == VALUE_IS_NUM)
		{
			if (left->value.number == right->value.number)
				result->value.logical = 0x1;
		}
		else if (left->flag == VALUE_IS_BITS && right->flag == VALUE_IS_BITS)
		{
			if (left->value.bits == right->value.bits)
				result->value.logical = 0x1;
		}
		else if (left->flag == VALUE_IS_TYPEID && right->flag == VALUE_IS_TYPEID)
		{
			if (left->value.bits == right->value.bits)
				result->value.logical = 0x1;
		}
		else if (left->flag == VALUE_IS_LOGICAL && right->flag == VALUE_IS_LOGICAL)
		{
			if (left->value.logical == right->value.logical)
				result->value.logical = 0x1;
		}
		else if (left->flag == VALUE_IS_STRING && right->flag == VALUE_IS_STRING)
		{
			if (!strcmp(left->value.str->c_str(), right->value.str->c_str()))
				result->value.logical = 0x1;
		}
		else 
		{
			if (left->flag == right->flag) {
				if (left->value.bits == right->value.bits)
					result->value.logical = 0x1;
			}
			else
			{
				result->flag = VALUE_IS_LOGICAL;
				result->value.logical = 0x0;
			}
		}

	}
	else if (!strcmp(op->c_str(), "&&"))
	{
		result->flag = VALUE_IS_LOGICAL;
		result->value.logical = use_logical(left) && use_logical(right);
	}
	else if (!strcmp(op->c_str(), "||"))
	{
		result->flag = VALUE_IS_LOGICAL;
		result->value.logical = use_logical(left) || use_logical(right);
	}
	else
	{
		throw "Can't apply operator " + *op;
	}
	return result;



}

static Value* eval_assignment(TokenAssign* exp, Environment* env)
{
	TokenLRvalue* lrvalue = exp->lrvalue;
	if (lrvalue->left->flag != TOKEN_FLAG_IS_VAR)
	{
#ifdef WINDOWS
		std::wstring error = L"\x8d4b\x503c\x8bed\x53e5\x7684\x5de6\x4fa7\x5fc5\x987b\x4e3a\x4e00\x4e2a\x53d8\x91cf\xff5e\x28\x2220\x30fb\x3c9\x3c\x20\x29\x2312\x266a";
#else
		std::string error = "\u8d4b\u503c\u8bed\u53e5\u7684\u5de6\u4fa7\u5fc5\u987b\u4e3a\u4e00\u4e2a\u53d8\u91cf\uff5e\u0028\u2220\u30fb\u03c9\u003c\u0020\u0029\u2312\u266a";
#endif // WINDOWS

		throw_eval_exception(error);
	}
	/*	
	NOTICE:
		���� ��ֵ ��ʽ���������ڴ˹���һ����ֵ�������أ������������Ӧ���뵱ǰ���������һ�����ʽΪ��ֵ��ʽ������������һ����ֵ
		������ֵ���ӵ���ʱ���ݻ���������������ǰ�����ĸ���������ֱ��ʹ�ø÷���ֵ��
	*/
	Value* result = DBG_NEW Value( env->set(lrvalue->left->value.str, eval(lrvalue->right, env)));
	env->parent->constant.push_back(result);
	return result;
	//return env->set(lrvalue->left->value.str, eval(lrvalue->right, env));
}

// ���lambda��env�ڵ�ǰ�����Ĵ����յĻ��������У�������Ҳ�����յ�ǰ�����������������Ӧ�Ծֲ�����Ƕ��
static bool match_env(std::vector<Environment*>* env_sequence, Environment* lambda_local_env){
	for (auto item_env = env_sequence->begin(); item_env != env_sequence->end(); item_env++)
		if (reinterpret_cast<uint64_t>(*item_env) == reinterpret_cast<uint64_t>(lambda_local_env))
			return true;
	return false;
};


Value* eval_prog(std::vector<Token*>* progs, Environment* env)
{
	std::vector<Token*>::iterator exp = progs->begin();
	Value* result = NULL;
	while (exp != progs->end())
	{
		result = eval(*exp, env);

		// �Ƴ� cache data
		std::for_each(env->constant.rbegin(), env->constant.rend(), [](Value* value) {delete value; });
		env->constant.erase(env->constant.begin(), env->constant.end());

		//delete* exp;
		exp++;

	}
	return result;
}

static Value* eval_calls(TokenCalls* calls, std::vector<Value*>& args_list, Environment* env)
{
	Value* result = NULL;
	Value* tmp_result = NULL;
	// ͨ�����õĺ�����������øú�����Ӧ��lambdaʵ��
	Value* func = env->get(calls->func->value.str);
	if (func->flag == VALUE_IS_STD_FUNC_OFFSET)
	{
		result = calls_stdlib_func(func->value.bits, &args_list);
		env->parent->constant.push_back(result);
	}
	else
	{
		ValueLambda* value_lambda = (ValueLambda*)func->value.lambda;
		// ����ͨ��rcl���Զ���ĺ���
		// ���ʵ�������������β����������׳�����
		if (calls->args->size() != value_lambda->func_ptr->args->size())
			throw "The expected number of parameters received does not match the actual number of parameters passed in...";

		// Ϊ�º�������һ������
		Environment* new_env = new Environment(value_lambda->env);
		auto args = value_lambda->func_ptr->args->begin(); 
		auto parameter = args_list.begin();
		while ( args != value_lambda->func_ptr->args->end())
		{
			// ���¹���Ļ��������øķ��ţ��Լ��������ֵ
			new_env->def(*args, *parameter);
			
			// ������++
			args++;
			parameter++;
		}
		// ��ȡ�����ı���ʽ���У�����ִ��
		Token* token_func_prog = (Token*)value_lambda->func_ptr->body;
		if (token_func_prog->flag == TOKEN_FLAG_IS_PROG)
		{
			// ��Ȼ����ʹ��evalֱ�ӽ�����ֵ���������eval������⵽tokenΪprogʱ���ٴδ���һ���»��������ں���������˵
			// ����û�б�Ҫ�ģ����Դ�ʱ��Ҫ��ȡ����ʽ���У��ֶ�����eval_prog
			std::vector<Token*>* func_expressions = (std::vector<Token*>*)token_func_prog->value.prog;
			tmp_result = eval_prog(func_expressions, new_env);
		}
		else
		{
			tmp_result = eval(token_func_prog, new_env);
		}
		result = DBG_NEW Value(tmp_result);
		env->parent->constant.push_back(result);

		// �ж�result��һ������
		if (result->flag == VALUE_IS_LAMBDA)
		{
			ValueLambda* v_lambda = (ValueLambda*)result->value.lambda;
			if (reinterpret_cast<uint64_t>(v_lambda->env) == reinterpret_cast<uint64_t>(new_env)
				|| match_env(&new_env->env_sequence, v_lambda->env)) 
				// ���ӵ����ĸ������еĻ�������
				env->env_sequence.push_back(new_env);
			else
				delete new_env;
		}
		else
			delete new_env;


	}
	return result;
}

static Value* eval_if(TokenIf* token_if, Environment* env)
{
	Value* result = eval(token_if->cond, env);

	/*

	NOTICE:
		һ��ȡ�ɵ���ƣ�������֧����ʱ���������������������ͣ�ֻ�ж�����λ�����Ƿ�Ϊ0��
		���� false �� nil ���ͣ����ǵ�λ����ʼ��Ϊ 0�� ���Բ�ȡ else��֧������������
		��double���ʹ洢�����Լ�ʹ��������ֵΪ0������Ҳ�ǳ����ġ�

	*/

	/* ���������д�����Ա�ע��	*/
	//if (result->flag != VALUE_IS_LOGICAL && result->flag != VALUE_IS_NIL)
	//	result->value.logical = 0x1;

	TokenBranch* branch = (TokenBranch*)token_if->branch;
	if (result->value.logical)
		result = eval(branch->then_branch, env);
	else
		if (branch->else_branch)
			result = eval(branch->else_branch, env);
	return result;
}

/*
	��ֵ������
	һ���򵥵�Լ���������ֵ���ڲ��������ⲿ�Ĺ��̣����ⲿ�Ĺ���������value������ֵ����Ҫ���ö�����뵽���ն�����
	�ⲿ���̲��ܣ�Ҳ��Ӧ���޸���ֵ���Ļ��ն���
	eval()������������Value����ʹ��eval������ֵ��ʱ����Ҫ�����Ƿ���Ҫ������Value
*/
static Value* eval(Token* exp, Environment* env)
{
	Value* result = NULL;
	switch (exp->flag)
	{
	case TOKEN_FLAG_IS_BOOL:
	{
		result = DBG_NEW Value(VALUE_IS_LOGICAL);
		// ��ֵ��һ����ʱ��ֵ������ͨ��env->set����������ֵ��Ȼ���ڱ���ʽ����ɾ����ֵ
		env->parent->constant.push_back(result);
		result->value.logical = exp->value.bits;
		break;
	}
	case TOKEN_FLAG_IS_NUM:
	{
		result = DBG_NEW Value(VALUE_IS_NUM);
		// ��ֵ��һ����ʱ��ֵ������ͨ��env->set����������ֵ��Ȼ���ڱ���ʽ����ɾ����ֵ
		env->parent->constant.push_back(result);
		result->value.number = std::stod(*exp->value.str);
		break;
	}
	case TOKEN_FLAG_IS_STRING:
	{
		result = DBG_NEW Value(VALUE_IS_STRING);
		env->parent->constant.push_back(result);
		result->value.str = DBG_NEW std::string(*exp->value.str);
		break;
	}
	case TOKEN_FLAG_IS_VAR:
	{
		result = DBG_NEW Value(env->get(exp->value.str));
		env->parent->constant.push_back(result);
		//result = env->get(exp->value.str);
		break;
	}
	case TOKEN_FLAG_IS_LAMBDA:
	{
		// �������������������Ҫ�¹���һ���﷨��
		TokenLambda* token_lambda = (TokenLambda*)exp->value.body;
		result = DBG_NEW Value(VALUE_IS_LAMBDA);

		// �½�һ��ValueLambdaʵ����������ֻ�д˴����½�valuelambdaʵ��
		ValueLambda* value_lambda = DBG_NEW ValueLambda;

		// ����valuelambda��value
		result->value.lambda = value_lambda;

		// ���� value_lambda
		value_lambda->func_ptr = token_lambda;
		// ��lambda�Ļ�������Ϊ��ǰ�Ļ���
		value_lambda->env = env;

		// ����lambda���ӵ���������lambdaֵ���У����ֲ����������ٺ󣬸�����������Щʵ��
		env->parent->sequence_value_lambda.push_back(value_lambda);
		env->parent->constant.push_back(result);
		break;
	}
	case TOKEN_FLAG_IS_PROG:
	{
		/*
			���� { } ����飬�����ж����ľֲ���������������һ������ʽ���С�
		*/

		/* �������д��뽫��Ϊ { } �����Ĵ���鴴��һ�������ľֲ�����, Ϊ������������ʽһ�㣬ע�͵����� */
		Environment* new_env = DBG_NEW Environment(env);
		result = eval_prog(exp->value.prog, new_env);

		// �ж�result��һ������
		if (result->flag == VALUE_IS_LAMBDA)
		{
			ValueLambda* v_lambda = (ValueLambda*)result->value.lambda;
			if (reinterpret_cast<uint64_t>(v_lambda->env) == reinterpret_cast<uint64_t>(new_env)
				|| match_env(&new_env->env_sequence, v_lambda->env)) 
				// ���ӵ����ĸ������еĻ�������
				env->env_sequence.push_back(new_env);
			else
				delete new_env;
		}
		else
			delete new_env;

		break;
	}
	case TOKEN_FLAG_IS_CALLS:
	{
		TokenCalls* calls = (TokenCalls*)exp->value.body;
		// ʵ���б�
		std::vector<Value*> args_list;

		// ����ʵ���б�
		for (auto tokens = calls->args->begin(); tokens != calls->args->end(); tokens++)
		{
			Token* tok = *tokens;
			Value* parameter;
			// ��ȡ�����������½�һ��Value���������յ�ʱ��������Щ���ݡ�
			// eval()������������Value����ʹ��eval������ֵ��ʱ����Ҫ�����Ƿ���Ҫ������Value
			parameter = DBG_NEW Value(eval(tok, env));
			// ���ӵ������б�
			env->constant.push_back(parameter);
			args_list.push_back(parameter);
		}

		result = eval_calls(calls, args_list, env);
		break;

	}
	case TOKEN_FLAG_IS_IF:
	{
		result = eval_if((TokenIf*)exp->value.body, env);
		break;
	}
	case TOKEN_FLAG_IS_ASSIGN:
	{
		result = eval_assignment((TokenAssign*)exp->value.body, env);
		break;
	}
	case TOKEN_FLAG_IS_BINARY:
	{
		TokenBinary* token_binary = (TokenBinary*)(exp->value.body);
		result = apply_op(token_binary->op, eval(token_binary->lrvalue->left, env), eval(token_binary->lrvalue->right, env), env);
		env->parent->constant.push_back(result);
		break;
	}
	case TOKEN_FLAG_IS_DEF:
	{
		TokenDef* token_def = (TokenDef*)exp->value.body;
		result = env->def(token_def->var->value.str, eval(token_def->value, env));
		break;
	}
	case TOKEN_FLAG_IS_REF:
	{
		Token* ref_var = exp->value.ref;
		Value* ref_value = env->get(ref_var->value.str);
		result = DBG_NEW Value(VALUE_IS_REF);
		result->value.ref = ref_value;

		env->parent->constant.push_back(result);
		break;

	}
	case TOKEN_FLAG_IS_LIST:
	{

		if (exp->value.items->size() == 0)
		{
			result = DBG_NEW Value(VALUE_IS_PAIR);

			ValuePair* root_pair = DBG_NEW ValuePair;
			root_pair->front = DBG_NEW Value(VALUE_IS_NIL);
			root_pair->rear = DBG_NEW Value(VALUE_IS_NIL);
			result->value.pair = root_pair;
			env->parent->constant.push_back(result);
			break;
		}
		else
		{

			result = DBG_NEW Value(VALUE_IS_NIL);
			ValuePair* last_pair = NULL;
			auto note_value = result;

			for (auto iter_token = exp->value.items->begin(); iter_token != exp->value.items->end(); iter_token++)
			{
				note_value->flag = VALUE_IS_PAIR;
				auto tmp_pair = DBG_NEW ValuePair;
				note_value->value.pair = tmp_pair;
				last_pair = tmp_pair;

				Value* item = eval(*iter_token, env);
				tmp_pair->front = DBG_NEW Value(item);
				tmp_pair->rear = DBG_NEW Value(VALUE_IS_NIL);
				note_value = tmp_pair->rear;
			}

			env->parent->constant.push_back(result);
			break;
		}
	}
	default:
		break;
	}
	return result;
}

// ȫ�ֻ�����ʼ��
static void global_env_init()
{
	stdlib_init(&global_env->scope);

	Value* none = DBG_NEW Value(VALUE_IS_NIL);
	global_env->scope["nil"] = none;

}

void eval_root_prog(Token* root)
{
	eval_env = DBG_NEW Environment;
	global_env = DBG_NEW Environment(eval_env);
	eval_env->env_sequence.push_back(global_env);

	global_env_init();

	std::vector<Token*>::iterator exp = root->value.prog->begin();
	Value* result = NULL;
	while (exp != root->value.prog->end())
	{
		result = eval(*exp, global_env);

		// �Ƴ� cache data
		std::for_each(global_env->constant.rbegin(), global_env->constant.rend(), [](Value* value) {delete value; });
		global_env->constant.erase(global_env->constant.begin(), global_env->constant.end());

		exp++;
	}

	delete eval_env;
}
