#include <iostream>
#include <string>
#include "parser/parser.h"
#include "lexer/lexer.h"
#include "lexer/value.h"

using namespace std;

int test_parser_helpers() {
    cout << "测试Parser的Helper方法" << endl;
    
    // 测试matchOperator
    cout << "1. 测试matchOperator方法" << endl;
    // 这里需要创建一个Parser实例并测试
    
    // 测试matchInt
    cout << "2. 测试matchInt方法" << endl;
    
    // 测试matchDouble
    cout << "3. 测试matchDouble方法" << endl;
    
    // 测试matchChar
    cout << "4. 测试matchChar方法" << endl;
    
    // 测试matchBool
    cout << "5. 测试matchBool方法" << endl;
    
    // 测试matchString
    cout << "6. 测试matchString方法" << endl;
    
    // 测试matchIdentifier
    cout << "7. 测试matchIdentifier方法" << endl;
    
    cout << "测试完成" << endl;
    return 0;
}
