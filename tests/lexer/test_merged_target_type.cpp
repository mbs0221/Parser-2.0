#include <gtest/gtest.h>
#include "lexer/value.h"
#include <iostream>

using namespace std;

class MergedTargetTypeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化设置
    }
    
    void TearDown() override {
        // 清理资源
    }
};

// 测试类型合并功能
TEST_F(MergedTargetTypeTest, TypeMerging) {
    // 测试整数和浮点数的合并
    Integer* intVal = new Integer(42);
    Double* doubleVal = new Double(3.14);
    
    // 整数转换为浮点数
    Double convertedInt(static_cast<double>(intVal->getValue()));
    EXPECT_DOUBLE_EQ(convertedInt.getValue(), 42.0);
    
    // 浮点数转换为整数（截断）
    Integer convertedDouble(static_cast<int>(doubleVal->getValue()));
    EXPECT_EQ(convertedDouble.getValue(), 3);
    
    delete intVal;
    delete doubleVal;
}

// 测试类型转换
TEST_F(MergedTargetTypeTest, TypeConversion) {
    // 测试字符串到数字的转换
    String* strVal = new String("123");
    EXPECT_EQ(strVal->getValue(), "123");
    
    // 测试布尔值转换
    Bool* trueVal = new Bool(true);
    Bool* falseVal = new Bool(false);
    
    EXPECT_TRUE(trueVal->getValue());
    EXPECT_FALSE(falseVal->getValue());
    EXPECT_TRUE(trueVal->toBool());
    EXPECT_FALSE(falseVal->toBool());
    
    delete strVal;
    delete trueVal;
    delete falseVal;
}

// 测试类型比较
TEST_F(MergedTargetTypeTest, TypeComparison) {
    Integer* intVal1 = new Integer(10);
    Integer* intVal2 = new Integer(5);
    
    // 测试比较运算
    Bool greater = *intVal1 > *intVal2;
    Bool less = *intVal2 < *intVal1;
    Bool equal = *intVal1 == *intVal1;
    
    EXPECT_TRUE(greater.getValue());
    EXPECT_TRUE(less.getValue());
    EXPECT_TRUE(equal.getValue());
    
    delete intVal1;
    delete intVal2;
}

int test_merged_target_type_main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
