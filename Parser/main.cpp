#include "parser.h"
#include "interpreter.h"
#include <iostream>
#include <string>

using namespace std;

// 测试
int main(int argc, char *argv[])
{
	if (argc < 2){
		printf("Usage: %s <file>\n", argv[0]);
		return 1;
	}
	Parser p;
	string str(argv[1]);
	Program *program = p.parse(str);
	if (program != nullptr){
		printf("解析成功，生成AST\n");
		printf("%s\n", program->toString().c_str());
		printf("AST已生成，可以使用解释器执行\n");
		Interpreter interpreter;
		interpreter.execute(program);
		printf("解释器执行完成\n");
	} else {
		printf("解析失败\n");
	}
	return 0;
}