#pragma once

#include "../../framework.h"
#include "token_atom.h"
#include "token_lrvalue.h"
#include "token_assign.h"
#include "token_binary.h"
#include "token_branch.h"
#include "token_calls.h"
#include "token_def.h"
#include "token_if.h"
#include "token_lambda.h"

enum ENUM_TOKEN
{
	TOKEN_FLAG_IS_BITS = 0,
	TOKEN_FLAG_IS_BOOL,
	TOKEN_FLAG_IS_TOKEN,
	TOKEN_FLAG_IS_NUM,
	TOKEN_FLAG_IS_STRING,
	TOKEN_FLAG_IS_LAMBDA,
	TOKEN_FLAG_IS_CALLS,
	TOKEN_FLAG_IS_IF,
	TOKEN_FLAG_IS_ASSIGN,
	TOKEN_FLAG_IS_BINARY,
	TOKEN_FLAG_IS_PROG,
	TOKEN_FLAG_IS_PUNC,
	TOKEN_FLAG_IS_OP,
	TOKEN_FLAG_IS_KW,
	TOKEN_FLAG_IS_VAR,
	TOKEN_FLAG_IS_DEF,
	TOKEN_FLAG_IS_REF,
};

#define SET_TOKEN_FLAG_TO_BOOL(token)			( (token).flag = TOKEN_FLAG_IS_BOOL )
#define SET_TOKEN_FLAG_TO_BITS(token)			( (token).flag = TOKEN_FLAG_IS_BITS )
#define SET_TOKEN_FLAG_TO_TOKEN(token)			( (token).flag = TOKEN_FLAG_IS_TOKEN )
#define SET_TOKEN_FLAG_TO_NUM(token)			( (token).flag = TOKEN_FLAG_IS_NUM )
#define SET_TOKEN_FLAG_TO_STRING(token)			( (token).flag = TOKEN_FLAG_IS_STRING )
#define SET_TOKEN_FLAG_TO_LAMBDA(token)			( (token).flag = TOKEN_FLAG_IS_LAMBDA )
#define SET_TOKEN_FLAG_TO_CALLS(token)			( (token).flag = TOKEN_FLAG_IS_CALLS )
#define SET_TOKEN_FLAG_TO_IF(token)				( (token).flag = TOKEN_FLAG_IS_IF )
#define SET_TOKEN_FLAG_TO_ASSIGN(token)			( (token).flag = TOKEN_FLAG_IS_ASSIGN )
#define SET_TOKEN_FLAG_TO_BINARY(token)			( (token).flag = TOKEN_FLAG_IS_BINARY )
#define SET_TOKEN_FLAG_TO_PROG(token)			( (token).flag = TOKEN_FLAG_IS_PROG )
#define SET_TOKEN_FLAG_TO_PUNC(token)			( (token).flag = TOKEN_FLAG_IS_PUNC )
#define SET_TOKEN_FLAG_TO_OP(token)				( (token).flag = TOKEN_FLAG_IS_OP )
#define SET_TOKEN_FLAG_TO_KW(token)				( (token).flag = TOKEN_FLAG_IS_KW )
#define SET_TOKEN_FLAG_TO_VAR(token)			( (token).flag = TOKEN_FLAG_IS_VAR )
#define SET_TOKEN_FLAG_TO_DEF(token)			( (token).flag = TOKEN_FLAG_IS_DEF )
#define SET_TOKEN_FLAG_TO_REF(token)			( (token).flag = TOKEN_FLAG_IS_REF )

#define SET_TOKEN_TYPE(token, type_name)		(memcpy((token).type, type_name, sizeof(type_name)))
