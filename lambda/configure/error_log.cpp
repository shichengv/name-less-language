#include "error_log.h"

std::string* analyzer_flag;
std::string* eval_flag;

void error_log_init()
{
	analyzer_flag = DBG_NEW std::string("[ANALYZER]:[FATAL]\t");
	eval_flag = DBG_NEW std::string("[EVAL]:[ERROR]\t");
}

void throw_analyzer_exception(std::wstring& msg)
{

#ifdef WINDOWS
    _setmode(_fileno(stdout), _O_U16TEXT);
#endif // WINDOWS

	msg = L"[ANALYZER]:[FATAL]\t" + msg;
	std::wcout << msg;
	throw msg;
	exit(0);
}

void throw_eval_exception(std::wstring& msg)
{
#ifdef WINDOWS
    _setmode(_fileno(stdout), _O_U16TEXT);
#endif // WINDOWS

	msg = L"[EVAL]:[ERROR]\t" + msg;
	std::wcout << msg;
	throw msg;
	exit(0);

}

void throw_analyzer_exception(std::string& msg)
{
	msg = "[ANALYZER]:[FATAL]\t" + msg;
	std::cout << msg;
	exit(0);
}

void throw_eval_exception(std::string& msg)
{
}

void error_log_clean()
{
	delete analyzer_flag;
	delete eval_flag;
}