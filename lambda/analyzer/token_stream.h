#pragma once
#include "../framework.h"
#include "input_stream.h"
#include "./token/token.h"

void token_stream_init();
void token_stream_clean();

Token* token_next();
Token* token_peek();
bool token_eof();
