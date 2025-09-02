#include <gtest/gtest.h>
#include "lexer/value.h"
#include <iostream>

using namespace std;

class SimpleOptimizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化设置
    }
    
    void TearDown() override {
        // 清理资源
    }
};

// 测试Value类型的优化
TEST_F(SimpleOptimizerTest, ValueOptimization) {
    Integer* intVal = new Integer(42);
    Double* doubleVal = new Double(3.14);
    Bool* boolVal = new Bool(true);
    
    ASSERT_NE(intVal, nullptr);
    ASSERT_NE(doubleVal, nullptr);
    ASSERT_NE(boolVal, nullptr);
    
    // 测试基本优化：确保值正确
    EXPECT_EQ(intVal->getValue(), 42);
    EXPECT_DOUBLE_EQ(doubleVal->getValue(), 3.14);
    EXPECT_TRUE(boolVal->getValue());
    
    delete intVal;
    delete doubleVal;
    delete boolVal;
}

// 测试类型转换优化
TEST_F(SimpleOptimizerTest, TypeConversionOptimization) {
    Integer* intVal = new Integer(100);
    Double* doubleVal = new Double(2.5);
    
    // 测试整数到浮点数的转换
    double intToDouble = static_cast<double>(intVal->getValue());
    EXPECT_DOUBLE_EQ(intToDouble, 100.0);
    
    // 测试浮点数到整数的转换（截断）
    int doubleToInt = static_cast<int>(doubleVal->getValue());
    EXPECT_EQ(doubleToInt, 2);
    
    delete intVal;
    delete doubleVal;
}

// 测试运算优化
TEST_F(SimpleOptimizerTest, OperationOptimization) {
    Integer* intVal1 = new Integer(10);
    Integer* intVal2 = new Integer(5);
    
    // 测试加法运算优化
    Integer result = *intVal1 + *intVal2;
    EXPECT_EQ(result.getValue(), 15);
    
    // 测试比较运算优化
    Bool comparison = *intVal1 > *intVal2;
    EXPECT_TRUE(comparison.getValue());
    
    delete intVal1;
    delete intVal2;
}

// 测试边界情况优化
TEST_F(SimpleOptimizerTest, EdgeCaseOptimization) {
    Integer* zeroInt = new Integer(0);
    Double* zeroDouble = new Double(0.0);
    Bool* falseBool = new Bool(false);
    
    // 测试零值优化
    EXPECT_FALSE(zeroInt->toBool());
    EXPECT_FALSE(zeroDouble->toBool());
    EXPECT_FALSE(falseBool->toBool());
    
    // 测试常量折叠
    Integer* constVal = new Integer(42);
    EXPECT_EQ(constVal->getValue(), 42);
    EXPECT_TRUE(constVal->toBool());
    
    delete zeroInt;
    delete zeroDouble;
    delete falseBool;
    delete constVal;
}

int test_simple_optimizer_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
