#include "command.h"

InterpreterOptions::InterpreterOptions()
{
	generate_visualized_ast = 0;
	print_help = 0;
	eval_the_source = 1;
	has_error = 0;
}

InterpreterOptions::~InterpreterOptions()
{
}

void print_help()
{
	char option[24] = { 0 };

	std::string hello_msg;

	hello_msg += "\n欢迎使用 name-less language 解析器：\n";

	sprintf_s(option, "%-20s", "\t-h --help");
	hello_msg += option;
	hello_msg += " : 获取帮助\n";

	sprintf_s(option, "%-20s", "\t-ast");
	hello_msg += option;
	hello_msg += " : 生成语法树文件\n";

	sprintf_s(option, "%-20s", "\t-i <filename>");
	hello_msg += option;
	hello_msg += " : 读取文件\n";

	sprintf_s(option, "%-20s", "\t-no-exec");
	hello_msg += option;
	hello_msg += " : 解析但不执行\n";

	std::cout << hello_msg << std::endl;
}

void print_version()
{

	std::cout << "Version: 0.01" << std::endl;
}

void handle_options(int argc, char* argv[], InterpreterOptions* options)
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << "\t[-h]" << std::endl;
		options->has_error = 1;
	}

	for (int i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
		{
			options->print_help = 1;
			print_help();
			break;
		}
		else if (!strcmp(argv[i], "-i"))
		{
			options->filename = argv[++i];
		}
		else if (!strcmp(argv[i], "-v"))
		{
			options->print_version = 1;
			print_version();
			break;
		}
		else if (!strcmp(argv[i], "-ast"))
		{
			options->generate_visualized_ast = 1;
		}
		else if (!strcmp(argv[i], "--no-exec"))
		{
			options->eval_the_source = 0;
		}
		else
		{
			std::cerr << "Usage: " << argv[0] << " [-h] " << std::endl;
			options->has_error = 1;
		}

	}


}
