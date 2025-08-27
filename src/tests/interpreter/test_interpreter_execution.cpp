#include "Parser/inter.h"
#include "Parser/interpreter.h"
#include "Parser/lexer.h"
#include <iostream>
#include <memory>

using namespace std;

// 创建测试用的Token
Token* createNumberToken(int value) {
    return new Integer(value);
}

Token* createStringToken(const string& value) {
    return new Word(STRING, value);
}

Token* createIdentifierToken(const string& name) {
    return new Word(ID, name);
}

Token* createOperatorToken(const string& op) {
    return new Word(op[0], op);
}

int main() {
    cout << "=== 解释器执行测试 ===" << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 测试1：创建简单的程序 - 变量声明和打印
    cout << "\n1. 测试变量声明和打印:" << endl;
    {
        // 创建程序
        Program* program = new Program();
        
        // 创建变量声明：let message = "Hello World";
        IdentifierNode* varId = new IdentifierNode(createIdentifierToken("message"));
        StringLiteral* varValue = new StringLiteral("Hello World");
        VariableDeclaration* varDecl = new VariableDeclaration(varId, varValue);
        program->addStatement(varDecl);
        
        // 创建打印语句：print(message);
        IdentifierNode* printId = new IdentifierNode(createIdentifierToken("print"));
        IdentifierExpression* printExpr = new IdentifierExpression(printId);
        CallExpression* printCall = new CallExpression(printExpr);
        printCall->addArgument(new IdentifierExpression(varId));
        ExpressionStatement* printStmt = new ExpressionStatement(printCall);
        program->addStatement(printStmt);
        
        // 执行程序
        interpreter.execute(program);
        
        delete program;
    }
    
    // 测试2：字符串运算
    cout << "\n2. 测试字符串运算:" << endl;
    {
        Program* program = new Program();
        
        // let str1 = "Hello, ";
        // let str2 = "World!";
        // let result = str1 + str2;
        // print(result);
        
        IdentifierNode* str1Id = new IdentifierNode(createIdentifierToken("str1"));
        StringLiteral* str1Value = new StringLiteral("Hello, ");
        VariableDeclaration* str1Decl = new VariableDeclaration(str1Id, str1Value);
        program->addStatement(str1Decl);
        
        IdentifierNode* str2Id = new IdentifierNode(createIdentifierToken("str2"));
        StringLiteral* str2Value = new StringLiteral("World!");
        VariableDeclaration* str2Decl = new VariableDeclaration(str2Id, str2Value);
        program->addStatement(str2Decl);
        
        // 创建字符串拼接表达式
        IdentifierExpression* str1Expr = new IdentifierExpression(str1Id);
        IdentifierExpression* str2Expr = new IdentifierExpression(str2Id);
        Token* plusOp = createOperatorToken("+");
        ArithmeticExpression* concatExpr = new ArithmeticExpression(str1Expr, plusOp, str2Expr);
        
        IdentifierNode* resultId = new IdentifierNode(createIdentifierToken("result"));
        VariableDeclaration* resultDecl = new VariableDeclaration(resultId, concatExpr);
        program->addStatement(resultDecl);
        
        // 打印结果
        IdentifierNode* printId = new IdentifierNode(createIdentifierToken("print"));
        IdentifierExpression* printExpr = new IdentifierExpression(printId);
        CallExpression* printCall = new CallExpression(printExpr);
        printCall->addArgument(new IdentifierExpression(resultId));
        ExpressionStatement* printStmt = new ExpressionStatement(printCall);
        program->addStatement(printStmt);
        
        interpreter.execute(program);
        
        delete program;
    }
    
    // 测试3：数组操作
    cout << "\n3. 测试数组操作:" << endl;
    {
        Program* program = new Program();
        
        // 创建数组：[1, 2, 3, 4, 5]
        ArrayNode* array = new ArrayNode();
        array->addElement(new StringLiteral("apple"));
        array->addElement(new StringLiteral("banana"));
        array->addElement(new StringLiteral("cherry"));
        
        IdentifierNode* arrId = new IdentifierNode(createIdentifierToken("fruits"));
        VariableDeclaration* arrDecl = new VariableDeclaration(arrId, array);
        program->addStatement(arrDecl);
        
        // 访问数组元素：fruits[0]
        IdentifierExpression* arrExpr = new IdentifierExpression(arrId);
        StringLiteral* indexExpr = new StringLiteral("0");
        AccessExpression* accessExpr = new AccessExpression(arrExpr, indexExpr, false);
        
        IdentifierNode* firstId = new IdentifierNode(createIdentifierToken("first"));
        VariableDeclaration* firstDecl = new VariableDeclaration(firstId, accessExpr);
        program->addStatement(firstDecl);
        
        // 打印第一个元素
        IdentifierNode* printId = new IdentifierNode(createIdentifierToken("print"));
        IdentifierExpression* printExpr = new IdentifierExpression(printId);
        CallExpression* printCall = new CallExpression(printExpr);
        printCall->addArgument(new IdentifierExpression(firstId));
        ExpressionStatement* printStmt = new ExpressionStatement(printCall);
        program->addStatement(printStmt);
        
        interpreter.execute(program);
        
        delete program;
    }
    
    // 测试4：字典操作
    cout << "\n4. 测试字典操作:" << endl;
    {
        Program* program = new Program();
        
        // 创建字典：{"name": "John", "age": "30"}
        DictNode* dict = new DictNode();
        dict->setEntry("name", new StringLiteral("John"));
        dict->setEntry("age", new StringLiteral("30"));
        dict->setEntry("city", new StringLiteral("New York"));
        
        IdentifierNode* dictId = new IdentifierNode(createIdentifierToken("person"));
        VariableDeclaration* dictDecl = new VariableDeclaration(dictId, dict);
        program->addStatement(dictDecl);
        
        // 访问字典元素：person["name"]
        IdentifierExpression* dictExpr = new IdentifierExpression(dictId);
        StringLiteral* keyExpr = new StringLiteral("name");
        AccessExpression* accessExpr = new AccessExpression(dictExpr, keyExpr, false);
        
        IdentifierNode* nameId = new IdentifierNode(createIdentifierToken("name"));
        VariableDeclaration* nameDecl = new VariableDeclaration(nameId, accessExpr);
        program->addStatement(nameDecl);
        
        // 打印姓名
        IdentifierNode* printId = new IdentifierNode(createIdentifierToken("print"));
        IdentifierExpression* printExpr = new IdentifierExpression(printId);
        CallExpression* printCall = new CallExpression(printExpr);
        printCall->addArgument(new IdentifierExpression(nameId));
        ExpressionStatement* printStmt = new ExpressionStatement(printCall);
        program->addStatement(printStmt);
        
        interpreter.execute(program);
        
        delete program;
    }
    
    // 测试5：字符串字符访问
    cout << "\n5. 测试字符串字符访问:" << endl;
    {
        Program* program = new Program();
        
        // let text = "Hello World";
        StringNode* textNode = new StringNode("Hello World");
        IdentifierNode* textId = new IdentifierNode(createIdentifierToken("text"));
        VariableDeclaration* textDecl = new VariableDeclaration(textId, textNode);
        program->addStatement(textDecl);
        
        // let firstChar = text[0];
        IdentifierExpression* textExpr = new IdentifierExpression(textId);
        StringLiteral* indexExpr = new StringLiteral("0");
        AccessExpression* accessExpr = new AccessExpression(textExpr, indexExpr, false);
        
        IdentifierNode* charId = new IdentifierNode(createIdentifierToken("firstChar"));
        VariableDeclaration* charDecl = new VariableDeclaration(charId, accessExpr);
        program->addStatement(charDecl);
        
        // 打印第一个字符
        IdentifierNode* printId = new IdentifierNode(createIdentifierToken("print"));
        IdentifierExpression* printExpr = new IdentifierExpression(printId);
        CallExpression* printCall = new CallExpression(printExpr);
        printCall->addArgument(new IdentifierExpression(charId));
        ExpressionStatement* printStmt = new ExpressionStatement(printCall);
        program->addStatement(printStmt);
        
        interpreter.execute(program);
        
        delete program;
    }
    
    // 测试6：条件语句（简化版）
    cout << "\n6. 测试条件语句:" << endl;
    {
        Program* program = new Program();
        
        // let condition = "true";
        StringLiteral* conditionValue = new StringLiteral("true");
        IdentifierNode* conditionId = new IdentifierNode(createIdentifierToken("condition"));
        VariableDeclaration* conditionDecl = new VariableDeclaration(conditionId, conditionValue);
        program->addStatement(conditionDecl);
        
        // 创建条件语句块
        BlockStatement* thenBlock = new BlockStatement();
        StringLiteral* thenMsg = new StringLiteral("条件为真");
        IdentifierNode* thenPrintId = new IdentifierNode(createIdentifierToken("print"));
        IdentifierExpression* thenPrintExpr = new IdentifierExpression(thenPrintId);
        CallExpression* thenPrintCall = new CallExpression(thenPrintExpr);
        thenPrintCall->addArgument(thenMsg);
        ExpressionStatement* thenPrintStmt = new ExpressionStatement(thenPrintCall);
        thenBlock->addStatement(thenPrintStmt);
        
        // 创建if语句
        IdentifierExpression* conditionExpr = new IdentifierExpression(conditionId);
        IfStatement* ifStmt = new IfStatement(conditionExpr, thenBlock);
        program->addStatement(ifStmt);
        
        interpreter.execute(program);
        
        delete program;
    }
    
    // 测试7：作用域测试
    cout << "\n7. 测试作用域:" << endl;
    {
        Program* program = new Program();
        
        // 全局变量
        StringLiteral* globalValue = new StringLiteral("全局变量");
        IdentifierNode* globalId = new IdentifierNode(createIdentifierToken("global"));
        VariableDeclaration* globalDecl = new VariableDeclaration(globalId, globalValue);
        program->addStatement(globalDecl);
        
        // 创建块语句
        BlockStatement* block = new BlockStatement();
        
        // 局部变量
        StringLiteral* localValue = new StringLiteral("局部变量");
        IdentifierNode* localId = new IdentifierNode(createIdentifierToken("local"));
        VariableDeclaration* localDecl = new VariableDeclaration(localId, localValue);
        block->addStatement(localDecl);
        
        // 在块内打印局部变量
        IdentifierNode* print1Id = new IdentifierNode(createIdentifierToken("print"));
        IdentifierExpression* print1Expr = new IdentifierExpression(print1Id);
        CallExpression* print1Call = new CallExpression(print1Expr);
        print1Call->addArgument(new IdentifierExpression(localId));
        ExpressionStatement* print1Stmt = new ExpressionStatement(print1Call);
        block->addStatement(print1Stmt);
        
        // 在块内打印全局变量
        IdentifierNode* print2Id = new IdentifierNode(createIdentifierToken("print"));
        IdentifierExpression* print2Expr = new IdentifierExpression(print2Id);
        CallExpression* print2Call = new CallExpression(print2Expr);
        print2Call->addArgument(new IdentifierExpression(globalId));
        ExpressionStatement* print2Stmt = new ExpressionStatement(print2Call);
        block->addStatement(print2Stmt);
        
        program->addStatement(block);
        
        interpreter.execute(program);
        
        delete program;
    }
    
    // 测试8：内置函数测试
    cout << "\n8. 测试内置函数:" << endl;
    {
        Program* program = new Program();
        
        // 测试length函数
        StringLiteral* testStr = new StringLiteral("Hello World");
        IdentifierNode* testId = new IdentifierNode(createIdentifierToken("test"));
        VariableDeclaration* testDecl = new VariableDeclaration(testId, testStr);
        program->addStatement(testDecl);
        
        // length(test)
        IdentifierNode* lengthId = new IdentifierNode(createIdentifierToken("length"));
        IdentifierExpression* lengthExpr = new IdentifierExpression(lengthId);
        CallExpression* lengthCall = new CallExpression(lengthExpr);
        lengthCall->addArgument(new IdentifierExpression(testId));
        
        IdentifierNode* lenId = new IdentifierNode(createIdentifierToken("len"));
        VariableDeclaration* lenDecl = new VariableDeclaration(lenId, lengthCall);
        program->addStatement(lenDecl);
        
        // 打印长度
        IdentifierNode* printId = new IdentifierNode(createIdentifierToken("print"));
        IdentifierExpression* printExpr = new IdentifierExpression(printId);
        CallExpression* printCall = new CallExpression(printExpr);
        printCall->addArgument(new IdentifierExpression(lenId));
        ExpressionStatement* printStmt = new ExpressionStatement(printCall);
        program->addStatement(printStmt);
        
        interpreter.execute(program);
        
        delete program;
    }
    
    cout << "\n=== 所有测试完成 ===" << endl;
    
    return 0;
}
