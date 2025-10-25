#include "interpreter/core/reflection.h"
#include "common/logger.h"
#include <sstream>
#include <algorithm>

namespace InterpreterCore {

// ==================== TypeReflection 实现 ====================

TypeReflection::TypeReflection(Type* type) : type(type) {
}

TypeReflection::~TypeReflection() {
    // 注意：不删除type，因为它可能被其他地方使用
}

std::string TypeReflection::getName() const {
    return type ? type->toString() : "null";
}

std::string TypeReflection::getDescription() const {
    if (!type) {
        return "null type";
    }
    
    std::ostringstream oss;
    oss << "Type: " << type->toString();
    if (isPrimitive()) {
        oss << " (primitive)";
    } else if (isComposite()) {
        oss << " (composite)";
    }
    if (isGeneric()) {
        oss << " (generic)";
    }
    return oss.str();
}

std::string TypeReflection::toString() const {
    return getName();
}

bool TypeReflection::isPrimitive() const {
    if (!type) {
        return false;
    }
    
    std::string typeName = type->toString();
    return typeName == "int" || typeName == "double" || typeName == "bool" || 
           typeName == "string" || typeName == "char";
}

bool TypeReflection::isComposite() const {
    return !isPrimitive();
}

bool TypeReflection::isGeneric() const {
    if (!type) {
        return false;
    }
    
    std::string typeName = type->toString();
    return typeName.find('<') != std::string::npos;
}

std::vector<TypeReflection*> TypeReflection::getBaseTypes() const {
    // 这里需要根据实际的类型系统实现
    return {};
}

std::vector<TypeReflection*> TypeReflection::getInterfaces() const {
    // 这里需要根据实际的类型系统实现
    return {};
}

// ==================== FunctionReflection 实现 ====================

FunctionReflection::FunctionReflection(Function* function) : function(function) {
}

FunctionReflection::~FunctionReflection() {
    // 注意：不删除function，因为它可能被其他地方使用
}

std::string FunctionReflection::getName() const {
    return function ? function->getName() : "null";
}

std::string FunctionReflection::getDescription() const {
    if (!function) {
        return "null function";
    }
    
    std::ostringstream oss;
    oss << "Function: " << function->getName();
    if (isStatic()) {
        oss << " (static)";
    }
    if (isVirtual()) {
        oss << " (virtual)";
    }
    if (isGeneric()) {
        oss << " (generic)";
    }
    return oss.str();
}

std::string FunctionReflection::toString() const {
    return getName();
}

std::vector<TypeReflection*> FunctionReflection::getParameterTypes() const {
    std::vector<TypeReflection*> types;
    if (!function) {
        return types;
    }
    
    // 这里需要根据实际的Function实现来获取参数类型
    // 简化实现：返回空列表
    return types;
}

TypeReflection* FunctionReflection::getReturnType() const {
    if (!function) {
        return nullptr;
    }
    
    // 这里需要根据实际的Function实现来获取返回类型
    // 简化实现：返回nullptr
    return nullptr;
}

bool FunctionReflection::isStatic() const {
    // 这里需要根据实际的Function实现来判断
    return false;
}

bool FunctionReflection::isVirtual() const {
    // 这里需要根据实际的Function实现来判断
    return false;
}

bool FunctionReflection::isGeneric() const {
    // 这里需要根据实际的Function实现来判断
    return false;
}

std::vector<FunctionReflection*> FunctionReflection::getOverloads() const {
    // 这里需要根据实际的Function实现来获取重载
    return {};
}

// ==================== VariableReflection 实现 ====================

VariableReflection::VariableReflection(const std::string& name, Type* type, Value* value)
    : name(name), type(type), value(value), isConst(false), isStatic(false), visibility("public") {
}

VariableReflection::~VariableReflection() {
    // 注意：不删除type和value，因为它们可能被其他地方使用
}

std::string VariableReflection::getDescription() const {
    std::ostringstream oss;
    oss << "Variable: " << name;
    if (type) {
        oss << " : " << type->toString();
    }
    if (isConst) {
        oss << " (const)";
    }
    if (isStatic) {
        oss << " (static)";
    }
    oss << " (" << visibility << ")";
    return oss.str();
}

std::string VariableReflection::toString() const {
    return name;
}

bool VariableReflection::isConstant() const {
    return isConst;
}

bool VariableReflection::isStatic() const {
    return isStatic;
}

std::string VariableReflection::getVisibility() const {
    return visibility;
}

// ==================== ClassReflection 实现 ====================

ClassReflection::ClassReflection(ClassDefinition* classDef) : classDef(classDef) {
}

ClassReflection::~ClassReflection() {
    // 注意：不删除classDef，因为它可能被其他地方使用
}

std::string ClassReflection::getName() const {
    return classDef ? classDef->getName() : "null";
}

std::string ClassReflection::getDescription() const {
    if (!classDef) {
        return "null class";
    }
    
    std::ostringstream oss;
    oss << "Class: " << classDef->getName();
    if (isAbstract()) {
        oss << " (abstract)";
    }
    if (isFinal()) {
        oss << " (final)";
    }
    oss << " (" << getVisibility() << ")";
    return oss.str();
}

std::string ClassReflection::toString() const {
    return getName();
}

std::vector<ClassReflection*> ClassReflection::getBaseClasses() const {
    // 这里需要根据实际的ClassDefinition实现来获取基类
    return {};
}

std::vector<ClassReflection*> ClassReflection::getDerivedClasses() const {
    // 这里需要根据实际的ClassDefinition实现来获取派生类
    return {};
}

std::vector<FunctionReflection*> ClassReflection::getMethods() const {
    // 这里需要根据实际的ClassDefinition实现来获取方法
    return {};
}

std::vector<VariableReflection*> ClassReflection::getFields() const {
    // 这里需要根据实际的ClassDefinition实现来获取字段
    return {};
}

std::vector<FunctionReflection*> ClassReflection::getConstructors() const {
    // 这里需要根据实际的ClassDefinition实现来获取构造函数
    return {};
}

std::vector<FunctionReflection*> ClassReflection::getDestructors() const {
    // 这里需要根据实际的ClassDefinition实现来获取析构函数
    return {};
}

bool ClassReflection::isAbstract() const {
    // 这里需要根据实际的ClassDefinition实现来判断
    return false;
}

bool ClassReflection::isFinal() const {
    // 这里需要根据实际的ClassDefinition实现来判断
    return false;
}

std::string ClassReflection::getVisibility() const {
    // 这里需要根据实际的ClassDefinition实现来获取可见性
    return "public";
}

// ==================== StructReflection 实现 ====================

StructReflection::StructReflection(StructDefinition* structDef) : structDef(structDef) {
}

StructReflection::~StructReflection() {
    // 注意：不删除structDef，因为它可能被其他地方使用
}

std::string StructReflection::getName() const {
    return structDef ? structDef->getName() : "null";
}

std::string StructReflection::getDescription() const {
    if (!structDef) {
        return "null struct";
    }
    
    std::ostringstream oss;
    oss << "Struct: " << structDef->getName();
    if (isValueType()) {
        oss << " (value type)";
    }
    oss << " (" << getVisibility() << ")";
    return oss.str();
}

std::string StructReflection::toString() const {
    return getName();
}

std::vector<VariableReflection*> StructReflection::getFields() const {
    // 这里需要根据实际的StructDefinition实现来获取字段
    return {};
}

std::vector<FunctionReflection*> StructReflection::getMethods() const {
    // 这里需要根据实际的StructDefinition实现来获取方法
    return {};
}

bool StructReflection::isValueType() const {
    // 结构体通常是值类型
    return true;
}

std::string StructReflection::getVisibility() const {
    // 这里需要根据实际的StructDefinition实现来获取可见性
    return "public";
}

// ==================== ModuleReflection 实现 ====================

ModuleReflection::ModuleReflection(const std::string& name) : name(name) {
}

ModuleReflection::~ModuleReflection() {
    // 注意：不删除容器中的对象，因为它们可能被其他地方使用
}

std::string ModuleReflection::getDescription() const {
    std::ostringstream oss;
    oss << "Module: " << name;
    oss << " (types: " << types.size();
    oss << ", functions: " << functions.size();
    oss << ", classes: " << classes.size();
    oss << ", structs: " << structs.size();
    oss << ", variables: " << variables.size();
    oss << ")";
    return oss.str();
}

std::string ModuleReflection::toString() const {
    return name;
}

std::vector<TypeReflection*> ModuleReflection::getTypes() const {
    return types;
}

std::vector<FunctionReflection*> ModuleReflection::getFunctions() const {
    return functions;
}

std::vector<ClassReflection*> ModuleReflection::getClasses() const {
    return classes;
}

std::vector<StructReflection*> ModuleReflection::getStructs() const {
    return structs;
}

std::vector<VariableReflection*> ModuleReflection::getVariables() const {
    return variables;
}

std::vector<ModuleReflection*> ModuleReflection::getSubModules() const {
    return subModules;
}

void ModuleReflection::addType(TypeReflection* type) {
    if (type) {
        types.push_back(type);
    }
}

void ModuleReflection::addFunction(FunctionReflection* function) {
    if (function) {
        functions.push_back(function);
    }
}

void ModuleReflection::addClass(ClassReflection* classRef) {
    if (classRef) {
        classes.push_back(classRef);
    }
}

void ModuleReflection::addStruct(StructReflection* structRef) {
    if (structRef) {
        structs.push_back(structRef);
    }
}

void ModuleReflection::addVariable(VariableReflection* variable) {
    if (variable) {
        variables.push_back(variable);
    }
}

// ==================== ReflectionSystem 实现 ====================

ReflectionSystem& ReflectionSystem::getInstance() {
    static ReflectionSystem instance;
    return instance;
}

ReflectionSystem::ReflectionSystem() {
}

ReflectionSystem::~ReflectionSystem() {
    // 清理注册的反射信息
    for (auto& pair : types) {
        delete pair.second;
    }
    for (auto& pair : functions) {
        delete pair.second;
    }
    for (auto& pair : classes) {
        delete pair.second;
    }
    for (auto& pair : structs) {
        delete pair.second;
    }
    for (auto& pair : variables) {
        delete pair.second;
    }
    for (auto& pair : modules) {
        delete pair.second;
    }
}

void ReflectionSystem::registerType(TypeReflection* type) {
    if (type) {
        types[type->getName()] = type;
        LOG_DEBUG("注册类型反射信息: " + type->getName());
    }
}

void ReflectionSystem::registerFunction(FunctionReflection* function) {
    if (function) {
        functions[function->getName()] = function;
        LOG_DEBUG("注册函数反射信息: " + function->getName());
    }
}

void ReflectionSystem::registerClass(ClassReflection* classRef) {
    if (classRef) {
        classes[classRef->getName()] = classRef;
        LOG_DEBUG("注册类反射信息: " + classRef->getName());
    }
}

void ReflectionSystem::registerStruct(StructReflection* structRef) {
    if (structRef) {
        structs[structRef->getName()] = structRef;
        LOG_DEBUG("注册结构体反射信息: " + structRef->getName());
    }
}

void ReflectionSystem::registerVariable(VariableReflection* variable) {
    if (variable) {
        variables[variable->getName()] = variable;
        LOG_DEBUG("注册变量反射信息: " + variable->getName());
    }
}

void ReflectionSystem::registerModule(ModuleReflection* module) {
    if (module) {
        modules[module->getName()] = module;
        LOG_DEBUG("注册模块反射信息: " + module->getName());
    }
}

TypeReflection* ReflectionSystem::findType(const std::string& name) {
    auto it = types.find(name);
    return (it != types.end()) ? it->second : nullptr;
}

FunctionReflection* ReflectionSystem::findFunction(const std::string& name) {
    auto it = functions.find(name);
    return (it != functions.end()) ? it->second : nullptr;
}

ClassReflection* ReflectionSystem::findClass(const std::string& name) {
    auto it = classes.find(name);
    return (it != classes.end()) ? it->second : nullptr;
}

StructReflection* ReflectionSystem::findStruct(const std::string& name) {
    auto it = structs.find(name);
    return (it != structs.end()) ? it->second : nullptr;
}

VariableReflection* ReflectionSystem::findVariable(const std::string& name) {
    auto it = variables.find(name);
    return (it != variables.end()) ? it->second : nullptr;
}

ModuleReflection* ReflectionSystem::findModule(const std::string& name) {
    auto it = modules.find(name);
    return (it != modules.end()) ? it->second : nullptr;
}

std::vector<TypeReflection*> ReflectionSystem::getAllTypes() const {
    std::vector<TypeReflection*> result;
    for (const auto& pair : types) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<FunctionReflection*> ReflectionSystem::getAllFunctions() const {
    std::vector<FunctionReflection*> result;
    for (const auto& pair : functions) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<ClassReflection*> ReflectionSystem::getAllClasses() const {
    std::vector<ClassReflection*> result;
    for (const auto& pair : classes) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<StructReflection*> ReflectionSystem::getAllStructs() const {
    std::vector<StructReflection*> result;
    for (const auto& pair : structs) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<VariableReflection*> ReflectionSystem::getAllVariables() const {
    std::vector<VariableReflection*> result;
    for (const auto& pair : variables) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<ModuleReflection*> ReflectionSystem::getAllModules() const {
    std::vector<ModuleReflection*> result;
    for (const auto& pair : modules) {
        result.push_back(pair.second);
    }
    return result;
}

Value* ReflectionSystem::createInstance(const std::string& typeName, const std::vector<Value*>& args) {
    // 这里需要根据实际的类型系统实现来创建实例
    // 简化实现：返回nullptr
    return nullptr;
}

Value* ReflectionSystem::invokeMethod(Value* instance, const std::string& methodName, const std::vector<Value*>& args) {
    // 这里需要根据实际的方法调用系统实现
    // 简化实现：返回nullptr
    return nullptr;
}

Value* ReflectionSystem::getField(Value* instance, const std::string& fieldName) {
    // 这里需要根据实际的字段访问系统实现
    // 简化实现：返回nullptr
    return nullptr;
}

void ReflectionSystem::setField(Value* instance, const std::string& fieldName, Value* value) {
    // 这里需要根据实际的字段设置系统实现
}

bool ReflectionSystem::isInstanceOf(Value* value, const std::string& typeName) {
    if (!value) {
        return false;
    }
    
    return value->getType()->toString() == typeName;
}

bool ReflectionSystem::isSubtypeOf(const std::string& subtype, const std::string& supertype) {
    TypeReflection* subTypeRef = findType(subtype);
    TypeReflection* superTypeRef = findType(supertype);
    
    if (!subTypeRef || !superTypeRef) {
        return false;
    }
    
    // 这里需要根据实际的类型系统实现来判断子类型关系
    return false;
}

std::string ReflectionSystem::serialize(Value* value) {
    if (!value) {
        return "null";
    }
    
    // 简化的序列化实现
    return value->toString();
}

Value* ReflectionSystem::deserialize(const std::string& data, const std::string& typeName) {
    // 简化的反序列化实现
    return nullptr;
}

// ==================== ReflectionUtils 实现 ====================

Value* ReflectionUtils::castTo(Value* value, const std::string& targetType) {
    if (!value) {
        return nullptr;
    }
    
    // 这里需要根据实际的类型转换系统实现
    return value;
}

bool ReflectionUtils::canCastTo(Value* value, const std::string& targetType) {
    if (!value) {
        return false;
    }
    
    // 简化的类型转换检查
    return value->getType()->toString() == targetType;
}

std::string ReflectionUtils::getTypeName(Value* value) {
    return value ? value->getType()->toString() : "null";
}

std::vector<std::string> ReflectionUtils::getTypeHierarchy(const std::string& typeName) {
    // 这里需要根据实际的类型系统实现来获取类型层次结构
    return {typeName};
}

std::vector<std::string> ReflectionUtils::getTypeInterfaces(const std::string& typeName) {
    // 这里需要根据实际的类型系统实现来获取接口
    return {};
}

std::vector<std::string> ReflectionUtils::getMethodNames(const std::string& typeName) {
    // 这里需要根据实际的类型系统实现来获取方法名
    return {};
}

std::vector<std::string> ReflectionUtils::getMethodSignatures(const std::string& typeName, const std::string& methodName) {
    // 这里需要根据实际的类型系统实现来获取方法签名
    return {};
}

std::vector<std::string> ReflectionUtils::getFieldNames(const std::string& typeName) {
    // 这里需要根据实际的类型系统实现来获取字段名
    return {};
}

std::string ReflectionUtils::getFieldType(const std::string& typeName, const std::string& fieldName) {
    // 这里需要根据实际的类型系统实现来获取字段类型
    return "unknown";
}

std::vector<std::string> ReflectionUtils::getAnnotations(const std::string& elementName) {
    // 这里需要根据实际的注解系统实现
    return {};
}

bool ReflectionUtils::hasAnnotation(const std::string& elementName, const std::string& annotationName) {
    // 这里需要根据实际的注解系统实现
    return false;
}

std::string ReflectionUtils::getAnnotationValue(const std::string& elementName, const std::string& annotationName) {
    // 这里需要根据实际的注解系统实现
    return "";
}

} // namespace InterpreterCore
