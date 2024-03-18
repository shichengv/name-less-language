#include "error_log.h"

void throw_analyzer_exception(std::wstring& msg)
{

#ifdef WINDOWS
    _setmode(_fileno(stdout), _O_U16TEXT);
#endif // WINDOWS

	msg = L"[ANALYZER]:[FATAL]\t" + msg;
	std::wcout << msg;
#ifdef WINDOWS
	throw msg;
#endif // WINDOWS
	exit(0);
}

void throw_eval_exception(std::wstring& msg)
{
#ifdef WINDOWS
    _setmode(_fileno(stdout), _O_U16TEXT);
#endif // WINDOWS

	msg = L"[EVAL]:[ERROR]\t" + msg;
	std::wcout << msg;
#ifdef WINDOWS
	throw msg;
#endif // WINDOWS
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

