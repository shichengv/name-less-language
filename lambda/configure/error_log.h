#pragma once

#include "../framework.h"

void error_log_init();
void throw_analyzer_exception(std::wstring& msg);
void throw_eval_exception(std::wstring& msg);
void throw_analyzer_exception(std::string& msg);
void throw_eval_exception(std::string& msg);
void error_log_clean();
