#include "Parser/inter.h"
#include "Parser/interpreter.h"
#include <iostream>

using namespace std;

int main() {
    cout << "=== 简化解释器测试 ===" << endl;
    
    // 创建解释器
    Interpreter interpreter;
    
    // 测试1：基本变量操作
    cout << "\n1. 基本变量操作测试:" << endl;
    {
        // 创建程序
        Program* program = new Program();
        
        // 创建变量声明：let message = "Hello World";
        StringLiteral* varValue = new StringLiteral("Hello World");
        IdentifierNode* varId = new IdentifierNode(new Word(ID, "message"));
        VariableDeclaration* varDecl = new VariableDeclaration(varId, varValue);
        program->addStatement(varDecl);
        
        // 执行程序
        interpreter.execute(program);
        
        // 验证变量是否被正确存储
        Expression* foundVar = interpreter.lookupVariable("message");
        if (foundVar) {
            cout << "变量存储成功: " << foundVar->toString() << endl;
        } else {
            cout << "变量存储失败" << endl;
        }
        
        delete program;
    }
    
    // 测试2：字符串运算
    cout << "\n2. 字符串运算测试:" << endl;
    {
        StringLiteral* str1 = new StringLiteral("Hello, ");
        StringLiteral* str2 = new StringLiteral("World!");
        
        Expression* result = interpreter.stringConcatenation(str1, str2);
        if (result) {
            cout << "字符串拼接结果: " << result->toString() << endl;
        }
        
        delete str1;
        delete str2;
        delete result;
    }
    
    // 测试3：字符串比较
    cout << "\n3. 字符串比较测试:" << endl;
    {
        StringLiteral* str1 = new StringLiteral("apple");
        StringLiteral* str2 = new StringLiteral("banana");
        
        Expression* result = interpreter.stringComparison(str1, str2, "<");
        if (result) {
            cout << "apple < banana: " << result->toString() << endl;
        }
        
        delete str1;
        delete str2;
        delete result;
    }
    
    // 测试4：数组操作
    cout << "\n4. 数组操作测试:" << endl;
    {
        ArrayNode* array = new ArrayNode();
        array->addElement(new StringLiteral("apple"));
        array->addElement(new StringLiteral("banana"));
        array->addElement(new StringLiteral("cherry"));
        
        cout << "数组内容: " << array->toString() << endl;
        cout << "数组大小: " << array->size() << endl;
        
        // 测试访问操作
        StringLiteral* index = new StringLiteral("0");
        Expression* element = array->access(index);
        if (element) {
            cout << "第一个元素: " << element->toString() << endl;
        }
        
        delete array;
        delete index;
    }
    
    // 测试5：字典操作
    cout << "\n5. 字典操作测试:" << endl;
    {
        DictNode* dict = new DictNode();
        dict->setEntry("name", new StringLiteral("John"));
        dict->setEntry("age", new StringLiteral("30"));
        dict->setEntry("city", new StringLiteral("New York"));
        
        cout << "字典内容: " << dict->toString() << endl;
        cout << "字典大小: " << dict->getEntryCount() << endl;
        
        // 测试访问操作
        StringLiteral* key = new StringLiteral("name");
        Expression* value = dict->access(key);
        if (value) {
            cout << "name的值: " << value->toString() << endl;
        }
        
        delete dict;
        delete key;
    }
    
    // 测试6：字符串节点操作
    cout << "\n6. 字符串节点测试:" << endl;
    {
        StringNode* strNode = new StringNode("Hello World");
        
        cout << "字符串值: " << strNode->getValue() << endl;
        cout << "字符串长度: " << strNode->length() << endl;
        cout << "字符串表示: " << strNode->toString() << endl;
        
        // 测试字符访问
        StringLiteral* index = new StringLiteral("0");
        Expression* charResult = strNode->access(index);
        if (charResult) {
            cout << "第一个字符: " << charResult->toString() << endl;
        }
        
        delete strNode;
        delete index;
    }
    
    // 测试7：作用域测试
    cout << "\n7. 作用域测试:" << endl;
    {
        // 在全局作用域定义变量
        interpreter.defineVariable("global", new StringLiteral("全局变量"));
        
        // 进入新作用域
        interpreter.enterScope();
        
        // 在局部作用域定义变量
        interpreter.defineVariable("local", new StringLiteral("局部变量"));
        
        // 查找变量
        Expression* globalVar = interpreter.lookupVariable("global");
        Expression* localVar = interpreter.lookupVariable("local");
        
        if (globalVar) {
            cout << "找到全局变量: " << globalVar->toString() << endl;
        }
        if (localVar) {
            cout << "找到局部变量: " << localVar->toString() << endl;
        }
        
        // 退出作用域
        interpreter.exitScope();
        
        // 再次查找变量
        Expression* globalVar2 = interpreter.lookupVariable("global");
        Expression* localVar2 = interpreter.lookupVariable("local");
        
        if (globalVar2) {
            cout << "退出作用域后，全局变量仍然存在: " << globalVar2->toString() << endl;
        }
        if (!localVar2) {
            cout << "退出作用域后，局部变量已不存在" << endl;
        }
    }
    
    // 测试8：内置函数测试
    cout << "\n8. 内置函数测试:" << endl;
    {
        vector<Expression*> args;
        args.push_back(new StringLiteral("Hello World"));
        
        Expression* lengthResult = interpreter.executeStringLength(args);
        if (lengthResult) {
            cout << "字符串长度: " << lengthResult->toString() << endl;
        }
        
        // 清理
        for (auto arg : args) {
            delete arg;
        }
        delete lengthResult;
    }
    
    cout << "\n=== 所有测试完成 ===" << endl;
    
    return 0;
}
