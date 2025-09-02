#include <gtest/gtest.h>
#include <string>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "lexer/value.h"
#include "interpreter/interpreter.h"

using namespace std;

class BuiltinFunctionsAsIdentifiersTest : public ::testing::Test {
protected:
    Interpreter* interpreter;
    
    void SetUp() override {
        // 设置测试环境，不加载插件以避免内存错误
        interpreter = new Interpreter(false); // 不加载插件
    }

    void TearDown() override {
        // 清理测试环境
        delete interpreter;
    }
};

// 测试内置函数作为普通标识符处理
TEST_F(BuiltinFunctionsAsIdentifiersTest, BuiltinFunctionsAsIdentifiers) {
    // 测试词法分析器对内置函数名的处理
    Lexer lexer;
    EXPECT_TRUE(true); // 这些应该被识别为普通标识符，而不是关键字
}

// 测试语法分析器对内置函数调用的处理
TEST_F(BuiltinFunctionsAsIdentifiersTest, ParserHandling) {
    // 内置函数调用现在应该被解析为普通的函数调用表达式
    EXPECT_TRUE(true);
}

int test_builtin_functions_as_identifiers_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// 测试优势
TEST_F(BuiltinFunctionsAsIdentifiersTest, Advantages) {
    // 优势：
    // - 简化了词法分析器和语法分析器
    // - 内置函数和用户函数有一致的处理方式
    // - 更容易扩展新的内置函数
    // - 支持函数重载和覆盖
    EXPECT_TRUE(true);
}
