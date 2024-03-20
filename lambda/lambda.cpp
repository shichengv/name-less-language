#include "framework.h"
#include "./analyzer/input_stream.h"
#include "./analyzer/token/token.h"
#include "./analyzer/token_stream.h"
#include "./analyzer/parser.h"
#include "./configure/error_log.h"
#include "./configure/command.h"

// ������ӻ��﷨��
std::string* construct_ast(Token* root);
// ��ֵ��[������]
void eval_root_prog(Token* root);

static std::string* file_contents;
static InterpreterOptions* options;
// �﷨�����ڵ�
static Token* root;

static void interpreter_init() 
{
    file_contents = DBG_NEW std::string;
    root = DBG_NEW Token(TOKEN_FLAG_IS_PROG);

    read_file(options->filename, file_contents);
    
    // ��������ʼ��
    input_stream_init(file_contents);
    // token����ʼ��
	token_stream_init();
    // ��������ʼ��
    parser_init();
}

static void interpreter_clean()
{

    // ɾ���﷨��
    if (root)
		delete root;
    // ���������
    parser_clean();
    // ����token��
	token_stream_clean();
    // ������������������� file_contents
	input_stream_clean();

}

int main(int argc, char* argv[])
{

#ifdef _FIND_MEMORY_LEAKS_USING_CRT
    _CrtMemState s1, s2, s3;
    _CrtMemCheckpoint(&s1);
#endif

#ifdef WINDOWS
    SetConsoleOutputCP(65001);
#endif // WINDOWS


#ifndef DEBUG

    options = DBG_NEW InterpreterOptions;
    handle_options(argc, argv, options);

    if (options->has_error)
        goto done;

    if (options->filename != "")
    {
        interpreter_init();
		parse_toplevel(root);

        if (options->generate_visualized_ast)
        {
			// ������ӻ��﷨��
			std::string* visualized_ast = construct_ast(root);
            // Find the position of the dot (.)
            size_t dotPosition = options->filename.find_last_of('.');
            std::string ast_filename;

            if (dotPosition != std::string::npos) {
                ast_filename = options->filename.substr(0, dotPosition);
                ast_filename += "-ast.txt";
            }
            else
                ast_filename = options->filename + "-ast.txt";

            // Create an output file stream
            std::ofstream outputFile(ast_filename); 
            if (outputFile.is_open()) {
                outputFile << *visualized_ast << std::endl;
                outputFile.close();
            }
            else 
                std::cerr << "Error opening the file." << std::endl;

            delete visualized_ast;
        }

        if (options->eval_the_source)
			// �����﷨��
			eval_root_prog(root);
        
        interpreter_clean();

    }


done:
    delete options;

#else
/*
    Debugʹ�ã���Ҫ���� DEBUG �ꡣ
    Visual Studio Debug��������Խ�������Bug�Լ����ԣ�����ʱ��Ҫע�� DEBUG �ꡣ
*/

    file_contents = DBG_NEW std::string;
    root = DBG_NEW Token(TOKEN_FLAG_IS_PROG);

    std::ifstream file("example\\machine-with-register\\machine.nl");

    if (file.is_open()) {
        // Read the file line by line and append to the content
        std::string line;
        while (std::getline(file, line)) 
            *file_contents += line + "\n";
       
        file.close();
    }
    else 
        std::cout << "Unable to open the file.\n";
    
;
    // ��������ʼ��
    input_stream_init(file_contents);
    // token����ʼ��
	token_stream_init();
    // ��������ʼ��
    parser_init();

    parse_toplevel(root);
	std::string* visualized_ast = construct_ast(root);
	eval_root_prog(root);
	delete visualized_ast;

    // ɾ���﷨��
    if (root)
		delete root;
    // ���������
    parser_clean();
    // ����token��
	token_stream_clean();
    // ������������������� file_contents
	input_stream_clean();
#endif // 




#ifdef _FIND_MEMORY_LEAKS_USING_CRT
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();

    _CrtMemCheckpoint(&s2);
    if (_CrtMemDifference(&s3, &s1, &s2))
        _CrtMemDumpStatistics(&s3);

#endif

    exit(0);
}

