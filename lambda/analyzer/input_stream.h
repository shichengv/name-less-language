#pragma once

#include "../../framework.h"

void input_stream_init(const std::string* input);
void input_stream_clean();

char input_next();
bool input_eof();
char input_predict();
char input_peek();
void input_croak(std::string msg);
void input_croak(std::wstring& msg);
