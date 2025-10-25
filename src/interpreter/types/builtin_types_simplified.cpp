#include <algorithm>
#include <cmath>
#include <cctype>

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "common/logger.h"
#include "interpreter/scope/scope.h"
#include "lexer/token.h"

using namespace std;

// ==================== 简化的基础内置类型构造函数实现 ====================
// 这些类型只提供最基本的构造函数，所有转换方法都通过标准库实现

// BoolType构造函数 - 只提供基本构造函数
BoolType::BoolType() : PrimitiveType("bool", true) {
    // 只注册基本构造函数
    addStaticMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (scope->getArgument<Value>("value")->isNull()) {
            return new Bool(false);  // 默认值
        } else if (scope->getArgument<Value>("value")->isInstance()) {
            if (Bool* boolVal = dynamic_cast<Bool*>(scope->getArgument<Value>("value"))) {
                return new Bool(boolVal->getValue());
            }
        }
        return nullptr;
    }, "bool(...)"));
}

// NullType构造函数 - 只提供基本构造函数
NullType::NullType() : PrimitiveType("null", false) {
    // 空值类型不需要额外的方法
}

// CharType构造函数 - 只提供基本构造函数
CharType::CharType() : NumericType("char", true) {
    // 只注册基本构造函数
    addStaticMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (scope->getArgument<Value>("value")->isNull()) {
            return new Char('\0');  // 默认值
        } else if (scope->getArgument<Value>("value")->isInstance()) {
            if (Char* charVal = dynamic_cast<Char*>(scope->getArgument<Value>("value"))) {
                return new Char(charVal->getValue());
            }
        }
        return nullptr;
    }, "char(...)"));
}

// IntegerType构造函数 - 只提供基本构造函数
IntegerType::IntegerType() : NumericType("int", true) {
    // 只注册基本构造函数
    addStaticMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (scope->getArgument<Value>("value")->isNull()) {
            return new Integer(0);  // 默认值
        } else if (scope->getArgument<Value>("value")->isInstance()) {
            if (Integer* intVal = dynamic_cast<Integer*>(scope->getArgument<Value>("value"))) {
                return new Integer(intVal->getValue());
            }
        }
        return nullptr;
    }, "int(...)"));
}

// DoubleType构造函数 - 只提供基本构造函数
DoubleType::DoubleType() : NumericType("double", true) {
    // 只注册基本构造函数
    addStaticMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (scope->getArgument<Value>("value")->isNull()) {
            return new Double(0.0);  // 默认值
        } else if (scope->getArgument<Value>("value")->isInstance()) {
            if (Double* doubleVal = dynamic_cast<Double*>(scope->getArgument<Value>("value"))) {
                return new Double(doubleVal->getValue());
            }
        }
        return nullptr;
    }, "double(...)"));
}

// ==================== 自动注册内置类型 ====================
// 在main()之前自动注册所有内置类型
// 注意：必须放在所有类型构造函数实现之后

// 全局类型注册表实例
TypeRegistry* globalRegistry = nullptr;

// 确保全局注册表已初始化
void ensureGlobalRegistry() {
    if (!globalRegistry) {
        globalRegistry = new TypeRegistry();
    }
}

// 注册内置类型的宏
#define REGISTER_BUILTIN_TYPE(name, TypeClass) \
    do { \
        ensureGlobalRegistry(); \
        globalRegistry->registerType(name, new TypeClass()); \
    } while(0)

// 初始化函数
void initialize_interpreter_types() {
    LOG_INFO("Initializing interpreter types...");
    
    // 注册基础类型（继承自PrimitiveType）
    REGISTER_BUILTIN_TYPE(bool, BoolType)      // BoolType -> PrimitiveType -> ObjectType
    REGISTER_BUILTIN_TYPE(null, NullType)      // NullType -> PrimitiveType -> ObjectType

    // 注册数值类型（继承自NumericType）
    REGISTER_BUILTIN_TYPE(char, CharType)      // CharType -> NumericType -> PrimitiveType -> ObjectType
    REGISTER_BUILTIN_TYPE(int, IntegerType)    // IntegerType -> NumericType -> PrimitiveType -> ObjectType
    REGISTER_BUILTIN_TYPE(double, DoubleType)  // DoubleType -> NumericType -> PrimitiveType -> ObjectType

    // 注册容器类型（继承自ContainerType）
    REGISTER_BUILTIN_TYPE(string, StringType)  // StringType -> ContainerType -> ObjectType
    REGISTER_BUILTIN_TYPE(array, ArrayType)    // ArrayType -> ContainerType -> ObjectType
    REGISTER_BUILTIN_TYPE(dict, DictType)      // DictType -> ContainerType -> ObjectType
    
    LOG_INFO("Interpreter types initialization completed.");
}

// TypeRegistry的静态初始化函数
void TypeRegistry::initializeBuiltinTypes() {
    initialize_interpreter_types();
}