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
		Interpreter interpreter;
		interpreter.execute(program);
	} else {
		printf("解析失败\n");
	}
	return 0;
}