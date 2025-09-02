#include <gtest/gtest.h>
#include "lexer/value.h"
#include <iostream>

using namespace std;

class SimpleInterpreterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化设置
    }
    
    void TearDown() override {
        // 清理资源
    }
};

// 测试Value类型的基本功能
TEST_F(SimpleInterpreterTest, ValueBasicFunctionality) {
    Integer* intVal = new Integer(42);
    Double* doubleVal = new Double(3.14);
    Bool* boolVal = new Bool(true);
    Char* charVal = new Char('A');
    
    ASSERT_NE(intVal, nullptr);
    ASSERT_NE(doubleVal, nullptr);
    ASSERT_NE(boolVal, nullptr);
    ASSERT_NE(charVal, nullptr);
    
    EXPECT_EQ(intVal->getValue(), 42);
    EXPECT_DOUBLE_EQ(doubleVal->getValue(), 3.14);
    EXPECT_TRUE(boolVal->getValue());
    EXPECT_EQ(charVal->getValue(), 'A');
    
    delete intVal;
    delete doubleVal;
    delete boolVal;
    delete charVal;
}

// 测试Value类型的运算
TEST_F(SimpleInterpreterTest, ValueOperations) {
    Integer* intVal1 = new Integer(10);
    Integer* intVal2 = new Integer(5);
    
    Integer result = *intVal1 + *intVal2;
    EXPECT_EQ(result.getValue(), 15);
    
    Bool comparison = *intVal1 > *intVal2;
    EXPECT_TRUE(comparison.getValue());
    
    delete intVal1;
    delete intVal2;
}

// 测试Value类型的类型转换
TEST_F(SimpleInterpreterTest, ValueTypeConversion) {
    Integer* intVal = new Integer(42);
    Double* doubleVal = new Double(3.14);
    Bool* boolVal = new Bool(true);
    
    EXPECT_EQ(intVal->getTypeName(), "int");
    EXPECT_EQ(doubleVal->getTypeName(), "double");
    EXPECT_EQ(boolVal->getTypeName(), "bool");
    
    EXPECT_EQ(intVal->toString(), "42");
    EXPECT_EQ(doubleVal->toString(), "3.14");
    EXPECT_EQ(boolVal->toString(), "true");
    
    delete intVal;
    delete doubleVal;
    delete boolVal;
}

// 测试Value类型的边界情况
TEST_F(SimpleInterpreterTest, ValueEdgeCases) {
    Integer* zeroInt = new Integer(0);
    Double* zeroDouble = new Double(0.0);
    Bool* falseBool = new Bool(false);
    
    EXPECT_FALSE(zeroInt->toBool());
    EXPECT_FALSE(zeroDouble->toBool());
    EXPECT_FALSE(falseBool->toBool());
    
    delete zeroInt;
    delete zeroDouble;
    delete falseBool;
}

int test_simple_interpreter_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
