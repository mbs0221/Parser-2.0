#include <gtest/gtest.h>
#include "lexer/value.h"

using namespace std;

class StrMethodTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 测试前的设置
    }
    
    void TearDown() override {
        // 测试后的清理
    }
};

TEST_F(StrMethodTest, TokenStr) {
    Token token(123);
    EXPECT_EQ(token.str(), "token");
}

TEST_F(StrMethodTest, WordStr) {
    Word word(ID, "variable");
    EXPECT_EQ(word.str(), "variable");
}

TEST_F(StrMethodTest, TypeStr) {
    Type type(ID, "int", 4);
    EXPECT_EQ(type.str(), "int");
}

TEST_F(StrMethodTest, OperatorStr) {
    Operator op(PLUS, "+", 1, true);
    EXPECT_EQ(op.str(), "+");
}

TEST_F(StrMethodTest, VisibilityStr) {
    Visibility vis(PUBLIC, VIS_PUBLIC);
    EXPECT_EQ(vis.str(), "public");
}

TEST_F(StrMethodTest, BoolStr) {
    Bool boolVal(true);
    EXPECT_EQ(boolVal.str(), "true");
}

TEST_F(StrMethodTest, IntegerStr) {
    Integer intVal(42);
    EXPECT_EQ(intVal.str(), "42");
}

TEST_F(StrMethodTest, CharStr) {
    Char charVal('A');
    EXPECT_EQ(charVal.str(), "A");
}

TEST_F(StrMethodTest, DoubleStr) {
    Double doubleVal(3.14);
    EXPECT_EQ(doubleVal.str(), "3.14");
}

TEST_F(StrMethodTest, StringStr) {
    String stringVal("Hello World");
    EXPECT_EQ(stringVal.str(), "Hello World");
}

TEST_F(StrMethodTest, ArrayStr) {
    Array array;
    array.addElement(new Integer(1));
    array.addElement(new Integer(2));
    array.addElement(new Integer(3));
    EXPECT_EQ(array.str(), "[1, 2, 3]");
}

TEST_F(StrMethodTest, DictStr) {
    Dict dict;
    dict.setEntry("name", new String("John"));
    dict.setEntry("age", new Integer(25));
    EXPECT_EQ(dict.str(), "{\"age\": 25, \"name\": \"John\"}");
} 
