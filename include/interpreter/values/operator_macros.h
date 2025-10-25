#ifndef INTERPRETER_OPERATOR_MACROS_NEW_H
#define INTERPRETER_OPERATOR_MACROS_NEW_H


// ==================== 运算符注册宏定义 ====================
// 这个文件包含了所有用于简化运算符方法注册的宏定义
// 使用addUserMethod而不是registerMethod

// ==================== 基础运算符注册宏 ====================

// 简化二元运算符方法注册的宏（带类型参数）
#define REGISTER_BINARY_OPERATOR(typeName, opName, opSymbol) \
    do { \
        auto method = [](Scope* scope) -> Value* { \
            Value* instance = scope->getVariable<Value>("this"); \
            Value* other = scope->getVariable<Value>("other"); \
            if (instance && other) { \
                auto a = dynamic_cast<typeName*>(instance); \
                auto b = dynamic_cast<typeName*>(other); \
                if (a && b) { \
                    return new typeName((*a) operator##opSymbol (*b)); \
                } \
            } \
            return nullptr; \
        }; \
        BuiltinFunction* func = new BuiltinFunction(method, (string(opName) + "(other:" + #typeName + ")").c_str()); \
        addUserMethod(func, VIS_PUBLIC); \
    } while(0)

// 简化一元运算符方法注册的宏（带类型参数）
#define REGISTER_UNARY_OPERATOR(typeName, opName, opSymbol) \
    do { \
        auto method = [](Scope* scope) -> Value* { \
            Value* instance = scope->getVariable<Value>("this"); \
            if (instance) { \
                auto a = dynamic_cast<typeName*>(instance); \
                if (a) { \
                    return new typeName(operator##opSymbol (*a)); \
                } \
            } \
            return nullptr; \
        }; \
        BuiltinFunction* func = new BuiltinFunction(method, (string(opName) + "()").c_str()); \
        addUserMethod(func, VIS_PUBLIC); \
    } while(0)

// 简化比较运算符方法注册的宏（带类型参数）
#define REGISTER_COMPARISON_OPERATOR(typeName, opName, opSymbol) \
    REGISTER_BINARY_OPERATOR(typeName, opName, opSymbol)

// 简化赋值运算符方法注册的宏（带类型参数）
#define REGISTER_ASSIGN_OPERATOR(typeName) \
    do { \
        auto method = [](Scope* scope) -> Value* { \
            Value* instance = scope->getVariable<Value>("this"); \
            Value* other = scope->getVariable<Value>("other"); \
            if (instance && other) { \
                auto a = dynamic_cast<typeName*>(instance); \
                auto b = dynamic_cast<typeName*>(other); \
                if (a && b) { \
                    *a = *b; \
                    return a; /* 返回当前对象以实现连续赋值 */ \
                } \
            } \
            return nullptr; \
        }; \
        BuiltinFunction* func = new BuiltinFunction(method, ("=(other:" + string(#typeName) + ")").c_str()); \
        addUserMethod(func, VIS_PUBLIC); \
    } while(0)

// 简化new方法注册的宏
#define REGISTER_NEW_METHOD(typeName) \
    do { \
        auto method = [](Scope* scope) -> Value* { \
            return new typeName(); \
        }; \
        BuiltinFunction* func = new BuiltinFunction(method, "new()"); \
        addUserMethod(func, VIS_PUBLIC); \
    } while(0)

// 简化下标访问运算符方法注册的宏（双目运算）
#define REGISTER_SUBSCRIPT_OPERATOR(typeName) \
    do { \
        auto method = [](Scope* scope) -> Value* { \
            Value* instance = scope->getVariable<Value>("this"); \
            Value* index = scope->getVariable<Value>("index"); \
            if (instance && index) { \
                auto a = dynamic_cast<typeName*>(instance); \
                auto intIndex = dynamic_cast<Integer*>(index); \
                if (a && intIndex) { \
                    return new Char((*a)[intIndex->getValue()]); \
                } \
            } \
            return nullptr; \
        }; \
        BuiltinFunction* func = new BuiltinFunction(method, "[](index:any)"); \
        addUserMethod(func, VIS_PUBLIC); \
    } while(0)

// 简化字典类型new方法注册的宏（处理键值对参数）
#define REGISTER_DICT_NEW_METHOD() \
    do { \
        auto method = [](Scope* scope) -> Value* { \
            Dict* newDict = new Dict(); \
            /* 处理键值对参数 - 这里需要从scope中获取参数 */ \
            /* 暂时简化实现 */ \
            return newDict; \
        }; \
        BuiltinFunction* func = new BuiltinFunction(method, "new()"); \
        addUserMethod(func, VIS_PUBLIC); \
    } while(0)

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

#endif // INTERPRETER_OPERATOR_MACROS_NEW_H
