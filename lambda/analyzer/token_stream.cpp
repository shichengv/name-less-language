#include "token_stream.h"
#include "input_stream.h"
#include "../configure/error_log.h"

static Token* current;
static std::string* keywords;
static std::string* digit;
static std::string* id_start;
static std::string* id;
static std::string* op_char;
static std::string* punc;
static std::string* whitespace;
static bool has_dot;
static bool is_negative_num;

static bool is_keyword(const std::string& x);
static bool is_digit(char ch);
static bool is_id_start(char ch);
static bool is_id(char ch);
static bool is_op_char(char ch);
static bool is_punc(char ch);
static bool is_whitespace(char ch);

static std::string* read_while(bool(*predicate)(char ch));
static Token* read_number();
static Token* read_ident();
static std::string* read_escaped(char end);
static Token* read_string();
static void skip_comment();
static Token* read_next();

void token_stream_init()
{
	keywords = DBG_NEW std::string(" if then else lambda λ true false def list ");
	digit = DBG_NEW std::string("[0-9]");
	id_start = DBG_NEW std::string("[a-zλ_]");
	id = DBG_NEW std::string("?!-<>=0123456789");
	op_char = DBG_NEW std::string("+-*/%=&|<>!");
	punc = DBG_NEW std::string(",;(){}[]");
	whitespace = DBG_NEW std::string(" \t\n");

	current = NULL;
	has_dot = false;
    is_negative_num = false;
}

void token_stream_clean()
{
	delete keywords;
	delete digit;
	delete id_start;
	delete id;
	delete op_char;
	delete punc;
	delete whitespace;
}

static bool is_decimal(char ch) {
    if (ch == '.')
    {
        if (has_dot)
            return false;
        has_dot = true;
        return true;
    }
    return is_digit(ch);
}

static bool is_not_lf(char ch) {
    return ch != '\n';
}

static bool is_keyword(const std::string& x)
{
    return (keywords->find(" " + x + " ") != std::string::npos);
}

// If it's a digit, then we proceed to read a number.
static bool is_digit(char ch) {
    // Create a regular expression object that matches a digit
    std::regex regex_digit(*digit, std::regex_constants::icase);
    // Return the result of testing the character against the regular expression
    return std::regex_match(std::string(1, ch), regex_digit);
}

// identifier start
static bool is_id_start(char ch)
{
    std::regex regex_id_start(*id_start, std::regex_constants::icase);
    return std::regex_match(std::string(1, ch), regex_id_start);
}

// identifier
static bool is_id(char ch)
{
    return (is_id_start(ch) || (id->find(ch) != std::string::npos));
}


// If it's one of the operator characters, return an operator token.
static bool is_op_char(char ch)
{
    return (op_char->find(ch) != std::string::npos);
}


// If it's one of the punctuation characters, return a punctuation token.
static bool is_punc(char ch)
{
    return punc->find(ch) != std::string::npos;
}

// skip over whitespace
static bool is_whitespace(char ch)
{
    return whitespace->find(ch) != std::string::npos;
}

// 按照Predicate方法一直读取，返回读取的字符串
static std::string* read_while(bool(*predicate)(char ch))
{
    has_dot = false;
    std::string* str = DBG_NEW std::string("");
    while (!input_eof() && predicate(input_peek()))
        *str += input_next();
    return str;
}

// 读取数字，返回存储数字的Token
static Token* read_number()
{
    Token* tok = DBG_NEW Token(TOKEN_FLAG_IS_NUM);
    tok->value.str = read_while(is_decimal);
    return tok;
}

// 读取identifier
static Token* read_ident()
{
    Token* tok;
    std::string* token_id = read_while(is_id); 

    if (is_keyword(*token_id))
    {
		tok = DBG_NEW Token(TOKEN_FLAG_IS_KW);
        tok->value.str = token_id;
    }
    else
    {
		tok = DBG_NEW Token(TOKEN_FLAG_IS_VAR);
        tok->value.str = token_id;
    }

    return tok;
}

static std::string* read_escaped(char end)
{
    bool escaped = false;
    std::string* str = DBG_NEW std::string("");
    input_next();

    while (!input_eof())
    {
        char ch = input_next();
        if (escaped)
        {
            switch (ch)
            {
            case 'n':
                *str += '\n';
                break;
            case 't':
                *str += '\t';
                break;
            case 'a':
                *str += '\a';
                break;
            case 'b':
                *str += '\b';
                break;
            case 'f':
                *str += '\f';
                break;
            case 'r':
                *str += '\r';
                break;
            case 'v':
                *str += '\v';
                break;
            case '\\':
                *str += '\\';
                break;
            case '\'':
                *str += '\'';
                break;
            case '\"':
                *str += '\"';
                break;
            case '\?':
                *str += '\?';
                break;
            case '\0':
                *str += '\0';
                break;
            //case '\ddd':
            //    *str += '\ddd';
            //    break;
            //case '\x':
            //    *str += '\x';
            //    break;

            default:
                break;
            }
            escaped = false;
        }
        else if (ch == '\\')
        {
            escaped = true;
        }
        else if (ch == end)
        {
            break;
        }
        else
        {
            *str += ch;
        }
    }

    return str;
}

// if it's a quote then read a string
static Token* read_string()
{
    Token* tok = DBG_NEW Token(TOKEN_FLAG_IS_STRING);
    tok->value.str = read_escaped('"');
    return tok;
}

// If it's a sharp sign (#), skip comment (retry after the end of line).
static void skip_comment()
{
    std::string* comment = read_while(is_not_lf);
    delete comment;
    comment = NULL;

    input_next();
}

static Token* read_next()
{

    std::string* whitespace = read_while(is_whitespace);
    delete whitespace;
    whitespace = NULL;

    if (input_eof())
        return NULL;

    char ch = input_peek();

    if (ch == '#')
    {
        skip_comment();
        return read_next();
    }

    if (ch == '"')
    {
        return read_string();
    }

    if (ch == '-')
    {
        char predict_char = input_predict();
        // 是一个负数
        if (is_digit(predict_char))
        {
            std::string* negative_num = DBG_NEW std::string("-");
            input_next();
            // 读取数字
            Token* num = read_number();

            *negative_num += *num->value.str;
            // 删除原先的字符串
            delete num->value.str;
            num->value.str = negative_num;
            return num;
        }
        // 是一个操作符
        else
        {
			Token* tok = DBG_NEW Token(TOKEN_FLAG_IS_OP);
			tok->value.str = read_while(is_op_char);
			return tok;
        }
    }

    if (ch == '&')
    {
        char predict_char = input_predict();
        // 是操作符
        if (is_op_char(predict_char))
        {
			Token* tok = DBG_NEW Token(TOKEN_FLAG_IS_OP);
			tok->value.str = read_while(is_op_char);
			return tok;
        }
        else if(is_id_start(predict_char))
        {
            Token* tok = DBG_NEW Token(TOKEN_FLAG_IS_REF);
            input_next();
            tok->value.ref = read_ident();
            return tok;
        }
        else
        {
			std::wstring error = L"引用后面记得跟上需要引用的变量名哦\x266A~";
			input_croak(error);
            return NULL;
        }
    }

    if (is_digit(ch))
    {
        return read_number();
    }

    if (is_id_start(ch))
    {
		return read_ident();
    }

    if (is_punc(ch))
    {
        Token* tok = DBG_NEW Token(TOKEN_FLAG_IS_PUNC);
        tok->value.str = DBG_NEW std::string;
        *tok->value.str = input_next();

        return tok;
    }

    if (is_op_char(ch))
    {
        Token* tok = DBG_NEW Token(TOKEN_FLAG_IS_OP);
        tok->value.str = read_while(is_op_char);
        return tok;
    }

    std::string error;
    error += "Can't handle character: " + ch;

    input_croak(error);


    return NULL;

}

/*

   The next() function doesn't always call read_next(), because it
might have been peeked before (in which case read_next() was already
and the stream advanced).

*/
Token* token_next()
{
    Token* token = current;
    current = NULL;
    if (token)
    {
        return token;
    }
    else
    {
        return read_next();
    }
}
Token* token_peek()
{
    if (current)
    {
        return current;
    }
    else
    {
        current = read_next();
        return current;
    }
}
bool token_eof()
{
    return token_peek() == NULL;
}
