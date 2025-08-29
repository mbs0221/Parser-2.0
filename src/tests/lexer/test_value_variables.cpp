#include <gtest/gtest.h>
#include "lexer/value.h"
#include <iostream>

using namespace std;

class ValueVariablesTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化设置
    }
    
    void TearDown() override {
        // 清理资源
    }
};

// 测试变量值的基本功能
TEST_F(ValueVariablesTest, VariableValueCreation) {
    Integer* intVar = new Integer(100);
    Double* doubleVar = new Double(2.718);
    String* stringVar = new String("test string");
    
    ASSERT_NE(intVar, nullptr);
    ASSERT_NE(doubleVar, nullptr);
    ASSERT_NE(stringVar, nullptr);
    
    EXPECT_EQ(intVar->getValue(), 100);
    EXPECT_DOUBLE_EQ(doubleVar->getValue(), 2.718);
    EXPECT_EQ(stringVar->getValue(), "test string");
    
    delete intVar;
    delete doubleVar;
    delete stringVar;
}

// 测试变量值的类型信息
TEST_F(ValueVariablesTest, VariableTypeInfo) {
    Integer* intVar = new Integer(42);
    Double* doubleVar = new Double(3.14);
    Bool* boolVar = new Bool(true);
    Char* charVar = new Char('X');
    
    EXPECT_EQ(intVar->getTypeName(), "int");
    EXPECT_EQ(doubleVar->getTypeName(), "double");
    EXPECT_EQ(boolVar->getTypeName(), "bool");
    EXPECT_EQ(charVar->getTypeName(), "char");
    
    delete intVar;
    delete doubleVar;
    delete boolVar;
    delete charVar;
}

// 测试变量值的字符串表示
TEST_F(ValueVariablesTest, VariableStringRepresentation) {
    Integer* intVar = new Integer(123);
    Double* doubleVar = new Double(1.23);
    String* stringVar = new String("hello");
    
    EXPECT_EQ(intVar->toString(), "123");
    EXPECT_EQ(doubleVar->toString(), "1.23");
    EXPECT_EQ(stringVar->toString(), "\"hello\"");
    
    delete intVar;
    delete doubleVar;
    delete stringVar;
}

int test_value_variables_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
