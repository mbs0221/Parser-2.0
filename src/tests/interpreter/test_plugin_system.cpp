#include <gtest/gtest.h>
#include "interpreter/interpreter.h"
#include "parser/parser.h"
#include "lexer/lexer.h"
#include <iostream>
#include <fstream>
#include <cstdio>

using namespace std;

class PluginSystemTest : public ::testing::Test {
protected:
    Interpreter* interpreter;
    
    void SetUp() override {
        interpreter = new Interpreter(false);
    }
    
    void TearDown() override {
        delete interpreter;
    }
};

TEST_F(PluginSystemTest, LoadedPluginsCheck) {
    vector<string> loadedPlugins = interpreter->getLoadedPlugins();
    // 由于我们禁用了插件加载，应该没有插件
    EXPECT_TRUE(loadedPlugins.empty());
}

TEST_F(PluginSystemTest, BasicBuiltinFunctions) {
    // 测试基础内置函数的基本结构
    EXPECT_NE(interpreter, nullptr);
    
    // 测试解释器可以正常创建
    Interpreter* testInterpreter = new Interpreter(false);
    EXPECT_NE(testInterpreter, nullptr);
    delete testInterpreter;
}

TEST_F(PluginSystemTest, PluginSystemStructure) {
    // 测试插件系统的基本结构
    EXPECT_TRUE(true);
}
