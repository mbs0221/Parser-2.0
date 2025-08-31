#include "interpreter/type_converter.h"
#include "interpreter/builtin_type.h"
#include "interpreter/interpreter.h"
#include "parser/statement.h"
#include "interpreter/logger.h"
#include "lexer/token.h"
#include <stdexcept>

using namespace std;

// 构造函数
TypeConverter::TypeConverter(TypeRegistry* registry, Interpreter* interp) 
    : typeRegistry(registry), interpreter(interp) {
    LOG_DEBUG("TypeConverter created");
}

// 设置类型注册表
void TypeConverter::setTypeRegistry(TypeRegistry* registry) {
    std::lock_guard<std::mutex> lock(converterMutex);
    typeRegistry = registry;
    LOG_DEBUG("TypeConverter: TypeRegistry set");
}

// 设置解释器
void TypeConverter::setInterpreter(Interpreter* interp) {
    std::lock_guard<std::mutex> lock(converterMutex);
    interpreter = interp;
    LOG_DEBUG("TypeConverter: Interpreter set");
}

// 转换AST类型到运行时类型
ObjectType* TypeConverter::convertASTTypeToRuntimeType(lexer::Type* astType) {
    if (!astType) return nullptr;
    
    std::lock_guard<std::mutex> lock(converterMutex);
    
    if (!typeRegistry) {
        LOG_ERROR("TypeRegistry not available for type conversion");
        return nullptr;
    }
    
    // 根据AST类型的标签转换为对应的运行时类型
    switch (astType->Tag) {
        case NUM:
            return typeRegistry->getType("int");
        case REAL:
        case DOUBLE:
            return typeRegistry->getType("double");
        case CHAR:
            return typeRegistry->getType("char");
        case BOOL:
            return typeRegistry->getType("bool");
        case STR:
            return typeRegistry->getType("string");
        case ARRAY:
            return typeRegistry->getType("array");
        case DICT:
            return typeRegistry->getType("dict");
        case NULL_VALUE:
            return typeRegistry->getType("null");
        case ID:
            // 用户定义类型需要从类型注册表中查找
            return typeRegistry->getType(astType->word);
        default:
            LOG_ERROR("Unknown AST type tag: " + to_string(astType->Tag));
            return nullptr;
    }
}

// 转换结构体定义
StructType* TypeConverter::convertStructDefinition(StructDefinition* structDef, const vector<pair<string, Value*>>& memberInitialValues) {
    if (!structDef) {
        LOG_ERROR("StructDefinition is null");
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(converterMutex);
    
    if (!typeRegistry) {
        LOG_ERROR("TypeRegistry not set in TypeConverter");
        return nullptr;
    }
    
    LOG_DEBUG("Converting struct definition: " + structDef->name);
    
    // 检查类型是否已存在
    if (typeRegistry->hasType(structDef->name)) {
        LOG_WARN("Struct type '" + structDef->name + "' already exists, skipping conversion");
        return nullptr;
    }
    
    // 创建结构体类型
    StructType* structType = typeRegistry->createStructType(structDef->name);
    if (!structType) {
        LOG_ERROR("Failed to create StructType for: " + structDef->name);
        return nullptr;
    }
    
    // 添加成员
    for (const auto& member : structDef->members) {
        ObjectType* memberType = convertASTTypeToRuntimeType(member.type);
        if (memberType) {
            structType->addMember(member.name, memberType, VIS_PUBLIC);
            LOG_DEBUG("Added member '" + member.name + "' to struct '" + structDef->name + "'");
        } else {
            LOG_ERROR("Failed to convert member type for: " + member.name);
        }
    }
    
    // 生成默认构造函数
    auto constructor = [this, structDef](Value* instance, vector<Value*>& args) -> Value* {
        LOG_DEBUG("Executing struct constructor for: " + structDef->name);
        
        // 创建实例
        Dict* structInstance = new Dict();
        
        // 处理参数初始化
        if (!args.empty() && dynamic_cast<Dict*>(args[0])) {
            // 字典初始化语法：Person {name: "Alice", age: 25}
            Dict* memberDict = dynamic_cast<Dict*>(args[0]);
            initializeStructFromDict(structInstance, structDef, memberDict);
        } else {
            // 参数初始化语法：Person("Alice", 25)
            initializeStructFromArgs(structInstance, structDef, args);
        }
        
        return structInstance;
    };
    
    // 添加构造函数（同名方法）
    structType->addMethod(structDef->name, constructor, VIS_PUBLIC);
    LOG_DEBUG("Added constructor '" + structDef->name + "' to struct '" + structDef->name + "'");
    
    // 添加setMember方法，用于成员赋值
    auto setMemberMethod = [](Value* instance, vector<Value*>& args) -> Value* {
        if (args.size() >= 2) {
            if (String* memberName = dynamic_cast<String*>(args[0])) {
                if (Dict* dictInstance = dynamic_cast<Dict*>(instance)) {
                    dictInstance->setEntry(memberName->getValue(), args[1]);
                    return args[1];  // 返回设置的值
                }
            }
        }
        return nullptr;
    };
    structType->addMethod("setMember", setMemberMethod, VIS_PUBLIC);
    LOG_DEBUG("Added setMember method to struct '" + structDef->name + "'");
    
    // 添加new方法，用于创建新实例，实质是调用Object的构造函数
    auto newMethod = [this, structDef](Value* instance, vector<Value*>& args) -> Value* {
        LOG_DEBUG("Executing struct new method for: " + structDef->name);
        
        // 创建新实例，调用Object的构造函数
        Dict* newInstance = new Dict();
        
        // 处理参数初始化
        if (!args.empty() && dynamic_cast<Dict*>(args[0])) {
            // 字典初始化语法：Person {name: "Alice", age: 25}
            Dict* memberDict = dynamic_cast<Dict*>(args[0]);
            initializeStructFromDict(newInstance, structDef, memberDict);
        } else {
            // 参数初始化语法：Person("Alice", 25)
            initializeStructFromArgs(newInstance, structDef, args);
        }
        
        return newInstance;
    };
    structType->addMethod("new", newMethod, VIS_PUBLIC);
    LOG_DEBUG("Added new method to struct '" + structDef->name + "'");
    
    // 添加assign方法，用于深拷贝赋值，本质是调用Object的赋值运算
    auto assignMethod = [](Value* instance, vector<Value*>& args) -> Value* {
        if (args.size() >= 1 && args[0]) {
            // 通过类型系统调用assign方法，不强制转换成Dict类型
            if (instance && instance->getValueType()) {
                return instance->getValueType()->callMethod(instance, "assign", args);
            }
        }
        return nullptr;
    };
    structType->addMethod("assign", assignMethod, VIS_PUBLIC);
    LOG_DEBUG("Added assign method to struct '" + structDef->name + "'");
    
    // 添加toString方法，用于字符串表示，本质是调用Value类型的toString方法
    auto toStringMethod = [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new String(instance->toString());
        }
        return nullptr;
    };
    structType->addMethod("toString", toStringMethod, VIS_PUBLIC);
    LOG_DEBUG("Added toString method to struct '" + structDef->name + "'");
    
    // 注意：StructDefinition没有methods字段，结构体的方法通过其他方式处理
    LOG_INFO("Successfully converted struct '" + structDef->name + "' with " + 
             to_string(structDef->members.size()) + " members");
    
    return structType;
}

// 转换类定义
ClassType* TypeConverter::convertClassDefinition(ClassDefinition* classDef, const vector<pair<string, Value*>>& memberInitialValues) {
    if (!classDef) {
        LOG_ERROR("ClassDefinition is null");
        return nullptr;
    }
    
    std::lock_guard<std::mutex> lock(converterMutex);
    
    if (!typeRegistry) {
        LOG_ERROR("TypeRegistry not set in TypeConverter");
        return nullptr;
    }
    
    LOG_DEBUG("Converting class definition: " + classDef->name);
    
    // 检查类型是否已存在
    if (typeRegistry->hasType(classDef->name)) {
        LOG_WARN("Class type '" + classDef->name + "' already exists, skipping conversion");
        return nullptr;
    }
    
    // 创建类类型
    ClassType* classType = nullptr;
    
    // 处理继承关系
    if (!classDef->baseClass.empty()) {
        // 检查父类是否存在
        ObjectType* parentType = typeRegistry->getType(classDef->baseClass);
        if (parentType) {
            ClassType* parentClassType = dynamic_cast<ClassType*>(parentType);
            if (parentClassType) {
                classType = typeRegistry->createClassType(classDef->name, classDef->baseClass);
                LOG_DEBUG("Created class '" + classDef->name + "' with parent '" + classDef->baseClass + "'");
            } else {
                LOG_ERROR("Parent type '" + classDef->baseClass + "' is not a class type");
                return nullptr;
            }
        } else {
            LOG_ERROR("Parent class '" + classDef->baseClass + "' not found");
            return nullptr;
        }
    } else {
        // 没有继承关系
        classType = typeRegistry->createClassType(classDef->name);
        LOG_DEBUG("Created class '" + classDef->name + "' without inheritance");
    }
    
    if (!classType) {
        LOG_ERROR("Failed to create ClassType for: " + classDef->name);
        return nullptr;
    }
    
    // 添加成员
    for (const auto& member : classDef->members) {
        ObjectType* memberType = convertASTTypeToRuntimeType(member.type);
        if (memberType) {
            // 转换visibility字符串为VisibilityType枚举
            VisibilityType visibility = VIS_PUBLIC;
            if (member.visibility == "private") {
                visibility = VIS_PRIVATE;
            } else if (member.visibility == "protected") {
                visibility = VIS_PROTECTED;
            }
            
            classType->addMember(member.name, memberType, visibility);
            LOG_DEBUG("Added member '" + member.name + "' to class '" + classDef->name + 
                     "' with visibility " + member.visibility);
        } else {
            LOG_ERROR("Failed to convert member type for: " + member.name);
        }
    }
    
    // 生成默认构造函数
    auto constructor = [this, classDef](Value* instance, vector<Value*>& args) -> Value* {
        LOG_DEBUG("Executing class constructor for: " + classDef->name);
        
        // 创建实例
        Dict* classInstance = new Dict();
        
        // 处理参数初始化
        if (!args.empty() && dynamic_cast<Dict*>(args[0])) {
            // 字典初始化语法：Student {name: "Bob", grade: 10}
            Dict* memberDict = dynamic_cast<Dict*>(args[0]);
            initializeClassFromDict(classInstance, classDef, memberDict);
        } else {
            // 参数初始化语法：Student("Bob", 10)
            initializeClassFromArgs(classInstance, classDef, args);
        }
        
        return classInstance;
    };
    
    // 添加构造函数（同名方法）
    classType->addMethod(classDef->name, constructor, VIS_PUBLIC);
    LOG_DEBUG("Added constructor '" + classDef->name + "' to class '" + classDef->name + "'");
    
    // 添加其他方法
    for (const auto& method : classDef->methods) {
        // 创建方法包装器
        auto methodWrapper = [this, method](Value* instance, vector<Value*>& args) -> Value* {
            if (!getInterpreter()) {
                LOG_ERROR("Interpreter not available for method execution");
                return nullptr;
            }
            
            if (method->body) {
                getInterpreter()->visit(method->body);
            }
            LOG_DEBUG("Executing class method: " + method->name);
            return nullptr;
        };
        
        // 转换visibility字符串为VisibilityType枚举
        VisibilityType visibility = VIS_PUBLIC;
        if (method->visibility == "private") {
            visibility = VIS_PRIVATE;
        } else if (method->visibility == "protected") {
            visibility = VIS_PROTECTED;
        }
        
        classType->addMethod(method->name, methodWrapper, visibility);
        LOG_DEBUG("Added method '" + method->name + "' to class '" + classDef->name + 
                 "' with visibility " + method->visibility);
    }
    
    // 添加setMember方法，用于成员赋值
    auto setMemberMethod = [](Value* instance, vector<Value*>& args) -> Value* {
        if (args.size() >= 2) {
            if (String* memberName = dynamic_cast<String*>(args[0])) {
                if (Dict* dictInstance = dynamic_cast<Dict*>(instance)) {
                    dictInstance->setEntry(memberName->getValue(), args[1]);
                    return args[1];  // 返回设置的值
                }
            }
        }
        return nullptr;
    };
    classType->addMethod("setMember", setMemberMethod, VIS_PUBLIC);
    LOG_DEBUG("Added setMember method to class '" + classDef->name + "'");
    
    // 添加new方法，用于创建新实例，实质是调用Object的构造函数
    auto newMethod = [this, classDef](Value* instance, vector<Value*>& args) -> Value* {
        LOG_DEBUG("Executing class new method for: " + classDef->name);
        
        // 创建新实例，调用Object的构造函数
        Dict* newInstance = new Dict();
        
        // 处理参数初始化
        if (!args.empty() && dynamic_cast<Dict*>(args[0])) {
            // 字典初始化语法：Student {name: "Bob", grade: 10}
            Dict* memberDict = dynamic_cast<Dict*>(args[0]);
            initializeClassFromDict(newInstance, classDef, memberDict);
        } else {
            // 参数初始化语法：Student("Bob", 10)
            initializeClassFromArgs(newInstance, classDef, args);
        }
        
        return newInstance;
    };
    classType->addMethod("new", newMethod, VIS_PUBLIC);
    LOG_DEBUG("Added new method to class '" + classDef->name + "'");
    
    // 添加assign方法，用于深拷贝赋值，本质是Object类型的赋值运算
    auto assignMethod = [](Value* instance, vector<Value*>& args) -> Value* {
        if (args.size() >= 1 && args[0]) {
            // 将Value转换为Object类型，然后完成Object类型的赋值
            if (Dict* currentInstance = dynamic_cast<Dict*>(instance)) {
                if (Dict* otherInstance = dynamic_cast<Dict*>(args[0])) {
                    // 调用Object类型的赋值运算符重载，进行深拷贝
                    *currentInstance = *otherInstance;
                    return currentInstance;
                }
            }
        }
        return nullptr;
    };
    classType->addMethod("assign", assignMethod, VIS_PUBLIC);
    LOG_DEBUG("Added assign method to class '" + classDef->name + "'");
    
    // 添加toString方法，用于字符串表示，本质是调用Value类型的toString方法
    auto toStringMethod = [](Value* instance, vector<Value*>& args) -> Value* {
        if (instance) {
            return new String(instance->toString());
        }
        return nullptr;
    };
    classType->addMethod("toString", toStringMethod, VIS_PUBLIC);
    LOG_DEBUG("Added toString method to class '" + classDef->name + "'");
    
    LOG_INFO("Successfully converted class '" + classDef->name + "' with " + 
             to_string(classDef->members.size()) + " members and " + 
             to_string(classDef->methods.size()) + " methods");
    
    return classType;
}

// ==================== 辅助方法实现 ====================

void TypeConverter::initializeStructFromDict(Dict* instance, StructDefinition* structDef, Dict* memberDict) {
    if (!instance || !structDef || !memberDict) {
        LOG_ERROR("TypeConverter::initializeStructFromDict: null parameter");
        return;
    }
    
    LOG_DEBUG("TypeConverter::initializeStructFromDict: initializing struct from dictionary");
    
    // 按照结构体定义的成员顺序初始化
    for (const auto& member : structDef->members) {
        Value* memberValue = memberDict->getEntry(member.name);
        if (memberValue) {
            // 如果提供了该成员的值，使用提供的值
            instance->setEntry(member.name, memberValue);
            LOG_DEBUG("TypeConverter::initializeStructFromDict: set member '" + member.name + "' from dict");
        } else {
            // 使用默认值
            Value* defaultVal = createMemberDefaultValue(member);
            instance->setEntry(member.name, defaultVal);
            LOG_DEBUG("TypeConverter::initializeStructFromDict: set member '" + member.name + "' to default value");
        }
    }
}

void TypeConverter::initializeStructFromArgs(Dict* instance, StructDefinition* structDef, const vector<Value*>& args) {
    if (!instance || !structDef) {
        LOG_ERROR("TypeConverter::initializeStructFromArgs: null parameter");
        return;
    }
    
    LOG_DEBUG("TypeConverter::initializeStructFromArgs: initializing struct from arguments");
    
    // 按参数顺序初始化成员
    for (size_t i = 0; i < structDef->members.size() && i < args.size(); ++i) {
        const auto& member = structDef->members[i];
        instance->setEntry(member.name, args[i]);
        LOG_DEBUG("TypeConverter::initializeStructFromArgs: set member '" + member.name + "' from arg[" + to_string(i) + "]");
    }
    
    // 为未初始化的成员设置默认值
    for (size_t i = args.size(); i < structDef->members.size(); ++i) {
        const auto& member = structDef->members[i];
        Value* defaultVal = createMemberDefaultValue(member);
        instance->setEntry(member.name, defaultVal);
        LOG_DEBUG("TypeConverter::initializeStructFromArgs: set member '" + member.name + "' to default value");
    }
}

void TypeConverter::initializeClassFromDict(Dict* instance, ClassDefinition* classDef, Dict* memberDict) {
    if (!instance || !classDef || !memberDict) {
        LOG_ERROR("TypeConverter::initializeClassFromDict: null parameter");
        return;
    }
    
    LOG_DEBUG("TypeConverter::initializeClassFromDict: initializing class from dictionary");
    
    // 按照类定义的成员顺序初始化
    for (const auto& member : classDef->members) {
        // 检查可见性（只初始化公有成员）
        if (member.visibility == "public") {
            Value* memberValue = memberDict->getEntry(member.name);
            if (memberValue) {
                // 如果提供了该成员的值，使用提供的值
                instance->setEntry(member.name, memberValue);
                LOG_DEBUG("TypeConverter::initializeClassFromDict: set member '" + member.name + "' from dict");
            } else {
                // 使用默认值
                Value* defaultVal = createMemberDefaultValue(member);
                instance->setEntry(member.name, defaultVal);
                LOG_DEBUG("TypeConverter::initializeClassFromDict: set member '" + member.name + "' to default value");
            }
        } else {
            LOG_DEBUG("TypeConverter::initializeClassFromDict: skipping private/protected member '" + member.name + "'");
        }
    }
}

void TypeConverter::initializeClassFromArgs(Dict* instance, ClassDefinition* classDef, const vector<Value*>& args) {
    if (!instance || !classDef) {
        LOG_ERROR("TypeConverter::initializeClassFromArgs: null parameter");
        return;
    }
    
    LOG_DEBUG("TypeConverter::initializeClassFromArgs: initializing class from arguments");
    
    // 按参数顺序初始化公有成员
    size_t argIndex = 0;
    for (const auto& member : classDef->members) {
        if (member.visibility == "public") {
            if (argIndex < args.size()) {
                instance->setEntry(member.name, args[argIndex]);
                LOG_DEBUG("TypeConverter::initializeClassFromArgs: set member '" + member.name + "' from arg[" + to_string(argIndex) + "]");
                argIndex++;
            } else {
                // 使用默认值
                Value* defaultVal = createMemberDefaultValue(member);
                instance->setEntry(member.name, defaultVal);
                LOG_DEBUG("TypeConverter::initializeClassFromArgs: set member '" + member.name + "' to default value");
            }
        } else {
            LOG_DEBUG("TypeConverter::initializeClassFromArgs: skipping private/protected member '" + member.name + "'");
        }
    }
}

Value* TypeConverter::createMemberDefaultValue(const StructMember& member) {
    // 当有初始值的时候，直接返回初始值
    if (member.defaultValue) {
        // 现在defaultValue是基本类型的ConstantExpression，需要根据类型获取值
        if (member.type) {
            switch (member.type->Tag) {
                case NUM:
                    if (ConstantExpression<int>* intExpr = dynamic_cast<ConstantExpression<int>*>(member.defaultValue)) {
                        return new Integer(intExpr->getValue());
                    }
                    break;
                case REAL:
                case DOUBLE:
                    if (ConstantExpression<double>* doubleExpr = dynamic_cast<ConstantExpression<double>*>(member.defaultValue)) {
                        return new Double(doubleExpr->getValue());
                    }
                    break;
                case CHAR:
                    if (ConstantExpression<char>* charExpr = dynamic_cast<ConstantExpression<char>*>(member.defaultValue)) {
                        return new Char(charExpr->getValue());
                    }
                    break;
                case BOOL:
                    if (ConstantExpression<bool>* boolExpr = dynamic_cast<ConstantExpression<bool>*>(member.defaultValue)) {
                        return new Bool(boolExpr->getValue());
                    }
                    break;
                case STR:
                    if (ConstantExpression<string>* strExpr = dynamic_cast<ConstantExpression<string>*>(member.defaultValue)) {
                        return new String(strExpr->getValue());
                    }
                    break;
                case ID:
                    // 用户定义的类型（结构体、类、数组、字典等）
                    if (member.type && member.type->word == "array") {
                        if (ConstantExpression<Array*>* arrayExpr = dynamic_cast<ConstantExpression<Array*>*>(member.defaultValue)) {
                            return arrayExpr->getValue();
                        }
                    } else if (member.type && member.type->word == "dict") {
                        if (ConstantExpression<Dict*>* dictExpr = dynamic_cast<ConstantExpression<Dict*>*>(member.defaultValue)) {
                            return dictExpr->getValue();
                        }
                    } else {
                        // 其他用户定义类型
                        if (ConstantExpression<Dict*>* dictExpr = dynamic_cast<ConstantExpression<Dict*>*>(member.defaultValue)) {
                            return dictExpr->getValue();
                        } else if (ConstantExpression<Array*>* arrayExpr = dynamic_cast<ConstantExpression<Array*>*>(member.defaultValue)) {
                            return arrayExpr->getValue();
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    
    // 否则使用类型系统创建默认值
    ObjectType* memberType = convertASTTypeToRuntimeType(member.type);
    if (memberType) {
        return ObjectFactory::createDefaultValueStatic(memberType);
    } else {
        return new Null();
    }
}

// 析构函数
TypeConverter::~TypeConverter() {
    LOG_DEBUG("TypeConverter destroyed");
}