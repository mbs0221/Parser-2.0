#ifndef SIMPLE_TEST_FRAMEWORK_H
#define SIMPLE_TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <vector>
#include <cassert>

// 简单的测试框架
class SimpleTestFramework {
private:
    static int totalTests;
    static int passedTests;
    static int failedTests;
    static std::vector<std::string> failedTestNames;

public:
    static void reset() {
        totalTests = 0;
        passedTests = 0;
        failedTests = 0;
        failedTestNames.clear();
    }
    
    static void printSummary() {
        std::cout << "\n=== 测试总结 ===" << std::endl;
        std::cout << "总测试数: " << totalTests << std::endl;
        std::cout << "通过: " << passedTests << std::endl;
        std::cout << "失败: " << failedTests << std::endl;
        
        if (!failedTestNames.empty()) {
            std::cout << "失败的测试:" << std::endl;
            for (const auto& name : failedTestNames) {
                std::cout << "  - " << name << std::endl;
            }
        }
    }
    
    static void test(const std::string& testName, bool condition) {
        totalTests++;
        if (condition) {
            passedTests++;
            std::cout << "✓ " << testName << " 通过" << std::endl;
        } else {
            failedTests++;
            failedTestNames.push_back(testName);
            std::cout << "✗ " << testName << " 失败" << std::endl;
        }
    }
    
    template<typename T>
    static void assertEquals(const std::string& testName, const T& expected, const T& actual) {
        totalTests++;
        if (expected == actual) {
            passedTests++;
            std::cout << "✓ " << testName << " 通过 (期望: " << expected << ", 实际: " << actual << ")" << std::endl;
        } else {
            failedTests++;
            failedTestNames.push_back(testName);
            std::cout << "✗ " << testName << " 失败 (期望: " << expected << ", 实际: " << actual << ")" << std::endl;
        }
    }
    
    template<typename T>
    static void assertNotEquals(const std::string& testName, const T& expected, const T& actual) {
        totalTests++;
        if (expected != actual) {
            passedTests++;
            std::cout << "✓ " << testName << " 通过 (期望不等于: " << expected << ", 实际: " << actual << ")" << std::endl;
        } else {
            failedTests++;
            failedTestNames.push_back(testName);
            std::cout << "✗ " << testName << " 失败 (期望不等于: " << expected << ", 实际: " << actual << ")" << std::endl;
        }
    }
    
    static void assertTrue(const std::string& testName, bool condition) {
        test(testName, condition);
    }
    
    static void assertFalse(const std::string& testName, bool condition) {
        test(testName, !condition);
    }
    
    template<typename T>
    static void assertNotNull(const std::string& testName, const T* ptr) {
        test(testName, ptr != nullptr);
    }
    
    template<typename T>
    static void assertNull(const std::string& testName, const T* ptr) {
        test(testName, ptr == nullptr);
    }
};

// 静态成员初始化
int SimpleTestFramework::totalTests = 0;
int SimpleTestFramework::passedTests = 0;
int SimpleTestFramework::failedTests = 0;
std::vector<std::string> SimpleTestFramework::failedTestNames;

// 测试夹具基类
class TestFixture {
protected:
    virtual void setUp() {}
    virtual void tearDown() {}
};

// 测试宏
#define TEST_CASE(name) \
    class TestCase_##name : public TestFixture { \
    public: \
        void run(); \
    }; \
    void TestCase_##name::run()

#define RUN_TEST(name) \
    do { \
        TestCase_##name test; \
        test.setUp(); \
        test.run(); \
        test.tearDown(); \
    } while(0)

#define EXPECT_EQ(expected, actual) \
    SimpleTestFramework::assertEquals(__FUNCTION__, expected, actual)

#define EXPECT_NE(expected, actual) \
    SimpleTestFramework::assertNotEquals(__FUNCTION__, expected, actual)

#define EXPECT_TRUE(condition) \
    SimpleTestFramework::assertTrue(__FUNCTION__, condition)

#define EXPECT_FALSE(condition) \
    SimpleTestFramework::assertFalse(__FUNCTION__, condition)

#define EXPECT_NOT_NULL(ptr) \
    SimpleTestFramework::assertNotNull(__FUNCTION__, ptr)

#define EXPECT_NULL(ptr) \
    SimpleTestFramework::assertNull(__FUNCTION__, ptr)

#endif // SIMPLE_TEST_FRAMEWORK_H
