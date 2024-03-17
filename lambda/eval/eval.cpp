#include "../framework.h"
#include "../analyzer/token/token.h"
#include "../stdlib/stdlib.h"
#include "value/value.h"
#include "env.h"
#include "../configure/error_log.h"

extern Value* calls_stdlib_func(uint64_t func_addr, std::vector<Value*>* args_list);
static Environment* eval_env;
static Environment* global_env;

void eval_root_prog(Token* root);

static Value* eval(Token* exp, Environment* env);

// evaluate multiple expressions
static Value* eval_prog(std::vector<Token*>* progs, Environment* env);

static inline double use_num(Value* value)
{
	if (value->flag != VALUE_IS_NUM)
	{
		std::string error = "Expected number but got";
		std::cout << error;
		throw error;
		exit(0);
	}

	return value->value.number;
};

static inline std::string* use_str(Value* value)
{
	if (value->flag != VALUE_IS_STRING)
	{
		std::string error = "Expected string but got";
		std::cout << error;
		throw error;
		exit(0);

	}
	return value->value.str;
};

static inline uint64_t use_logical(Value* value)
{
	if (value->flag != VALUE_IS_LOGICAL)
	{
		std::string error = "Expected bool but got";
		std::cout << error;
		throw error;
		exit(0);
	}
	return value->value.logical;
};

static Value* apply_op(std::string* op, Value* left, Value* right, Environment* env)
{
	Value* result = DBG_NEW Value;
	// 为了保持统一，我们不在apply_op添加回收动作，回收动作只能由eval函数设置
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
		std::wstring error = L"\x8d4b\x503c\x8bed\x53e5\x7684\x5de6\x4fa7\x5fc5\x987b\x4e3a\x4e00\x4e2a\x53d8\x91cf\xff5e\x28\x2220\x30fb\x3c9\x3c\x20\x29\x2312\x266a";
		throw_eval_exception(error);
	}
	/*	
	NOTICE:
		对于 赋值 等式，解释器在此构建一个新值用来返回，这种情况用来应对与当前环境的最后一句表达式为赋值等式，解释器构建一个新值
		并将新值添加到临时数据回收器，这样，当前环境的父环境可以直接使用该返回值。
	*/
	Value* result = DBG_NEW Value( env->set(lrvalue->left->value.str, eval(lrvalue->right, env)));
	env->parent->constant.push_back(result);
	return result;
	//return env->set(lrvalue->left->value.str, eval(lrvalue->right, env));
}

// 如果lambda的env在当前环境的待回收的环境序列中，解释器也不回收当前环境。这种情况用于应对局部环境嵌套
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

		// 移除 cache data
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
	// 通过调用的函数名字来获得该函数对应的lambda实体
	Value* func = env->get(calls->func->value.str);
	if (func->flag == VALUE_IS_STD_FUNC_OFFSET)
	{
		result = calls_stdlib_func(func->value.bits, &args_list);
		env->parent->constant.push_back(result);
	}
	else
	{
		ValueLambda* value_lambda = (ValueLambda*)func->value.lambda;
		// 调用通过rcl语言定义的函数
		// 如果实参数量不等于形参数量，就抛出错误
		if (calls->args->size() != value_lambda->func_ptr->args->size())
			throw "The expected number of parameters received does not match the actual number of parameters passed in...";

		// 为新函数构造一个环境
		Environment* new_env = new Environment(value_lambda->env);
		auto args = value_lambda->func_ptr->args->begin(); 
		auto parameter = args_list.begin();
		while ( args != value_lambda->func_ptr->args->end())
		{
			// 在新构造的环境中设置改符号，以及其关联的值
			new_env->def(*args, *parameter);
			
			// 迭代器++
			args++;
			parameter++;
		}
		// 获取函数的表达式序列，解释执行
		Token* token_func_prog = (Token*)value_lambda->func_ptr->body;
		if (token_func_prog->flag == TOKEN_FLAG_IS_PROG)
		{
			// 虽然可以使用eval直接进行求值，但是如果eval函数检测到token为prog时会再次创建一个新环境，对于函数调用来说
			// 这是没有必要的，所以此时需要获取表达式序列，手动调用eval_prog
			std::vector<Token*>* func_expressions = (std::vector<Token*>*)token_func_prog->value.prog;
			tmp_result = eval_prog(func_expressions, new_env);
		}
		else
		{
			tmp_result = eval(token_func_prog, new_env);
		}
		result = DBG_NEW Value(tmp_result);
		env->parent->constant.push_back(result);

		// 判断result是一个函数
		if (result->flag == VALUE_IS_LAMBDA)
		{
			ValueLambda* v_lambda = (ValueLambda*)result->value.lambda;
			if (reinterpret_cast<uint64_t>(v_lambda->env) == reinterpret_cast<uint64_t>(new_env)
				|| match_env(&new_env->env_sequence, v_lambda->env)) 
				// 添加到它的父环境中的环境序列
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
		一个取巧的设计，解析分支条件时并不检测它的条件结果类型，只判断他的位向量是否为0。
		对于 false 或 nil 类型，它们的位向量始终为 0， 所以采取 else分支。由于数字是
		由double类型存储，所以即使函数返回值为0，条件也是成立的。

	*/

	/* 下面这两行代码可以被注释	*/
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
	求值器过程
	一个简单的约束，如果求值器内部调用了外部的过程，而外部的过程申请了value对象，求值器需要将该对象加入到回收队列中
	外部过程不能，也不应该修改求值器的回收队列
	eval()函数并不保存Value，在使用eval函数赋值的时候需要考虑是否需要拷贝该Value
*/
static Value* eval(Token* exp, Environment* env)
{
	Value* result = NULL;
	switch (exp->flag)
	{
	case TOKEN_FLAG_IS_BOOL:
	{
		result = DBG_NEW Value(VALUE_IS_LOGICAL);
		// 该值是一个临时的值，后续通过env->set方法拷贝该值，然后在表达式结束删除该值
		env->parent->constant.push_back(result);
		result->value.logical = exp->value.bits;
		break;
	}
	case TOKEN_FLAG_IS_NUM:
	{
		result = DBG_NEW Value(VALUE_IS_NUM);
		// 该值是一个临时的值，后续通过env->set方法拷贝该值，然后在表达式结束删除该值
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
		// 对于这种情况，我们需要新构造一个语法树
		TokenLambda* token_lambda = (TokenLambda*)exp->value.body;
		result = DBG_NEW Value(VALUE_IS_LAMBDA);

		// 新建一个ValueLambda实例，代码中只有此处会新建valuelambda实例
		ValueLambda* value_lambda = DBG_NEW ValueLambda;

		// 连接valuelambda到value
		result->value.lambda = value_lambda;

		// 设置 value_lambda
		value_lambda->func_ptr = token_lambda;
		// 将lambda的环境设置为当前的环境
		value_lambda->env = env;

		// 将该lambda添加到父环境的lambda值序列，当局部环境被销毁后，父环境销毁这些实例
		env->parent->sequence_value_lambda.push_back(value_lambda);
		env->parent->constant.push_back(result);
		break;
	}
	case TOKEN_FLAG_IS_PROG:
	{
		/*
			对于 { } 代码块，它具有独立的局部环境，它不仅是一个表达式序列。
		*/

		/* 下面两行代码将会为 { } 包裹的代码块创建一个独立的局部环境, 为了让它更函数式一点，注释掉它们 */
		Environment* new_env = DBG_NEW Environment(env);
		result = eval_prog(exp->value.prog, new_env);

		// 判断result是一个函数
		if (result->flag == VALUE_IS_LAMBDA)
		{
			ValueLambda* v_lambda = (ValueLambda*)result->value.lambda;
			if (reinterpret_cast<uint64_t>(v_lambda->env) == reinterpret_cast<uint64_t>(new_env)
				|| match_env(&new_env->env_sequence, v_lambda->env)) 
				// 添加到它的父环境中的环境序列
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
		// 实参列表
		std::vector<Value*> args_list;

		// 构造实参列表
		for (auto tokens = calls->args->begin(); tokens != calls->args->end(); tokens++)
		{
			Token* tok = *tokens;
			Value* parameter;
			// 获取参数，这里新建一个Value，参数回收的时候会清除这些数据。
			// eval()函数并不保存Value，在使用eval函数赋值的时候需要考虑是否需要拷贝该Value
			parameter = DBG_NEW Value(eval(tok, env));
			// 添加到参数列表
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

// 全局环境初始化
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

		// 移除 cache data
		std::for_each(global_env->constant.rbegin(), global_env->constant.rend(), [](Value* value) {delete value; });
		global_env->constant.erase(global_env->constant.begin(), global_env->constant.end());

		exp++;
	}

	delete eval_env;
}
