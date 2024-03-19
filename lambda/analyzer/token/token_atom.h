#pragma once

#include "../../framework.h"

class Token
{
public:
	Token();
	Token(uint8_t to_flag);
	//Token(const char*);
	~Token();
	
	int8_t flag;

	char type[7];
	union 
	{
		uint64_t bits;
		std::string* str;
		void* body;
		Token* ref;
		std::vector<Token*>* prog;
		
	} value;

private:

};

