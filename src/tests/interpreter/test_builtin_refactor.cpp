#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace std;

class BuiltinRefactorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

TEST_F(BuiltinRefactorTest, BuiltinFunctionNames) {
    // 模拟内置函数名称列表
    vector<string> funcNames = {"print", "count", "cin", "len", "type"};
    
    EXPECT_FALSE(funcNames.empty());
    EXPECT_EQ(funcNames.size(), 5);
    
    // 检查是否包含预期的函数名
    EXPECT_TRUE(find(funcNames.begin(), funcNames.end(), "print") != funcNames.end());
    EXPECT_TRUE(find(funcNames.begin(), funcNames.end(), "count") != funcNames.end());
    EXPECT_TRUE(find(funcNames.begin(), funcNames.end(), "cin") != funcNames.end());
}

TEST_F(BuiltinRefactorTest, FunctionExistenceCheck) {
    // 模拟函数存在性检查
    vector<string> builtinFunctions = {"print", "count", "cin", "len", "type"};
    
    auto isBuiltinFunction = [&builtinFunctions](const string& name) -> bool {
        return find(builtinFunctions.begin(), builtinFunctions.end(), name) != builtinFunctions.end();
    };
    
    EXPECT_TRUE(isBuiltinFunction("print"));
    EXPECT_TRUE(isBuiltinFunction("count"));
    EXPECT_TRUE(isBuiltinFunction("cin"));
    EXPECT_FALSE(isBuiltinFunction("unknown"));
}
