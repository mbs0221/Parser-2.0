#include "Parser/lexer.h"
#include "Parser/value.h"
#include "Parser/expression.h"
#include "Parser/interpreter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "测试Operator类型和determineTargetType函数..." << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 测试1：创建不同类型的Operator
    cout << "\n=== 测试Operator类型 ===" << endl;
    
    Operator* plusOp = new Operator('+', "+", 4, true);
    Operator* minusOp = new Operator('-', "-", 4, true);
    Operator* multiplyOp = new Operator('*', "*", 5, true);
    Operator* divideOp = new Operator('/', "/", 5, true);
    Operator* eqOp = new Operator(EQ, "==", 2, true);
    Operator* lessOp = new Operator('<', "<", 3, true);
    Operator* andOp = new Operator(AND, "&&", 1, true);
    Operator* orOp = new Operator(OR, "||", 1, true);
    
    cout << "Plus operator: " << plusOp->getSymbol() << ", precedence: " << plusOp->getPrecedence() << endl;
    cout << "Equal operator: " << eqOp->getSymbol() << ", precedence: " << eqOp->getPrecedence() << endl;
    cout << "And operator: " << andOp->getSymbol() << ", precedence: " << andOp->getPrecedence() << endl;
    
    // 测试2：测试determineTargetType函数
    cout << "\n=== 测试determineTargetType函数 ===" << endl;
    
    IntegerValue* intVal = new IntegerValue(42);
    DoubleValue* doubleVal = new DoubleValue(3.14);
    BoolValue* boolVal = new BoolValue(true);
    StringValue* strVal = new StringValue("hello");
    CharValue* charVal = new CharValue('A');
    
    // 算术运算
    cout << "int + double -> " << interpreter.determineTargetType(intVal, doubleVal, plusOp) << endl;
    cout << "int * bool -> " << interpreter.determineTargetType(intVal, boolVal, multiplyOp) << endl;
    cout << "char + int -> " << interpreter.determineTargetType(charVal, intVal, plusOp) << endl;
    
    // 比较运算
    cout << "int < double -> " << interpreter.determineTargetType(intVal, doubleVal, lessOp) << endl;
    cout << "string == string -> " << interpreter.determineTargetType(strVal, strVal, eqOp) << endl;
    cout << "bool < bool -> " << interpreter.determineTargetType(boolVal, boolVal, lessOp) << endl;
    
    // 逻辑运算
    cout << "int && bool -> " << interpreter.determineTargetType(intVal, boolVal, andOp) << endl;
    cout << "bool || bool -> " << interpreter.determineTargetType(boolVal, boolVal, orOp) << endl;
    
    // 测试3：测试BinaryExpression处理
    cout << "\n=== 测试BinaryExpression处理 ===" << endl;
    
    ConstantExpression* leftExpr = new ConstantExpression(10);
    ConstantExpression* rightExpr = new ConstantExpression(3.5);
    BinaryExpression* binaryExpr = new BinaryExpression(leftExpr, rightExpr, plusOp);
    
    Value* result = interpreter.visit(binaryExpr);
    cout << "BinaryExpression result: " << result->toString() << endl;
    
    // 测试4：测试比较运算
    ConstantExpression* boolExpr1 = new ConstantExpression(true);
    ConstantExpression* boolExpr2 = new ConstantExpression(false);
    BinaryExpression* compareExpr = new BinaryExpression(boolExpr1, boolExpr2, lessOp);
    
    Value* compareResult = interpreter.visit(compareExpr);
    cout << "Comparison result: " << compareResult->toString() << endl;
    
    cout << "\nOperator类型和determineTargetType函数测试完成！" << endl;
    cout << "优势：" << endl;
    cout << "1. 类型安全：Operator类型提供了类型检查" << endl;
    cout << "2. 更好的语义：操作符包含符号、优先级等信息" << endl;
    cout << "3. 统一的接口：determineTargetType直接使用Operator类型" << endl;
    cout << "4. 可扩展性：容易添加新的操作符属性" << endl;
    
    return 0;
}
