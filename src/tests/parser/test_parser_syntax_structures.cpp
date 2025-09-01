#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include <string>
#include <memory>
#include "parser/parser.h"
#include "parser/inter.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include "lexer/value.h"

using namespace std;

// Parser语法结构测试套件
class ParserSyntaxStructuresTest : public ::testing::Test {
protected:
    void SetUp() override {
        parser = new Parser();
    }
    
    void TearDown() override {
        if (parser) {
            delete parser;
        }
        // 清理临时文件
        if (tempFile.is_open()) {
            tempFile.close();
            remove(tempFileName.c_str());
        }
    }
    
    // 创建临时测试文件
    void createTempFile(const string& content) {
        tempFileName = "/tmp/parser_test_" + to_string(getpid()) + ".txt";
        tempFile.open(tempFileName);
        tempFile << content;
        tempFile.close();
    }
    
    // 测试解析结果
    bool testParseSuccess(const string& code) {
        createTempFile(code);
        Program* program = parser->parse(tempFileName);
        bool success = (program != nullptr);
        if (program) {
            delete program;
        }
        return success;
    }
    
    // 测试特定语法结构
    void testSyntaxStructure(const string& description, const string& code, bool shouldSucceed = true) {
        SCOPED_TRACE("Testing: " + description);
        bool result = testParseSuccess(code);
        if (shouldSucceed) {
            EXPECT_TRUE(result) << "Failed to parse: " << code;
        } else {
            EXPECT_FALSE(result) << "Should not parse: " << code;
        }
    }

private:
    Parser* parser;
    ofstream tempFile;
    string tempFileName;
};

// ==================== 基础表达式测试 ====================

TEST_F(ParserSyntaxStructuresTest, BasicExpressions) {
    // 整数常量
    testSyntaxStructure("Integer constant", "42");
    
    // 浮点数常量
    testSyntaxStructure("Float constant", "3.14");
    
    // 布尔常量
    testSyntaxStructure("Boolean true", "true");
    testSyntaxStructure("Boolean false", "false");
    
    // 字符串常量
    testSyntaxStructure("String constant", "\"Hello World\"");
    
    // 变量引用
    testSyntaxStructure("Variable reference", "x");
    testSyntaxStructure("Variable with underscore", "my_variable");
    testSyntaxStructure("Variable with numbers", "var123");
}

TEST_F(ParserSyntaxStructuresTest, BinaryExpressions) {
    // 算术运算
    testSyntaxStructure("Addition", "1 + 2");
    testSyntaxStructure("Subtraction", "10 - 5");
    testSyntaxStructure("Multiplication", "3 * 4");
    testSyntaxStructure("Division", "15 / 3");
    testSyntaxStructure("Modulo", "17 % 5");
    
    // 比较运算
    testSyntaxStructure("Equal", "x == y");
    testSyntaxStructure("Not equal", "a != b");
    testSyntaxStructure("Less than", "x < y");
    testSyntaxStructure("Less equal", "x <= y");
    testSyntaxStructure("Greater than", "x > y");
    testSyntaxStructure("Greater equal", "x >= y");
    
    // 逻辑运算
    testSyntaxStructure("Logical AND", "true && false");
    testSyntaxStructure("Logical OR", "true || false");
}

TEST_F(ParserSyntaxStructuresTest, UnaryExpressions) {
    // 一元运算
    testSyntaxStructure("Unary minus", "-42");
    testSyntaxStructure("Unary plus", "+42");
    testSyntaxStructure("Logical NOT", "!true");
    
    // 前缀和后缀
    testSyntaxStructure("Prefix increment", "++x");
    testSyntaxStructure("Prefix decrement", "--x");
    testSyntaxStructure("Postfix increment", "x++");
    testSyntaxStructure("Postfix decrement", "x--");
}

TEST_F(ParserSyntaxStructuresTest, ParenthesizedExpressions) {
    // 括号表达式
    testSyntaxStructure("Simple parentheses", "(42)");
    testSyntaxStructure("Nested parentheses", "((42))");
    testSyntaxStructure("Expression in parentheses", "(1 + 2) * 3");
    testSyntaxStructure("Complex nested", "((1 + 2) * (3 + 4))");
}

// ==================== 函数调用测试 ====================

TEST_F(ParserSyntaxStructuresTest, FunctionCalls) {
    // 简单函数调用
    testSyntaxStructure("Simple function call", "func()");
    testSyntaxStructure("Function with one argument", "func(42)");
    testSyntaxStructure("Function with multiple arguments", "func(1, 2, 3)");
    
    // 嵌套函数调用
    testSyntaxStructure("Nested function calls", "outer(inner(42))");
    testSyntaxStructure("Chained function calls", "func1(func2(func3()))");
    
    // 方法调用
    testSyntaxStructure("Method call", "obj.method()");
    testSyntaxStructure("Method with arguments", "obj.method(1, 2)");
    testSyntaxStructure("Chained methods", "obj.method1().method2()");
}

// ==================== 数组和字典测试 ====================

TEST_F(ParserSyntaxStructuresTest, Arrays) {
    // 数组字面量
    testSyntaxStructure("Empty array", "[]");
    testSyntaxStructure("Array with one element", "[42]");
    testSyntaxStructure("Array with multiple elements", "[1, 2, 3]");
    testSyntaxStructure("Array with expressions", "[1 + 2, 3 * 4]");
    
    // 数组访问
    testSyntaxStructure("Array access", "arr[0]");
    testSyntaxStructure("Nested array access", "arr[0][1]");
    testSyntaxStructure("Array access with expression", "arr[i + 1]");
}

TEST_F(ParserSyntaxStructuresTest, Dictionaries) {
    // 字典字面量
    testSyntaxStructure("Empty dictionary", "{}");
    testSyntaxStructure("Dictionary with one pair", "{'key': 'value'}");
    testSyntaxStructure("Dictionary with multiple pairs", "{'a': 1, 'b': 2}");
    testSyntaxStructure("Dictionary with expressions", "{'sum': 1 + 2}");
    
    // 字典访问
    testSyntaxStructure("Dictionary access", "dict['key']");
    testSyntaxStructure("Dictionary access with variable", "dict[key]");
}

// ==================== 变量声明测试 ====================

TEST_F(ParserSyntaxStructuresTest, VariableDefinitions) {
    // 简单变量声明
    testSyntaxStructure("Variable declaration with type", "let x: int = 42");
    testSyntaxStructure("Variable declaration without type", "let x = 42");
    testSyntaxStructure("Variable declaration with expression", "let y = 1 + 2");
    
    // 多个变量声明
    testSyntaxStructure("Multiple variables", "let x = 1, y = 2, z = 3");
    
    // 常量声明
    testSyntaxStructure("Constant declaration", "let PI: float = 3.14159");
}

// ==================== 控制流语句测试 ====================

TEST_F(ParserSyntaxStructuresTest, IfStatements) {
    // 简单if语句
    testSyntaxStructure("Simple if", "if (x > 0) { x = 1; }");
    testSyntaxStructure("If with else", "if (x > 0) { x = 1; } else { x = -1; }");
    testSyntaxStructure("If-else if-else", "if (x > 0) { x = 1; } else if (x < 0) { x = -1; } else { x = 0; }");
    
    // 嵌套if语句
    testSyntaxStructure("Nested if", "if (x > 0) { if (y > 0) { z = 1; } }");
}

TEST_F(ParserSyntaxStructuresTest, LoopStatements) {
    // while循环
    testSyntaxStructure("While loop", "while (x > 0) { x = x - 1; }");
    testSyntaxStructure("While with break", "while (true) { if (x == 0) break; x = x - 1; }");
    
    // for循环
    testSyntaxStructure("For loop", "for (let i = 0; i < 10; i = i + 1) { sum = sum + i; }");
    testSyntaxStructure("For with continue", "for (let i = 0; i < 10; i = i + 1) { if (i % 2 == 0) continue; sum = sum + i; }");
}

TEST_F(ParserSyntaxStructuresTest, SwitchStatements) {
    // switch语句
    testSyntaxStructure("Switch statement", "switch (x) { case 1: y = 1; break; case 2: y = 2; break; default: y = 0; }");
    testSyntaxStructure("Switch with fallthrough", "switch (x) { case 1: y = 1; case 2: y = 2; break; }");
}

// ==================== 函数定义测试 ====================

TEST_F(ParserSyntaxStructuresTest, FunctionDefinitions) {
    // 简单函数
    testSyntaxStructure("Simple function", "function add(a: int, b: int): int { return a + b; }");
    testSyntaxStructure("Function without return type", "function greet(name: string) { print(name); }");
    testSyntaxStructure("Function without parameters", "function getAnswer(): int { return 42; }");
    
    // 函数重载
    testSyntaxStructure("Function overload", "function add(a: int, b: int): int { return a + b; } function add(a: float, b: float): float { return a + b; }");
}

// ==================== 结构体和类测试 ====================

TEST_F(ParserSyntaxStructuresTest, Structs) {
    // 结构体定义
    testSyntaxStructure("Simple struct", "struct Point { x: int; y: int; }");
    testSyntaxStructure("Struct with methods", "struct Counter { count: int; function increment() { count = count + 1; } }");
    
    // 结构体实例化
    testSyntaxStructure("Struct instantiation", "let p = Point{x: 1, y: 2}");
}

TEST_F(ParserSyntaxStructuresTest, Classes) {
    // 类定义
    testSyntaxStructure("Simple class", "class Animal { name: string; function speak() { print(name); } }");
    testSyntaxStructure("Class with inheritance", "class Dog extends Animal { function bark() { print('Woof!'); } }");
}

// ==================== 错误语法测试 ====================

TEST_F(ParserSyntaxStructuresTest, InvalidSyntax) {
    // 这些应该解析失败
    testSyntaxStructure("Missing semicolon", "let x = 42", false);
    testSyntaxStructure("Unmatched parentheses", "(1 + 2", false);
    testSyntaxStructure("Invalid operator", "1 ** 2", false);
    testSyntaxStructure("Missing expression", "let x = ;", false);
}

// ==================== 复杂组合测试 ====================

TEST_F(ParserSyntaxStructuresTest, ComplexCombinations) {
    // 复杂的组合语法
    testSyntaxStructure("Complex expression", "let result = (func1(x) + func2(y)) * (arr[i] + dict['key'])");
    
    testSyntaxStructure("Complex control flow", 
        "if (x > 0 && y < 10) { "
        "    for (let i = 0; i < x; i = i + 1) { "
        "        if (i % 2 == 0) { "
        "            result = result + i; "
        "        } else { "
        "            continue; "
        "        } "
        "    } "
        "} else { "
        "        result = -1; "
        "}");
    
    testSyntaxStructure("Complex function", 
        "function processData(data: array, filter: function): array { "
        "    let result = []; "
        "    for (let i = 0; i < data.length(); i = i + 1) { "
        "        if (filter(data[i])) { "
        "            result.push(data[i]); "
        "        } "
        "    } "
        "    return result; "
        "}");
}

// ==================== 边界情况测试 ====================

TEST_F(ParserSyntaxStructuresTest, EdgeCases) {
    // 空程序
    testSyntaxStructure("Empty program", "");
    
    // 只有注释
    testSyntaxStructure("Only comments", "// This is a comment\n/* Another comment */");
    
    // 非常长的标识符
    testSyntaxStructure("Long identifier", "very_long_identifier_name_with_many_underscores_and_numbers_123");
    
    // 嵌套深度
    testSyntaxStructure("Deep nesting", "((((((((((42))))))))))");
}

// ==================== 模板表达式测试 ====================

TEST_F(ParserSyntaxStructuresTest, TemplateExpressions) {
    // 测试模板化的常量表达式
    ConstantExpression<int>* intExpr = new ConstantExpression<int>(42);
    EXPECT_NE(intExpr, nullptr);
    EXPECT_EQ(intExpr->getValue(), 42);
    delete intExpr;
    
    ConstantExpression<double>* doubleExpr = new ConstantExpression<double>(3.14);
    EXPECT_NE(doubleExpr, nullptr);
    EXPECT_EQ(doubleExpr->getValue(), 3.14);
    delete doubleExpr;
    
    ConstantExpression<bool>* boolExpr = new ConstantExpression<bool>(true);
    EXPECT_NE(boolExpr, nullptr);
    EXPECT_EQ(boolExpr->getValue(), true);
    delete boolExpr;
    
    ConstantExpression<string>* strExpr = new ConstantExpression<string>("Hello");
    EXPECT_NE(strExpr, nullptr);
    EXPECT_EQ(strExpr->getValue(), "Hello");
    delete strExpr;
}

// ==================== 变量表达式测试 ====================

TEST_F(ParserSyntaxStructuresTest, VariableExpressions) {
    VariableExpression* varExpr = new VariableExpression("x");
    EXPECT_NE(varExpr, nullptr);
    EXPECT_EQ(varExpr->name, "x");
    EXPECT_EQ(varExpr->getLocation(), "variable: x");
    delete varExpr;
    
    VariableExpression* underscoreVar = new VariableExpression("my_variable");
    EXPECT_NE(underscoreVar, nullptr);
    EXPECT_EQ(underscoreVar->name, "my_variable");
    delete underscoreVar;
}

// 主函数
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 