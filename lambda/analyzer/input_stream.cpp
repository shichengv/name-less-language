#include "input_stream.h"
#include "../configure/error_log.h"

static const std::string* input;
static size_t input_size = 0;
static size_t pos = 0;
static size_t last_row_start = 0;
static size_t line = 1;
static size_t col = 0;

void input_stream_init(const std::string* input_stream)
{
	input = input_stream;
	input_size = input->size();
}
void input_stream_clean()
{
	delete input;
}

char input_next()
{
	char ch = input->at(pos++);
	if (ch == '\n')
	{
		last_row_start = pos - col - 1;
		line++;
		col = 0;
	}
	else
	{
		col++;
	}
	return ch;
}

char input_predict()
{
	size_t old_pos = pos++;
	size_t old_line = line;
	size_t old_col = col;
	char predict = input_next();

	pos = old_pos;
	line = old_line;
	col = old_col;
	return predict;
}

bool input_eof()
{
	return input_peek() == '\0';
}

char input_peek()
{
	if (pos >= input_size)
	{
		return '\0';
	}
	return input->at(pos);
}

static std::wstring get_current_row_wstring()
{
	std::wstring row;

	size_t row_start = last_row_start;
	char ch = '\0';
	do
	{
		ch = input->at(row_start++);
		row += ch;

	} while (ch != '\n');
	
	return row;
}

static std::string get_current_row()
{
	std::string row;

	size_t row_start = last_row_start;
	char ch = '\0';
	do
	{
		ch = input->at(row_start++);
		row += ch;

	} while (ch != '\n');

	return row;
}

void input_croak(std::string& msg)
{
	msg += " (" + std::to_string(line - 1) + ":" + std::to_string(col) + ")\n\n";
	msg += get_current_row();

	throw_analyzer_exception(msg);
}

void input_croak(std::wstring& msg)
{
	msg += L" (" + std::to_wstring(line - 1) + L":" + std::to_wstring(col) + L")\n\n";
	msg += get_current_row_wstring();

	throw_analyzer_exception(msg);
}

