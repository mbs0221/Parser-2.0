#include <gtest/gtest.h>
#include "parser/expression.h"
#include "interpreter/interpreter.h"
#include "lexer/value.h"

using namespace std;

// Cast操作测试套件
class CastOperationsTest : public ::testing::Test {
protected:
    Interpreter interpreter;
    
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
    
    // 辅助函数：创建并测试cast表达式
    Value* testCastExpression(Value* operand, const string& targetType) {
        // 创建操作数的常量表达式
        ConstantExpression* operandExpr = new ConstantExpression(operand);
        
        // 创建cast表达式
        CastExpression* castExpr = new CastExpression(operandExpr, targetType);
        
        // 执行cast表达式
        Value* result = interpreter.visit(castExpr);
        
        // 清理
        delete operandExpr;
        delete castExpr;
        
        return result;
    }
};

// 测试int类型转换
TEST_F(CastOperationsTest, IntCastOperations) {
    // 测试double转int
    Double* doubleVal = new Double(3.14);
    Value* result = testCastExpression(doubleVal, "int");
    EXPECT_NE(result, nullptr);
    Integer* intResult = dynamic_cast<Integer*>(result);
    EXPECT_NE(intResult, nullptr);
    EXPECT_EQ(intResult->getValue(), 3);  // 3.14转int应该是3
    delete doubleVal;
    delete result;
    
    // 测试bool转int
    Bool* boolVal = new Bool(true);
    result = testCastExpression(boolVal, "int");
    EXPECT_NE(result, nullptr);
    intResult = dynamic_cast<Integer*>(result);
    EXPECT_NE(intResult, nullptr);
    EXPECT_EQ(intResult->getValue(), 1);  // true转int应该是1
    delete boolVal;
    delete result;
    
    // 测试char转int
    Char* charVal = new Char('A');
    result = testCastExpression(charVal, "int");
    EXPECT_NE(result, nullptr);
    intResult = dynamic_cast<Integer*>(result);
    EXPECT_NE(intResult, nullptr);
    EXPECT_EQ(intResult->getValue(), 65);  // 'A'的ASCII码是65
    delete charVal;
    delete result;
    
    // 测试string转int
    String* strVal = new String("42");
    result = testCastExpression(strVal, "int");
    EXPECT_NE(result, nullptr);
    intResult = dynamic_cast<Integer*>(result);
    EXPECT_NE(intResult, nullptr);
    EXPECT_EQ(intResult->getValue(), 42);  // "42"转int应该是42
    delete strVal;
    delete result;
}

// 测试double类型转换
TEST_F(CastOperationsTest, DoubleCastOperations) {
    // 测试int转double
    Integer* intVal = new Integer(42);
    Value* result = testCastExpression(intVal, "double");
    EXPECT_NE(result, nullptr);
    Double* doubleResult = dynamic_cast<Double*>(result);
    EXPECT_NE(doubleResult, nullptr);
    EXPECT_DOUBLE_EQ(doubleResult->getValue(), 42.0);  // 42转double应该是42.0
    delete intVal;
    delete result;
    
    // 测试bool转double
    Bool* boolVal = new Bool(true);
    result = testCastExpression(boolVal, "double");
    EXPECT_NE(result, nullptr);
    doubleResult = dynamic_cast<Double*>(result);
    EXPECT_NE(doubleResult, nullptr);
    EXPECT_DOUBLE_EQ(doubleResult->getValue(), 1.0);  // true转double应该是1.0
    delete boolVal;
    delete result;
    
    // 测试char转double
    Char* charVal = new Char('A');
    result = testCastExpression(charVal, "double");
    EXPECT_NE(result, nullptr);
    doubleResult = dynamic_cast<Double*>(result);
    EXPECT_NE(doubleResult, nullptr);
    EXPECT_DOUBLE_EQ(doubleResult->getValue(), 65.0);  // 'A'转double应该是65.0
    delete charVal;
    delete result;
    
    // 测试string转double
    String* strVal = new String("3.14");
    result = testCastExpression(strVal, "double");
    EXPECT_NE(result, nullptr);
    doubleResult = dynamic_cast<Double*>(result);
    EXPECT_NE(doubleResult, nullptr);
    EXPECT_DOUBLE_EQ(doubleResult->getValue(), 3.14);  // "3.14"转double应该是3.14
    delete strVal;
    delete result;
}

// 测试bool类型转换
TEST_F(CastOperationsTest, BoolCastOperations) {
    // 测试int转bool
    Integer* intVal = new Integer(42);
    Value* result = testCastExpression(intVal, "bool");
    EXPECT_NE(result, nullptr);
    Bool* boolResult = dynamic_cast<Bool*>(result);
    EXPECT_NE(boolResult, nullptr);
    EXPECT_TRUE(boolResult->getValue());  // 非零整数转bool应该是true
    delete intVal;
    delete result;
    
    // 测试double转bool
    Double* doubleVal = new Double(3.14);
    result = testCastExpression(doubleVal, "bool");
    EXPECT_NE(result, nullptr);
    boolResult = dynamic_cast<Bool*>(result);
    EXPECT_NE(boolResult, nullptr);
    EXPECT_TRUE(boolResult->getValue());  // 非零浮点数转bool应该是true
    delete doubleVal;
    delete result;
    
    // 测试char转bool
    Char* charVal = new Char('A');
    result = testCastExpression(charVal, "bool");
    EXPECT_NE(result, nullptr);
    boolResult = dynamic_cast<Bool*>(result);
    EXPECT_NE(boolResult, nullptr);
    EXPECT_TRUE(boolResult->getValue());  // 非零字符转bool应该是true
    delete charVal;
    delete result;
    
    // 测试string转bool
    String* strVal = new String("hello");
    result = testCastExpression(strVal, "bool");
    EXPECT_NE(result, nullptr);
    boolResult = dynamic_cast<Bool*>(result);
    EXPECT_NE(boolResult, nullptr);
    EXPECT_TRUE(boolResult->getValue());  // 非空字符串转bool应该是true
    delete strVal;
    delete result;
}

// 测试char类型转换
TEST_F(CastOperationsTest, CharCastOperations) {
    // 测试int转char
    Integer* intVal = new Integer(65);
    Value* result = testCastExpression(intVal, "char");
    EXPECT_NE(result, nullptr);
    Char* charResult = dynamic_cast<Char*>(result);
    EXPECT_NE(charResult, nullptr);
    EXPECT_EQ(charResult->getValue(), 'A');  // 65转char应该是'A'
    delete intVal;
    delete result;
    
    // 测试double转char
    Double* doubleVal = new Double(65.5);
    result = testCastExpression(doubleVal, "char");
    EXPECT_NE(result, nullptr);
    charResult = dynamic_cast<Char*>(result);
    EXPECT_NE(charResult, nullptr);
    EXPECT_EQ(charResult->getValue(), 'A');  // 65.5转char应该是'A'
    delete doubleVal;
    delete result;
    
    // 测试bool转char
    Bool* boolVal = new Bool(true);
    result = testCastExpression(boolVal, "char");
    EXPECT_NE(result, nullptr);
    charResult = dynamic_cast<Char*>(result);
    EXPECT_NE(charResult, nullptr);
    EXPECT_EQ(charResult->getValue(), '1');  // true转char应该是'1'
    delete boolVal;
    delete result;
    
    // 测试string转char
    String* strVal = new String("A");
    result = testCastExpression(strVal, "char");
    EXPECT_NE(result, nullptr);
    charResult = dynamic_cast<Char*>(result);
    EXPECT_NE(charResult, nullptr);
    EXPECT_EQ(charResult->getValue(), 'A');  // "A"转char应该是'A'
    delete strVal;
    delete result;
}

// 测试string类型转换
TEST_F(CastOperationsTest, StringCastOperations) {
    // 测试int转string
    Integer* intVal = new Integer(42);
    Value* result = testCastExpression(intVal, "string");
    EXPECT_NE(result, nullptr);
    String* strResult = dynamic_cast<String*>(result);
    EXPECT_NE(strResult, nullptr);
    EXPECT_EQ(strResult->getValue(), "42");  // 42转string应该是"42"
    delete intVal;
    delete result;
    
    // 测试double转string
    Double* doubleVal = new Double(3.14);
    result = testCastExpression(doubleVal, "string");
    EXPECT_NE(result, nullptr);
    strResult = dynamic_cast<String*>(result);
    EXPECT_NE(strResult, nullptr);
    EXPECT_EQ(strResult->getValue(), "3.14");  // 3.14转string应该是"3.14"
    delete doubleVal;
    delete result;
    
    // 测试bool转string
    Bool* boolVal = new Bool(true);
    result = testCastExpression(boolVal, "string");
    EXPECT_NE(result, nullptr);
    strResult = dynamic_cast<String*>(result);
    EXPECT_NE(strResult, nullptr);
    EXPECT_EQ(strResult->getValue(), "true");  // true转string应该是"true"
    delete boolVal;
    delete result;
    
    // 测试char转string
    Char* charVal = new Char('A');
    result = testCastExpression(charVal, "string");
    EXPECT_NE(result, nullptr);
    strResult = dynamic_cast<String*>(result);
    EXPECT_NE(strResult, nullptr);
    EXPECT_EQ(strResult->getValue(), "A");  // 'A'转string应该是"A"
    delete charVal;
    delete result;
}

// 测试边界情况
TEST_F(CastOperationsTest, EdgeCases) {
    // 测试零值转换
    Integer* zeroInt = new Integer(0);
    Value* result = testCastExpression(zeroInt, "bool");
    EXPECT_NE(result, nullptr);
    Bool* boolResult = dynamic_cast<Bool*>(result);
    EXPECT_NE(boolResult, nullptr);
    EXPECT_FALSE(boolResult->getValue());  // 0转bool应该是false
    delete zeroInt;
    delete result;
    
    // 测试空字符串转换
    String* emptyStr = new String("");
    result = testCastExpression(emptyStr, "bool");
    EXPECT_NE(result, nullptr);
    boolResult = dynamic_cast<Bool*>(result);
    EXPECT_NE(boolResult, nullptr);
    EXPECT_FALSE(boolResult->getValue());  // 空字符串转bool应该是false
    delete emptyStr;
    delete result;
    
    // 测试无效字符串转数字
    String* invalidStr = new String("abc");
    result = testCastExpression(invalidStr, "int");
    EXPECT_NE(result, nullptr);
    Integer* intResult = dynamic_cast<Integer*>(result);
    EXPECT_NE(intResult, nullptr);
    EXPECT_EQ(intResult->getValue(), 0);  // 无效字符串转int应该是0
    delete invalidStr;
    delete result;
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
