#ifndef INTERPRETER_OPERATOR_MACROS_H
#define INTERPRETER_OPERATOR_MACROS_H

// ==================== 运算符注册宏定义 ====================
// 这个文件包含了所有用于简化运算符方法注册的宏定义

// ==================== 基础运算符注册宏 ====================

// 简化二元运算符方法注册的宏
#define REGISTER_BINARY_OPERATOR(opName, opSymbol) \
    registerMethod(opName, [](Value* instance, vector<Value*>& args) -> Value* { \
        if (args.size() >= 1 && args[0]) { \
            return (*instance) opSymbol (*args[0]); \
        } \
        return nullptr; \
    })

// 简化一元运算符方法注册的宏
#define REGISTER_UNARY_OPERATOR(opName, opSymbol) \
    registerMethod(opName, [](Value* instance, vector<Value*>& args) -> Value* { \
        return opSymbol (*instance); \
    })

// 简化比较运算符方法注册的宏
#define REGISTER_COMPARISON_OPERATOR(opName, opSymbol) \
    registerMethod(opName, [](Value* instance, vector<Value*>& args) -> Value* { \
        if (args.size() >= 1 && args[0]) { \
            return (*instance) opSymbol (*args[0]); \
        } \
        return nullptr; \
    })

// 简化赋值运算符方法注册的宏
#define REGISTER_ASSIGN_OPERATOR() \
    REGISTER_BINARY_OPERATOR("assign", =)

// 简化下标访问运算符方法注册的宏（双目运算）
#define REGISTER_SUBSCRIPT_OPERATOR() \
    REGISTER_BINARY_OPERATOR("subscript", [])

// 简化成员访问运算符方法注册的宏（双目运算）
#define REGISTER_MEMBER_ACCESS_OPERATOR() \
    REGISTER_BINARY_OPERATOR("member_access", .)

// 简化new方法注册的宏
#define REGISTER_NEW_METHOD(typeName) \
    registerMethod("new", [](Value* instance, vector<Value*>& args) -> Value* { \
        return new typeName(); \
    })

// 简化字典类型new方法注册的宏（处理键值对参数）
#define REGISTER_DICT_NEW_METHOD() \
    registerMethod("new", [](Value* instance, vector<Value*>& args) -> Value* { \
        Dict* newDict = new Dict(); \
        /* 处理键值对参数 */ \
        for (size_t i = 0; i < args.size(); i += 2) { \
            if (i + 1 < args.size()) { \
                if (String* key = dynamic_cast<String*>(args[i])) { \
                    newDict->setEntry(key->getValue(), args[i + 1] ? args[i + 1]->clone() : nullptr); \
                } \
            } \
        } \
        return newDict; \
    })

// ==================== 组合运算符注册宏 ====================

// 相等性比较运算符注册宏
#define REGISTER_EQUALITY_OPERATORS() \
    /* 相等性比较运算符 */ \
    REGISTER_COMPARISON_OPERATOR("eq", ==); \
    REGISTER_COMPARISON_OPERATOR("ne", !=)

// 位运算运算符注册宏
#define REGISTER_BITWISE_OPERATORS() \
    /* 位运算运算符 */ \
    REGISTER_BINARY_OPERATOR("bitwiseand", &); \
    REGISTER_BINARY_OPERATOR("bitwiseor", |); \
    REGISTER_BINARY_OPERATOR("bitwisexor", ^); \
    REGISTER_BINARY_OPERATOR("leftshift", <<); \
    REGISTER_BINARY_OPERATOR("rightshift", >>)

// 数值类型通用运算符注册宏
#define REGISTER_NUMERIC_OPERATORS() \
    /* 赋值运算符 */ \
    REGISTER_BINARY_OPERATOR("assign", =); \
    /* 算术运算符 */ \
    REGISTER_BINARY_OPERATOR("add", +); \
    REGISTER_BINARY_OPERATOR("sub", -); \
    REGISTER_BINARY_OPERATOR("mul", *); \
    REGISTER_BINARY_OPERATOR("div", /); \
    /* 比较运算符 */ \
    REGISTER_EQUALITY_OPERATORS(); \
    REGISTER_COMPARISON_OPERATOR("lt", <); \
    REGISTER_COMPARISON_OPERATOR("le", <=); \
    REGISTER_COMPARISON_OPERATOR("gt", >); \
    REGISTER_COMPARISON_OPERATOR("ge", >=); \
    /* 一元运算符 */ \
    REGISTER_UNARY_OPERATOR("unary_plus", +); \
    REGISTER_UNARY_OPERATOR("unary_minus", -); \
    REGISTER_UNARY_OPERATOR("bitwise_not", ~); \
    /* 前后缀自增自减运算符 */ \
    REGISTER_UNARY_OPERATOR("prefix_increment", ++); \
    REGISTER_UNARY_OPERATOR("postfix_increment", ++); \
    REGISTER_UNARY_OPERATOR("prefix_decrement", --); \
    REGISTER_UNARY_OPERATOR("postfix_decrement", --)

// ==================== 类型专用运算符注册宏 ====================

// 整数类型专用运算符注册宏（包含位运算）
#define REGISTER_INTEGER_OPERATORS() \
    REGISTER_NUMERIC_OPERATORS(); \
    REGISTER_BITWISE_OPERATORS()

// 浮点类型专用运算符注册宏（不包含位运算）
#define REGISTER_FLOAT_OPERATORS() \
    REGISTER_NUMERIC_OPERATORS()

// 字符类型专用运算符注册宏（包含位运算）
#define REGISTER_CHAR_OPERATORS() \
    REGISTER_NUMERIC_OPERATORS(); \
    REGISTER_BITWISE_OPERATORS()

// 布尔类型专用运算符注册宏
#define REGISTER_BOOL_OPERATORS() \
    /* 赋值运算符 */ \
    REGISTER_BINARY_OPERATOR("assign", =); \
    /* 逻辑运算符 */ \
    REGISTER_BINARY_OPERATOR("and", &&); \
    REGISTER_BINARY_OPERATOR("or", ||); \
    /* 比较运算符 */ \
    REGISTER_EQUALITY_OPERATORS(); \
    /* 一元运算符 */ \
    REGISTER_UNARY_OPERATOR("not", !)

// 字符串类型专用运算符注册宏
#define REGISTER_STRING_OPERATORS() \
    /* 赋值运算符 */ \
    REGISTER_BINARY_OPERATOR("assign", =); \
    /* 字符串连接运算符 */ \
    REGISTER_BINARY_OPERATOR("concat", +); \
    /* 比较运算符 */ \
    REGISTER_EQUALITY_OPERATORS(); \
    REGISTER_COMPARISON_OPERATOR("lt", <); \
    REGISTER_COMPARISON_OPERATOR("le", <=); \
    REGISTER_COMPARISON_OPERATOR("gt", >); \
    REGISTER_COMPARISON_OPERATOR("ge", >=); \
    /* 下标访问运算符 */ \
    REGISTER_SUBSCRIPT_OPERATOR(); \
    /* 成员访问运算符 */ \
    REGISTER_MEMBER_ACCESS_OPERATOR()

// 数组类型专用运算符注册宏
#define REGISTER_ARRAY_OPERATORS() \
    /* 赋值运算符 */ \
    REGISTER_BINARY_OPERATOR("assign", =); \
    /* 数组连接运算符 */ \
    REGISTER_BINARY_OPERATOR("concat", +); \
    /* 比较运算符 */ \
    REGISTER_EQUALITY_OPERATORS(); \
    /* 下标访问运算符 */ \
    REGISTER_SUBSCRIPT_OPERATOR(); \
    /* 成员访问运算符 */ \
    REGISTER_MEMBER_ACCESS_OPERATOR()

// 字典类型专用运算符注册宏
#define REGISTER_DICT_OPERATORS() \
    /* 赋值运算符 */ \
    REGISTER_BINARY_OPERATOR("assign", =); \
    /* 比较运算符 */ \
    REGISTER_EQUALITY_OPERATORS(); \
    /* 下标访问运算符 */ \
    REGISTER_SUBSCRIPT_OPERATOR(); \
    /* 成员访问运算符 */ \
    REGISTER_MEMBER_ACCESS_OPERATOR()

#endif // INTERPRETER_OPERATOR_MACROS_H 