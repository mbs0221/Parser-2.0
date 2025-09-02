#include <gtest/gtest.h>
#include "lexer/lexer.h"
#include "lexer/value.h"

using namespace std;

class CharFlyweightTest : public ::testing::Test {
protected:
    void SetUp() override {
        factory = TokenFlyweight::getInstance();
    }

    TokenFlyweight* factory;
};

// 测试相同的字符值是否返回相同的对象指针
TEST_F(CharFlyweightTest, SameCharValueReturnsSamePointer) {
    auto char1 = factory->getChar('a');
    auto char2 = factory->getChar('a');

    // 验证返回的是同一个对象
    EXPECT_EQ(char1.get(), char2.get());
    EXPECT_EQ(char1->getValue(), 'a');
    EXPECT_EQ(char2->getValue(), 'a');
}

// 测试不同的字符值返回不同的对象指针
TEST_F(CharFlyweightTest, DifferentCharValuesReturnDifferentPointers) {
    auto char1 = factory->getChar('a');
    auto char2 = factory->getChar('b');

    // 验证返回的是不同的对象
    EXPECT_NE(char1.get(), char2.get());
    EXPECT_EQ(char1->getValue(), 'a');
    EXPECT_EQ(char2->getValue(), 'b');
}

// 测试缓存大小
TEST_F(CharFlyweightTest, CacheSizeIncreases) {
    size_t initialSize = factory->getCharCacheSize();
    
    factory->getChar('x');
    factory->getChar('y');
    factory->getChar('z');
    
    EXPECT_EQ(factory->getCharCacheSize(), initialSize + 3);
}

// 测试重复获取相同字符不会增加缓存大小
TEST_F(CharFlyweightTest, RepeatedCharsDoNotIncreaseCache) {
    // 先获取一个字符，确保它在缓存中
    factory->getChar('a');
    size_t initialSize = factory->getCharCacheSize();
    
    // 重复获取相同的字符
    factory->getChar('a');
    factory->getChar('a');
    factory->getChar('a');
    
    // 缓存大小应该不变
    EXPECT_EQ(factory->getCharCacheSize(), initialSize);
}
