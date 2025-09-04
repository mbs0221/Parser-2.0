#include <algorithm>
#include <cmath>
#include <cctype>

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "common/logger.h"
#include "interpreter/scope/scope.h"
#include "lexer/token.h"

using namespace std;

// ==================== 基础内置类型构造函数实现 ====================

// BoolType构造函数
BoolType::BoolType() : PrimitiveType("bool", true) {
    // 注册构造函数作为静态方法
    addStaticMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (scope->getArgument<Value>("value")->isNull()) {
            return new Bool(false);  // 默认值
        } else if (scope->getArgument<Value>("value")->isInstance()) {
            if (Bool* boolVal = dynamic_cast<Bool*>(scope->getArgument<Value>("value"))) {
                return new Bool(boolVal->getValue());
            } else if (Integer* intVal = dynamic_cast<Integer*>(scope->getArgument<Value>("value"))) {
                return new Bool(intVal->getValue() != 0);
            } else if (Double* doubleVal = dynamic_cast<Double*>(scope->getArgument<Value>("value"))) {
                return new Bool(doubleVal->getValue() != 0.0);
            } else if (String* strVal = dynamic_cast<String*>(scope->getArgument<Value>("value"))) {
                string str = strVal->getValue();
                return new Bool(str == "true" || str == "1");
            }
        }
        return nullptr;
    }, "bool(...)"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Bool* boolInstance = scope->getThis<Bool>()) {
            return new String(boolInstance->toString());
        }
        return nullptr;
    }, "toString()"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Bool* boolInstance = scope->getThis<Bool>()) {
            return new Bool(boolInstance->toBool());
        }
        return nullptr;
    }, "toBool()"));
}

// NullType构造函数
NullType::NullType() : PrimitiveType("null", false) {
    // 注册空值类型的方法
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Null* nullInstance = scope->getThis<Null>()) {
            return new String(nullInstance->toString());
        }
        return nullptr;
    }, "toString()"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Null* nullInstance = scope->getThis<Null>()) {
            return new Bool(nullInstance->toBool());
        }
        return nullptr;
    }, "toBool()"));
}

// CharType构造函数
CharType::CharType() : NumericType("char", true) {
    // 注册构造函数作为静态方法
    addStaticMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (scope->getArgument<Value>("value")->isNull()) {
            return new Char('\0');  // 默认值
        } else if (scope->getArgument<Value>("value")->isInstance()) {
                if (Char* charVal = dynamic_cast<Char*>(scope->getArgument<Value>("value"))) {
                return new Char(charVal->getValue());
            } else if (Integer* intVal = dynamic_cast<Integer*>(scope->getArgument<Value>("value"))) {
                return new Char((char)intVal->getValue());
            } else if (String* strVal = dynamic_cast<String*>(scope->getArgument<Value>("value"))) {
                string str = strVal->toString();
                return new Char(str.empty() ? '\0' : str[0]);
            }
        }
        return nullptr;
    }, "char(...)"));
    
    // 注册字符类型的方法
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Char* charInstance = scope->getThis<Char>()) {
            return new String(charInstance->toString());
        }
        return nullptr;
    }, "toString()"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Char* charInstance = scope->getThis<Char>()) {
            return new Bool(charInstance->toBool());
        }
        return nullptr;
    }, "toBool()"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Char* charInstance = scope->getThis<Char>()) {
            return new Char(charInstance->getValue());
        }
        return nullptr;
    }, "getValue()"));
}

// IntegerType构造函数
IntegerType::IntegerType() : NumericType("int", true) {
    // 注册构造函数作为静态方法
    addStaticMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (scope->getArgument<Value>("value")->isNull()) {
            return new Integer(0);  // 默认值
        } else if (scope->getArgument<Value>("value")->isInstance()) {
            if (Integer* intVal = dynamic_cast<Integer*>(scope->getArgument<Value>("value"))) {
                return new Integer(intVal->getValue());
            } else if (Double* doubleVal = dynamic_cast<Double*>(scope->getArgument<Value>("value"))) {
                return new Integer((int)doubleVal->getValue());
            } else if (String* strVal = dynamic_cast<String*>(scope->getArgument<Value>("value"))) {
                try {
                    return new Integer(stoi(strVal->getValue()));
                } catch (...) {
                    return new Integer(0);
                }
            }
        }
        return nullptr;
    }, "int(...)"));
    
    // 注册整数类型的方法
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            return new String(intInstance->toString());
        }
        return nullptr;
    }, "toString()"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            return new Bool(intInstance->toBool());
        }
        return nullptr;
    }, "toBool()"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            return new Integer(intInstance->getValue());
        }
        return nullptr;
    }, "getValue()"));
    
    // 注册算术运算方法 - 使用操作符名称
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            if (Integer* other = scope->getArgument<Integer>("other")) {
                return new Integer(*intInstance + *other);
            }
        }
        return nullptr;
    }, "add(other)"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            if (Integer* other = scope->getArgument<Integer>("other")) {
                return new Integer(*intInstance - *other);
            }
        }
        return nullptr;
    }, "subtract(other)"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            if (Integer* other = scope->getArgument<Integer>("other")) {
                return new Integer(*intInstance * *other);
            }
        }
        return nullptr;
    }, "multiply(other)"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            if (Integer* other = scope->getArgument<Integer>("other")) {
                return new Integer(*intInstance / *other);
            }
        }
        return nullptr;
    }, "divide(other)"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            if (Integer* other = scope->getArgument<Integer>("other")) {
                return new Integer(*intInstance % *other);
            }
        }
        return nullptr;
    }, "modulo(other)"));
    
    // 注册比较运算方法 - 使用操作符名称
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            if (Integer* other = scope->getArgument<Integer>("other")) {
                return new Bool(*intInstance > *other);
            }
        }
        return nullptr;
    }, "greaterThan(other)"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            if (Integer* other = scope->getArgument<Integer>("other")) {
                return new Bool(*intInstance < *other);
            }
        }
        return nullptr;
    }, "lessThan(other)"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            if (Integer* other = scope->getArgument<Integer>("other")) {
                return new Bool(*intInstance == *other);
            }
        }
        return nullptr;
    }, "equals(other)"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            if (Integer* other = scope->getArgument<Integer>("other")) {
                return new Bool(*intInstance != *other);
            }
        }
        return nullptr;
    }, "notEquals(other)"));

    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Integer* intInstance = scope->getThis<Integer>()) {
            return new Double(intInstance->toDouble());
        }
        return nullptr;
    }, "toDouble()"));
}

// DoubleType构造函数
DoubleType::DoubleType() : NumericType("double", true) {
    // 注册构造函数作为静态方法
    addStaticMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Double* doubleVal = scope->getArgument<Double>("value")) {
                return new Double(doubleVal->getValue());
        } else if (Integer* intVal = scope->getArgument<Integer>("value")) {
                return new Double((double)intVal->getValue());
        } else if (String* strVal = scope->getArgument<String>("value")) {
                try {
                    return new Double(stod(strVal->getValue()));
                } catch (...) {
                    return new Double(0.0);
            }
        }
        return nullptr;
    }, "double(...)"));
    
    // 注册双精度浮点类型的方法
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Double* doubleInstance = scope->getThis<Double>()) {
            return new String(doubleInstance->toString());
        }
        return nullptr;
    }, "toString()"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Double* doubleInstance = scope->getThis<Double>()) {
            return new Bool(doubleInstance->toBool());
        }
        return nullptr;
    }, "toBool()"));
    
    addUserMethod(new BuiltinFunction([](class Scope* scope) -> Value* {
        if (Double* doubleInstance = scope->getThis<Double>()) {
            return new Double(doubleInstance->getValue());
        }
        return nullptr;
    }, "getValue()"));
}

// ==================== 自动注册内置类型 ====================
// 在main()之前自动注册所有内置类型
// 注意：必须放在所有类型构造函数实现之后

// 注册基本类型（继承自PrimitiveType）
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

// 添加调试信息，验证注册是否成功
void debugTypeRegistration() {
    TypeRegistry* globalRegistry = TypeRegistry::getGlobalInstance();
    if (globalRegistry) {
        LOG_DEBUG("=== Debug: Built-in Types Registration ===");
        LOG_DEBUG("Global registry has " + to_string(globalRegistry->types.size()) + " types:");
        for (const auto& pair : globalRegistry->types) {
            LOG_DEBUG("  - " + pair.first + " -> " + pair.second->getTypeName());
        }
        LOG_DEBUG("=========================================");
    }
}

// 在程序启动时调用调试函数
static bool debugInitialized = false;
void ensureDebugInitialized() {
    if (!debugInitialized) {
        debugTypeRegistration();
        debugInitialized = true;
    }
}

// ==================== 显式初始化函数 ====================

// 统一的动态库初始化函数
extern "C" void initialize_interpreter_types() {
    LOG_INFO("Initializing interpreter types...");
    
    // 调用所有类型的显式初始化函数
    register_bool_type_explicit();
    register_null_type_explicit();
    register_char_type_explicit();
    register_int_type_explicit();
    register_double_type_explicit();
    register_string_type_explicit();
    register_array_type_explicit();
    register_dict_type_explicit();
    
    LOG_INFO("Interpreter types initialization completed.");
    
    // 显示调试信息
    debugTypeRegistration();
}

// TypeRegistry的静态初始化函数
void TypeRegistry::initializeBuiltinTypes() {
    initialize_interpreter_types();
}
