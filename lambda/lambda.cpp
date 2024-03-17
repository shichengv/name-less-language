#include "framework.h"
#include "./analyzer/input_stream.h"
#include "./analyzer/token/token.h"
#include "./analyzer/token_stream.h"
#include "./analyzer/parser.h"
#include "./configure/error_log.h"
#include "./configure/command.h"
#include "./configure/read_file.h"

// 构造可视化语法树
void construct_ast(Token* root);
// 求值器[解释器]
void eval_root_prog(Token* root);

int main(int argc, char* argv[])
{
#ifdef _FIND_MEMORY_LEAKS_USING_CRT
    _CrtMemState s1, s2, s3;
    _CrtMemCheckpoint(&s1);
#endif

    // 可视化语法树
	std::string* visualized_ast = DBG_NEW std::string;
    // 语法树根节点
    Token* root = DBG_NEW Token(TOKEN_FLAG_IS_PROG);

    read_file(visualized_ast);
    handle_options(argv);
    
    // 错误日志初始化
    error_log_init();
    // 输入流初始化
    input_stream_init(visualized_ast);
    // token流初始化
	token_stream_init();
    // 解析器初始化
    parser_init();

    // 解析代码，构造语法树
    parse_toplevel(root);

    // 构造可视化语法树
    construct_ast(root);
    // 解析语法树
    eval_root_prog(root);

    // 删除语法树
    delete root;
    // 清理解析器
    parser_clean();
    // 清理token流
	token_stream_clean();
    // 清理输入流
	input_stream_clean();
    // 清理错误日志
    error_log_clean();

#ifdef _FIND_MEMORY_LEAKS_USING_CRT
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();

    _CrtMemCheckpoint(&s2);
    if (_CrtMemDifference(&s3, &s1, &s2))
        _CrtMemDumpStatistics(&s3);

#endif
    exit(0);
}

