#pragma once
#include "../framework.h"

class InterpreterOptions
{
public:
	InterpreterOptions();
	~InterpreterOptions();

	uint64_t generate_visualized_ast : 1;
	uint64_t eval_the_source : 1;
	uint64_t print_help : 1;
	uint64_t has_error : 1;
	uint64_t print_version : 1;

	std::string filename;
	

private:

};



void print_help();
void print_version();
void read_file(std::string& filename, std::string* data);
void handle_options(int argc, char* argv[], InterpreterOptions* options);
