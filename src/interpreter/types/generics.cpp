#include "interpreter/types/generics.h"
#include "common/logger.h"
#include <algorithm>
#include <sstream>

namespace InterpreterTypes {

// ==================== GenericParameter 实现 ====================

GenericParameter::GenericParameter(const std::string& name, Type* constraint)
    : name(name), constraint(constraint) {
}

GenericParameter::~GenericParameter() {
    // 注意：不删除constraint，因为它可能被其他地方使用
}

bool GenericParameter::checkConstraint(Type* type) const {
    if (!constraint) {
        return true; // 没有约束，任何类型都可以
    }
    
    // 简单的类型兼容性检查
    return type->equals(constraint) || type->isSubtypeOf(constraint);
}

std::string GenericParameter::toString() const {
    std::string result = name;
    if (constraint) {
        result += " : " + constraint->toString();
    }
    return result;
}

// ==================== GenericTypeInstance 实现 ====================

GenericTypeInstance::GenericTypeInstance(Type* baseType, const std::map<std::string, Type*>& typeArguments)
    : baseType(baseType), typeArguments(typeArguments) {
}

GenericTypeInstance::~GenericTypeInstance() {
    // 注意：不删除typeArguments中的Type*，因为它们可能被其他地方使用
}

Type* GenericTypeInstance::getTypeArgument(const std::string& name) const {
    auto it = typeArguments.find(name);
    return (it != typeArguments.end()) ? it->second : nullptr;
}

bool GenericTypeInstance::isComplete() const {
    // 检查所有类型参数是否都已提供
    return !typeArguments.empty();
}

std::string GenericTypeInstance::toString() const {
    std::ostringstream oss;
    oss << baseType->toString() << "<";
    
    bool first = true;
    for (const auto& pair : typeArguments) {
        if (!first) {
            oss << ", ";
        }
        oss << pair.first << "=" << pair.second->toString();
        first = false;
    }
    
    oss << ">";
    return oss.str();
}

bool GenericTypeInstance::equals(const GenericTypeInstance* other) const {
    if (!other) {
        return false;
    }
    
    if (!baseType->equals(other->baseType)) {
        return false;
    }
    
    if (typeArguments.size() != other->typeArguments.size()) {
        return false;
    }
    
    for (const auto& pair : typeArguments) {
        auto it = other->typeArguments.find(pair.first);
        if (it == other->typeArguments.end() || !pair.second->equals(it->second)) {
            return false;
        }
    }
    
    return true;
}

// ==================== GenericFunctionSignature 实现 ====================

GenericFunctionSignature::GenericFunctionSignature(const std::string& name,
                                                 const std::vector<GenericParameter*>& genericParams,
                                                 const std::vector<Type*>& paramTypes,
                                                 Type* returnType)
    : name(name), genericParams(genericParams), paramTypes(paramTypes), returnType(returnType) {
}

GenericFunctionSignature::~GenericFunctionSignature() {
    // 注意：不删除genericParams、paramTypes和returnType中的Type*，因为它们可能被其他地方使用
}

FunctionSignature* GenericFunctionSignature::instantiate(const std::vector<Type*>& typeArguments) {
    if (!checkTypeArguments(typeArguments)) {
        LOG_ERROR("泛型函数实例化失败：类型参数不匹配");
        return nullptr;
    }
    
    // 创建类型参数映射
    std::map<std::string, Type*> typeMap;
    for (size_t i = 0; i < genericParams.size() && i < typeArguments.size(); ++i) {
        typeMap[genericParams[i]->getName()] = typeArguments[i];
    }
    
    // 实例化参数类型
    std::vector<Type*> instantiatedParamTypes;
    for (Type* paramType : paramTypes) {
        // 这里需要根据类型参数映射替换泛型类型
        // 简化实现：直接使用原类型
        instantiatedParamTypes.push_back(paramType);
    }
    
    // 实例化返回类型
    Type* instantiatedReturnType = returnType; // 简化实现
    
    return new FunctionSignature(name, instantiatedParamTypes, instantiatedReturnType);
}

bool GenericFunctionSignature::checkTypeArguments(const std::vector<Type*>& typeArguments) const {
    if (typeArguments.size() != genericParams.size()) {
        return false;
    }
    
    for (size_t i = 0; i < genericParams.size(); ++i) {
        if (!genericParams[i]->checkConstraint(typeArguments[i])) {
            return false;
        }
    }
    
    return true;
}

std::string GenericFunctionSignature::toString() const {
    std::ostringstream oss;
    oss << name << "<";
    
    // 泛型参数
    for (size_t i = 0; i < genericParams.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << genericParams[i]->toString();
    }
    
    oss << ">(";
    
    // 参数类型
    for (size_t i = 0; i < paramTypes.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << paramTypes[i]->toString();
    }
    
    oss << ") -> " << returnType->toString();
    return oss.str();
}

// ==================== GenericTypeDefinition 实现 ====================

GenericTypeDefinition::GenericTypeDefinition(const std::string& name,
                                           const std::vector<GenericParameter*>& genericParams)
    : name(name), genericParams(genericParams) {
}

GenericTypeDefinition::~GenericTypeDefinition() {
    // 注意：不删除genericParams中的GenericParameter*，因为它们可能被其他地方使用
}

GenericTypeInstance* GenericTypeDefinition::instantiate(const std::vector<Type*>& typeArguments) {
    if (!checkTypeArguments(typeArguments)) {
        LOG_ERROR("泛型类型实例化失败：类型参数不匹配");
        return nullptr;
    }
    
    // 创建类型参数映射
    std::map<std::string, Type*> typeMap;
    for (size_t i = 0; i < genericParams.size() && i < typeArguments.size(); ++i) {
        typeMap[genericParams[i]->getName()] = typeArguments[i];
    }
    
    // 创建基础类型（这里需要根据实际的类型系统实现）
    Type* baseType = new Type(name, 0); // 简化实现
    
    return new GenericTypeInstance(baseType, typeMap);
}

bool GenericTypeDefinition::checkTypeArguments(const std::vector<Type*>& typeArguments) const {
    if (typeArguments.size() != genericParams.size()) {
        return false;
    }
    
    for (size_t i = 0; i < genericParams.size(); ++i) {
        if (!genericParams[i]->checkConstraint(typeArguments[i])) {
            return false;
        }
    }
    
    return true;
}

std::string GenericTypeDefinition::toString() const {
    std::ostringstream oss;
    oss << name << "<";
    
    for (size_t i = 0; i < genericParams.size(); ++i) {
        if (i > 0) {
            oss << ", ";
        }
        oss << genericParams[i]->toString();
    }
    
    oss << ">";
    return oss.str();
}

// ==================== GenericTypeInferencer 实现 ====================

GenericTypeInferencer::GenericTypeInferencer() {
}

GenericTypeInferencer::~GenericTypeInferencer() {
}

std::vector<Type*> GenericTypeInferencer::inferTypeArguments(GenericFunctionSignature* signature,
                                                           const std::vector<Type*>& argumentTypes) {
    std::vector<Type*> inferredTypes;
    
    if (!signature || argumentTypes.size() != signature->getParameterTypes().size()) {
        return inferredTypes;
    }
    
    // 简化的类型推断：直接使用参数类型
    // 实际实现需要更复杂的类型统一算法
    for (size_t i = 0; i < signature->getGenericParameters().size(); ++i) {
        if (i < argumentTypes.size()) {
            inferredTypes.push_back(argumentTypes[i]);
        }
    }
    
    return inferredTypes;
}

GenericTypeInstance* GenericTypeInferencer::inferGenericType(GenericTypeDefinition* definition,
                                                          const std::vector<Type*>& argumentTypes) {
    if (!definition || !definition->checkTypeArguments(argumentTypes)) {
        return nullptr;
    }
    
    return definition->instantiate(argumentTypes);
}

bool GenericTypeInferencer::checkTypeCompatibility(Type* expected, Type* actual) {
    if (!expected || !actual) {
        return false;
    }
    
    return expected->equals(actual) || actual->isSubtypeOf(expected);
}

Type* GenericTypeInferencer::resolveGenericType(const std::string& typeExpression,
                                               const std::map<std::string, Type*>& context) {
    // 简化的泛型类型解析
    auto it = context.find(typeExpression);
    if (it != context.end()) {
        return it->second;
    }
    
    // 如果没有找到，返回一个基础类型
    return new Type(typeExpression, 0);
}

std::map<std::string, Type*> GenericTypeInferencer::unifyTypes(const std::vector<Type*>& expected,
                                                            const std::vector<Type*>& actual) {
    std::map<std::string, Type*> unified;
    
    if (expected.size() != actual.size()) {
        return unified;
    }
    
    for (size_t i = 0; i < expected.size(); ++i) {
        if (expected[i]->equals(actual[i])) {
            // 类型匹配，可以统一
            unified["T" + std::to_string(i)] = actual[i];
        }
    }
    
    return unified;
}

bool GenericTypeInferencer::isGenericType(Type* type) const {
    // 简化的泛型类型检查
    return type && type->toString().find('<') != std::string::npos;
}

bool GenericTypeInferencer::isConcreteType(Type* type) const {
    return type && !isGenericType(type);
}

// ==================== GenericTypeRegistry 实现 ====================

GenericTypeRegistry& GenericTypeRegistry::getInstance() {
    static GenericTypeRegistry instance;
    return instance;
}

GenericTypeRegistry::GenericTypeRegistry() {
    typeInferencer = new GenericTypeInferencer();
}

GenericTypeRegistry::~GenericTypeRegistry() {
    delete typeInferencer;
    
    // 清理注册的泛型类型和函数
    for (auto& pair : genericTypes) {
        delete pair.second;
    }
    for (auto& pair : genericFunctions) {
        delete pair.second;
    }
}

void GenericTypeRegistry::registerGenericType(GenericTypeDefinition* definition) {
    if (definition) {
        genericTypes[definition->getName()] = definition;
        LOG_DEBUG("注册泛型类型: " + definition->getName());
    }
}

void GenericTypeRegistry::registerGenericFunction(GenericFunctionSignature* signature) {
    if (signature) {
        genericFunctions[signature->getName()] = signature;
        LOG_DEBUG("注册泛型函数: " + signature->getName());
    }
}

GenericTypeDefinition* GenericTypeRegistry::findGenericType(const std::string& name) {
    auto it = genericTypes.find(name);
    return (it != genericTypes.end()) ? it->second : nullptr;
}

GenericFunctionSignature* GenericTypeRegistry::findGenericFunction(const std::string& name) {
    auto it = genericFunctions.find(name);
    return (it != genericFunctions.end()) ? it->second : nullptr;
}

GenericTypeInstance* GenericTypeRegistry::createGenericTypeInstance(const std::string& name,
                                                                  const std::vector<Type*>& typeArguments) {
    GenericTypeDefinition* definition = findGenericType(name);
    if (!definition) {
        LOG_ERROR("未找到泛型类型定义: " + name);
        return nullptr;
    }
    
    return typeInferencer->inferGenericType(definition, typeArguments);
}

FunctionSignature* GenericTypeRegistry::createGenericFunctionInstance(const std::string& name,
                                                                    const std::vector<Type*>& typeArguments) {
    GenericFunctionSignature* signature = findGenericFunction(name);
    if (!signature) {
        LOG_ERROR("未找到泛型函数签名: " + name);
        return nullptr;
    }
    
    return signature->instantiate(typeArguments);
}

std::vector<GenericTypeDefinition*> GenericTypeRegistry::getAllGenericTypes() const {
    std::vector<GenericTypeDefinition*> types;
    for (const auto& pair : genericTypes) {
        types.push_back(pair.second);
    }
    return types;
}

std::vector<GenericFunctionSignature*> GenericTypeRegistry::getAllGenericFunctions() const {
    std::vector<GenericFunctionSignature*> functions;
    for (const auto& pair : genericFunctions) {
        functions.push_back(pair.second);
    }
    return functions;
}

// ==================== TypeConstraint 实现 ====================

TypeConstraint::TypeConstraint(ConstraintType type, const std::string& description)
    : type(type), description(description) {
}

TypeConstraint::~TypeConstraint() {
}

bool TypeConstraint::checkConstraint(Type* type) const {
    if (!type) {
        return false;
    }
    
    // 简化的约束检查
    switch (type) {
        case EQUALS:
            return type->toString() == description;
        case SUBTYPE:
            return type->isSubtypeOf(new Type(description, 0));
        case SUPERTYPE:
            return (new Type(description, 0))->isSubtypeOf(type);
        default:
            return true; // 其他约束类型暂时返回true
    }
}

std::string TypeConstraint::toString() const {
    std::string typeStr;
    switch (type) {
        case EQUALS: typeStr = "=="; break;
        case SUBTYPE: typeStr = "<:"; break;
        case SUPERTYPE: typeStr = ">:"; break;
        case CONSTRUCTOR: typeStr = "constructor"; break;
        case METHOD: typeStr = "method"; break;
        case PROPERTY: typeStr = "property"; break;
    }
    return typeStr + " " + description;
}

// ==================== TypeConstraintChecker 实现 ====================

TypeConstraintChecker::TypeConstraintChecker() {
    addBuiltinConstraints();
}

TypeConstraintChecker::~TypeConstraintChecker() {
    for (auto& pair : constraints) {
        delete pair.second;
    }
}

bool TypeConstraintChecker::checkConstraints(const std::vector<TypeConstraint*>& constraints, Type* type) {
    for (TypeConstraint* constraint : constraints) {
        if (!constraint->checkConstraint(type)) {
            return false;
        }
    }
    return true;
}

void TypeConstraintChecker::addBuiltinConstraints() {
    // 添加内置约束
    constraints["Comparable"] = new TypeConstraint(TypeConstraint::METHOD, "compare");
    constraints["Serializable"] = new TypeConstraint(TypeConstraint::METHOD, "serialize");
    constraints["Cloneable"] = new TypeConstraint(TypeConstraint::METHOD, "clone");
}

void TypeConstraintChecker::registerConstraint(const std::string& name, TypeConstraint* constraint) {
    constraints[name] = constraint;
}

} // namespace InterpreterTypes
