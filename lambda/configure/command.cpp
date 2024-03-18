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
    printf("\n欢迎使用 name-less language 解析器：\n");
    printf("%-20s : %s\n", "\t-h --help", "获取帮助");
    printf("%-20s : %s\n", "\t-ast", "生成语法树文件\\0");
    printf("%-20s : %s\n", "\t-i <filename>", "读取文件");
    printf("%-20s : %s\n\n", "\t-no-exec", "解析但不执行");

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
