#include "Parser/ast_visitor.h"
#include "parser/inter.h"
#include <iostream>

int main() {
    // 创建一个简单的AST
    auto* intExpr = new IntExpression(42);
    auto* idExpr = new IdentifierExpression("x");
    auto* addExpr = new ArithmeticExpression(intExpr, idExpr, new Token('+', "PLUS"));
    
    // 创建访问者
    PrettyPrintVisitor visitor;
    
    // 使用访问者打印AST
    std::cout << "IntExpression: " << intExpr->accept(&visitor) << std::endl;
    std::cout << "IdentifierExpression: " << idExpr->accept(&visitor) << std::endl;
    std::cout << "ArithmeticExpression: " << addExpr->accept(&visitor) << std::endl;
    
    // 清理内存
    delete intExpr;
    delete idExpr;
    delete addExpr;
    
    return 0;
}
