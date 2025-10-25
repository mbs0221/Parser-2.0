#include <gtest/gtest.h>
#include <string>
#include <fstream>
#include <iostream>
#include "interpreter/core/interpreter.h"
#include "parser/parser.h"
#include "common/logger.h"

using namespace std;

class ModuleSystemTest : public ::testing::Test {
protected:
    Interpreter* interpreter;
    string testDir;
    
    void SetUp() override {
        // 设置测试环境
        interpreter = new Interpreter();
        testDir = "test_modules";
        
        // 创建测试目录
        system(("mkdir -p " + testDir).c_str());
        
        // 创建测试模块文件
        createTestModules();
    }

    void TearDown() override {
        // 清理测试环境
        delete interpreter;
        system(("rm -rf " + testDir).c_str());
    }
    
    void createTestModules() {
        // 创建基础模块
        ofstream file1(testDir + "/math_utils.txt");
        file1 << "// 数学工具模块\n";
        file1 << "function add(a, b) {\n";
        file1 << "    return a + b;\n";
        file1 << "}\n";
        file1 << "function multiply(a, b) {\n";
        file1 << "    return a * b;\n";
        file1 << "}\n";
        file1 << "let PI = 3.14159;\n";
        file1.close();
        
        // 创建字符串工具模块
        ofstream file2(testDir + "/string_utils.txt");
        file2 << "// 字符串工具模块\n";
        file2 << "function toUpper(str) {\n";
        file2 << "    return str;\n"; // 简化实现
        file2 << "}\n";
        file2 << "function toLower(str) {\n";
        file2 << "    return str;\n"; // 简化实现
        file2 << "}\n";
        file2.close();
        
        // 创建依赖模块
        ofstream file3(testDir + "/advanced_math.txt");
        file3 << "// 高级数学模块\n";
        file3 << "import \"" << testDir << "/math_utils.txt\";\n";
        file3 << "function power(base, exp) {\n";
        file3 << "    let result = 1;\n";
        file3 << "    let i = 0;\n";
        file3 << "    while (i < exp) {\n";
        file3 << "        result = result * base;\n";
        file3 << "        i = i + 1;\n";
        file3 << "    }\n";
        file3 << "    return result;\n";
        file3 << "}\n";
        file3.close();
    }
};

// 测试默认导入
TEST_F(ModuleSystemTest, DefaultImport) {
    // 创建测试文件
    ofstream testFile("test_default_import.txt");
    testFile << "import \"" << testDir << "/math_utils.txt\";\n";
    testFile << "let result = add(5, 3);\n";
    testFile << "print(result);\n";
    testFile.close();
    
    // 解析和执行
    Program* program = interpreter->parser->parse("test_default_import.txt");
    ASSERT_NE(program, nullptr);
    
    // 执行程序
    interpreter->visit(program);
    
    // 清理
    delete program;
    system("rm -f test_default_import.txt");
}

// 测试命名空间导入
TEST_F(ModuleSystemTest, NamespaceImport) {
    // 创建测试文件
    ofstream testFile("test_namespace_import.txt");
    testFile << "import \"" << testDir << "/math_utils.txt\" as math;\n";
    testFile << "let result = math.add(5, 3);\n";
    testFile << "print(result);\n";
    testFile.close();
    
    // 解析和执行
    Program* program = interpreter->parser->parse("test_namespace_import.txt");
    ASSERT_NE(program, nullptr);
    
    // 执行程序
    interpreter->visit(program);
    
    // 清理
    delete program;
    system("rm -f test_namespace_import.txt");
}

// 测试选择性导入
TEST_F(ModuleSystemTest, SelectiveImport) {
    // 创建测试文件
    ofstream testFile("test_selective_import.txt");
    testFile << "import { add, multiply } from \"" << testDir << "/math_utils.txt\";\n";
    testFile << "let sum = add(5, 3);\n";
    testFile << "let product = multiply(4, 6);\n";
    testFile << "print(sum);\n";
    testFile << "print(product);\n";
    testFile.close();
    
    // 解析和执行
    Program* program = interpreter->parser->parse("test_selective_import.txt");
    ASSERT_NE(program, nullptr);
    
    // 执行程序
    interpreter->visit(program);
    
    // 清理
    delete program;
    system("rm -f test_selective_import.txt");
}

// 测试带别名的选择性导入
TEST_F(ModuleSystemTest, SelectiveImportWithAlias) {
    // 创建测试文件
    ofstream testFile("test_selective_alias_import.txt");
    testFile << "import { add as plus, multiply as times } from \"" << testDir << "/math_utils.txt\";\n";
    testFile << "let sum = plus(5, 3);\n";
    testFile << "let product = times(4, 6);\n";
    testFile << "print(sum);\n";
    testFile << "print(product);\n";
    testFile.close();
    
    // 解析和执行
    Program* program = interpreter->parser->parse("test_selective_alias_import.txt");
    ASSERT_NE(program, nullptr);
    
    // 执行程序
    interpreter->visit(program);
    
    // 清理
    delete program;
    system("rm -f test_selective_alias_import.txt");
}

// 测试通配符导入
TEST_F(ModuleSystemTest, WildcardImport) {
    // 创建测试文件
    ofstream testFile("test_wildcard_import.txt");
    testFile << "import * from \"" << testDir << "/math_utils.txt\";\n";
    testFile << "let sum = add(5, 3);\n";
    testFile << "let product = multiply(4, 6);\n";
    testFile << "print(sum);\n";
    testFile << "print(product);\n";
    testFile << "print(PI);\n";
    testFile.close();
    
    // 解析和执行
    Program* program = interpreter->parser->parse("test_wildcard_import.txt");
    ASSERT_NE(program, nullptr);
    
    // 执行程序
    interpreter->visit(program);
    
    // 清理
    delete program;
    system("rm -f test_wildcard_import.txt");
}

// 测试模块依赖
TEST_F(ModuleSystemTest, ModuleDependencies) {
    // 创建测试文件
    ofstream testFile("test_dependencies.txt");
    testFile << "import \"" << testDir << "/advanced_math.txt\";\n";
    testFile << "let result = power(2, 3);\n";
    testFile << "print(result);\n";
    testFile.close();
    
    // 解析和执行
    Program* program = interpreter->parser->parse("test_dependencies.txt");
    ASSERT_NE(program, nullptr);
    
    // 执行程序
    interpreter->visit(program);
    
    // 清理
    delete program;
    system("rm -f test_dependencies.txt");
}

// 测试模块系统功能
TEST_F(ModuleSystemTest, ModuleSystemFeatures) {
    // 测试模块系统是否已初始化
    ASSERT_NE(interpreter->getModuleSystem(), nullptr);
    
    // 测试搜索路径
    interpreter->getModuleSystem()->addSearchPath(testDir);
    
    // 测试模块加载
    auto module = interpreter->getModuleSystem()->getModuleLoader()->loadModule(testDir + "/math_utils.txt");
    ASSERT_NE(module, nullptr);
    EXPECT_EQ(module->getName(), testDir + "/math_utils.txt");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
