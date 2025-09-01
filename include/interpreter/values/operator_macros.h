#ifndef INTERPRETER_OPERATOR_MACROS_H
#define INTERPRETER_OPERATOR_MACROS_H

// ==================== 运算符注册宏定义 ====================
// 这个文件包含了所有用于简化运算符方法注册的宏定义

// ==================== 基础运算符注册宏 ====================

// 简化二元运算符方法注册的宏（带类型参数）
#define REGISTER_BINARY_OPERATOR(typeName, opName, opSymbol) \
    registerMethod(opName, [](Value* instance, vector<Value*>& args) -> Value* { \
        if (args.size() >= 1 && args[0]) { \
            auto a = dynamic_cast<typeName*>(instance); \
            auto b = dynamic_cast<typeName*>(args[0]); \
            if (a && b) { \
                return new typeName((*a) operator##opSymbol (*b)); \
            } \
        } \
        return nullptr; \
    })

// 简化一元运算符方法注册的宏（带类型参数）
#define REGISTER_UNARY_OPERATOR(typeName, opName, opSymbol) \
    registerMethod(opName, [](Value* instance, vector<Value*>& args) -> Value* { \
        auto a = dynamic_cast<typeName*>(instance); \
        if (a) { \
            return new typeName(operator##opSymbol (*a)); \
        } \
        return nullptr; \
    })

// 简化比较运算符方法注册的宏（带类型参数）
#define REGISTER_COMPARISON_OPERATOR(typeName, opName, opSymbol) \
    REGISTER_BINARY_OPERATOR(typeName, opName, opSymbol)

// 简化赋值运算符方法注册的宏（带类型参数）
#define REGISTER_ASSIGN_OPERATOR(typeName) \
    registerMethod("=", [](Value* instance, vector<Value*>& args) -> Value* { \
        if (args.size() >= 1 && args[0]) { \
            auto a = dynamic_cast<typeName*>(instance); \
            auto b = dynamic_cast<typeName*>(args[0]); \
            if (a && b) { \
                *a = *b; \
                return a; /* 返回当前对象以实现连续赋值 */ \
            } \
        } \
        return nullptr; \
    })

// 简化new方法注册的宏
#define REGISTER_NEW_METHOD(typeName) \
    registerMethod("new", [](Value* instance, vector<Value*>& args) -> Value* { \
        return new typeName(); \
    })

// 简化下标访问运算符方法注册的宏（双目运算）
#define REGISTER_SUBSCRIPT_OPERATOR(typeName) \
    registerMethod("[]", [](Value* instance, vector<Value*>& args) -> Value* { \
        if (args.size() >= 1 && args[0]) { \
            auto a = dynamic_cast<typeName*>(instance); \
            auto index = dynamic_cast<Integer*>(args[0]); \
            if (a && index) { \
                return new Char((*a)[index->getValue()]); \
            } \
        } \
        return nullptr; \
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

// 相等性比较运算符注册宏（带类型参数）
#define REGISTER_EQUALITY_OPERATORS(typeName) \
    /* 相等性比较运算符 */ \
    REGISTER_COMPARISON_OPERATOR(typeName, "==", ==); \
    REGISTER_COMPARISON_OPERATOR(typeName, "!=", !=)

// 位运算运算符注册宏（带类型参数）
#define REGISTER_BITWISE_OPERATORS(typeName) \
    /* 位运算运算符 */ \
    REGISTER_BINARY_OPERATOR(typeName, "&", &); \
    REGISTER_BINARY_OPERATOR(typeName, "|", |); \
    REGISTER_BINARY_OPERATOR(typeName, "^", ^); \
    REGISTER_BINARY_OPERATOR(typeName, "<<", <<); \
    REGISTER_BINARY_OPERATOR(typeName, ">>", >>)

// 数值类型通用运算符注册宏（带类型参数）
#define REGISTER_NUMERIC_OPERATORS(typeName) \
    /* 赋值运算符 */ \
    REGISTER_ASSIGN_OPERATOR(typeName); \
    /* 算术运算符 */ \
    REGISTER_BINARY_OPERATOR(typeName, "+", +); \
    REGISTER_BINARY_OPERATOR(typeName, "-", -); \
    REGISTER_BINARY_OPERATOR(typeName, "*", *); \
    REGISTER_BINARY_OPERATOR(typeName, "/", /); \
    /* 比较运算符 */ \
    REGISTER_EQUALITY_OPERATORS(typeName); \
    REGISTER_COMPARISON_OPERATOR(typeName, "<", <); \
    REGISTER_COMPARISON_OPERATOR(typeName, "<=", <=); \
    REGISTER_COMPARISON_OPERATOR(typeName, ">", >); \
    REGISTER_COMPARISON_OPERATOR(typeName, ">=", >=); \
    /* 一元运算符 */ \
    REGISTER_UNARY_OPERATOR(typeName, "+", +); \
    REGISTER_UNARY_OPERATOR(typeName, "-", -); \
    REGISTER_UNARY_OPERATOR(typeName, "~", ~); \
    /* 前后缀自增自减运算符 */ \
    REGISTER_UNARY_OPERATOR(typeName, "++", ++); \
    REGISTER_UNARY_OPERATOR(typeName, "--", --)

// ==================== 类型专用运算符注册宏 ====================

// 整数类型专用运算符注册宏（包含位运算）
#define REGISTER_INTEGER_OPERATORS(typeName) \
    REGISTER_NUMERIC_OPERATORS(typeName); \
    REGISTER_BITWISE_OPERATORS(typeName)

// 浮点类型专用运算符注册宏（不包含位运算）
#define REGISTER_FLOAT_OPERATORS(typeName) \
    REGISTER_NUMERIC_OPERATORS(typeName)

// 字符类型专用运算符注册宏（包含位运算）
#define REGISTER_CHAR_OPERATORS(typeName) \
    REGISTER_NUMERIC_OPERATORS(typeName); \
    REGISTER_BITWISE_OPERATORS(typeName)

// 布尔类型专用运算符注册宏
#define REGISTER_BOOL_OPERATORS(typeName) \
    /* 赋值运算符 */ \
    REGISTER_BINARY_OPERATOR(typeName, "=", =); \
    /* 逻辑运算符 */ \
    REGISTER_BINARY_OPERATOR(typeName, "&&", &&); \
    REGISTER_BINARY_OPERATOR(typeName, "||", ||); \
    /* 比较运算符 */ \
    REGISTER_EQUALITY_OPERATORS(typeName); \
    /* 一元运算符 */ \
    REGISTER_UNARY_OPERATOR(typeName, "!", !);

// 字符串类型专用运算符注册宏
#define REGISTER_STRING_OPERATORS(typeName) \
    /* 赋值运算符 */ \
    REGISTER_BINARY_OPERATOR(typeName, "=", =); \
    /* 字符串连接运算符 */ \
    REGISTER_BINARY_OPERATOR(typeName, "+", +); \
    /* 比较运算符 */ \
    REGISTER_EQUALITY_OPERATORS(typeName); \
    REGISTER_COMPARISON_OPERATOR(typeName, "<", <); \
    REGISTER_COMPARISON_OPERATOR(typeName, "<=", <=); \
    REGISTER_COMPARISON_OPERATOR(typeName, ">", >); \
    REGISTER_COMPARISON_OPERATOR(typeName, ">=", >=)

// 数组类型专用运算符注册宏
#define REGISTER_ARRAY_OPERATORS(typeName) \
    /* 赋值运算符 */ \
    REGISTER_BINARY_OPERATOR(typeName, "=", =); \
    /* 数组连接运算符 */ \
    REGISTER_BINARY_OPERATOR(typeName, "+", +); \
    /* 比较运算符 */ \
    REGISTER_EQUALITY_OPERATORS(typeName); \
    /* 下标访问运算符 */ \
    REGISTER_SUBSCRIPT_OPERATOR(typeName)

// 字典类型专用运算符注册宏
#define REGISTER_DICT_OPERATORS(typeName) \
    /* 赋值运算符 */ \
    REGISTER_BINARY_OPERATOR(typeName, "=", =); \
    /* 比较运算符 */ \
    REGISTER_EQUALITY_OPERATORS(typeName); \
    /* 下标访问运算符 */ \
    REGISTER_SUBSCRIPT_OPERATOR(typeName)

#endif // INTERPRETER_OPERATOR_MACROS_H 