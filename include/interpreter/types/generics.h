#ifndef GENERICS_H
#define GENERICS_H

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <typeinfo>

namespace InterpreterTypes {

// 泛型参数
class GenericParameter {
public:
    GenericParameter(const std::string& name, Type* constraint = nullptr);
    ~GenericParameter();
    
    // 获取参数名称
    const std::string& getName() const { return name; }
    
    // 获取类型约束
    Type* getConstraint() const { return constraint; }
    
    // 设置类型约束
    void setConstraint(Type* constraint) { this->constraint = constraint; }
    
    // 检查类型是否满足约束
    bool checkConstraint(Type* type) const;
    
    // 获取字符串表示
    std::string toString() const;

private:
    std::string name;
    Type* constraint;
};

// 泛型类型实例
class GenericTypeInstance {
public:
    GenericTypeInstance(Type* baseType, const std::map<std::string, Type*>& typeArguments);
    ~GenericTypeInstance();
    
    // 获取基础类型
    Type* getBaseType() const { return baseType; }
    
    // 获取类型参数映射
    const std::map<std::string, Type*>& getTypeArguments() const { return typeArguments; }
    
    // 获取特定类型参数
    Type* getTypeArgument(const std::string& name) const;
    
    // 检查类型参数是否完整
    bool isComplete() const;
    
    // 获取字符串表示
    std::string toString() const;
    
    // 比较两个泛型类型实例
    bool equals(const GenericTypeInstance* other) const;

private:
    Type* baseType;
    std::map<std::string, Type*> typeArguments;
};

// 泛型函数签名
class GenericFunctionSignature {
public:
    GenericFunctionSignature(const std::string& name, 
                           const std::vector<GenericParameter*>& genericParams,
                           const std::vector<Type*>& paramTypes,
                           Type* returnType);
    ~GenericFunctionSignature();
    
    // 获取函数名称
    const std::string& getName() const { return name; }
    
    // 获取泛型参数列表
    const std::vector<GenericParameter*>& getGenericParameters() const { return genericParams; }
    
    // 获取参数类型列表
    const std::vector<Type*>& getParameterTypes() const { return paramTypes; }
    
    // 获取返回类型
    Type* getReturnType() const { return returnType; }
    
    // 实例化泛型函数
    FunctionSignature* instantiate(const std::vector<Type*>& typeArguments);
    
    // 检查类型参数是否匹配
    bool checkTypeArguments(const std::vector<Type*>& typeArguments) const;
    
    // 获取字符串表示
    std::string toString() const;

private:
    std::string name;
    std::vector<GenericParameter*> genericParams;
    std::vector<Type*> paramTypes;
    Type* returnType;
};

// 泛型类型定义
class GenericTypeDefinition {
public:
    GenericTypeDefinition(const std::string& name, 
                         const std::vector<GenericParameter*>& genericParams);
    ~GenericTypeDefinition();
    
    // 获取类型名称
    const std::string& getName() const { return name; }
    
    // 获取泛型参数列表
    const std::vector<GenericParameter*>& getGenericParameters() const { return genericParams; }
    
    // 实例化泛型类型
    GenericTypeInstance* instantiate(const std::vector<Type*>& typeArguments);
    
    // 检查类型参数是否匹配
    bool checkTypeArguments(const std::vector<Type*>& typeArguments) const;
    
    // 获取字符串表示
    std::string toString() const;

private:
    std::string name;
    std::vector<GenericParameter*> genericParams;
};

// 泛型类型推断器
class GenericTypeInferencer {
public:
    GenericTypeInferencer();
    ~GenericTypeInferencer();
    
    // 推断泛型类型参数
    std::vector<Type*> inferTypeArguments(GenericFunctionSignature* signature, 
                                        const std::vector<Type*>& argumentTypes);
    
    // 推断泛型类型实例
    GenericTypeInstance* inferGenericType(GenericTypeDefinition* definition,
                                        const std::vector<Type*>& argumentTypes);
    
    // 检查类型兼容性
    bool checkTypeCompatibility(Type* expected, Type* actual);
    
    // 解析泛型类型表达式
    Type* resolveGenericType(const std::string& typeExpression,
                           const std::map<std::string, Type*>& context);

private:
    // 类型推断辅助方法
    std::map<std::string, Type*> unifyTypes(const std::vector<Type*>& expected,
                                          const std::vector<Type*>& actual);
    
    bool isGenericType(Type* type) const;
    bool isConcreteType(Type* type) const;
};

// 泛型类型注册表
class GenericTypeRegistry {
public:
    static GenericTypeRegistry& getInstance();
    
    // 注册泛型类型定义
    void registerGenericType(GenericTypeDefinition* definition);
    
    // 注册泛型函数签名
    void registerGenericFunction(GenericFunctionSignature* signature);
    
    // 查找泛型类型定义
    GenericTypeDefinition* findGenericType(const std::string& name);
    
    // 查找泛型函数签名
    GenericFunctionSignature* findGenericFunction(const std::string& name);
    
    // 创建泛型类型实例
    GenericTypeInstance* createGenericTypeInstance(const std::string& name,
                                                  const std::vector<Type*>& typeArguments);
    
    // 创建泛型函数实例
    FunctionSignature* createGenericFunctionInstance(const std::string& name,
                                                    const std::vector<Type*>& typeArguments);
    
    // 获取所有注册的泛型类型
    std::vector<GenericTypeDefinition*> getAllGenericTypes() const;
    
    // 获取所有注册的泛型函数
    std::vector<GenericFunctionSignature*> getAllGenericFunctions() const;

private:
    GenericTypeRegistry();
    ~GenericTypeRegistry();
    
    // 禁用拷贝构造和赋值
    GenericTypeRegistry(const GenericTypeRegistry&) = delete;
    GenericTypeRegistry& operator=(const GenericTypeRegistry&) = delete;
    
    std::map<std::string, GenericTypeDefinition*> genericTypes;
    std::map<std::string, GenericFunctionSignature*> genericFunctions;
    GenericTypeInferencer* typeInferencer;
};

// 泛型类型约束
class TypeConstraint {
public:
    enum ConstraintType {
        EQUALS,           // T == U
        SUBTYPE,          // T <: U
        SUPERTYPE,        // T >: U
        CONSTRUCTOR,      // T has constructor
        METHOD,           // T has method
        PROPERTY          // T has property
    };
    
    TypeConstraint(ConstraintType type, const std::string& description);
    ~TypeConstraint();
    
    // 获取约束类型
    ConstraintType getType() const { return type; }
    
    // 获取约束描述
    const std::string& getDescription() const { return description; }
    
    // 检查类型是否满足约束
    bool checkConstraint(Type* type) const;
    
    // 获取字符串表示
    std::string toString() const;

private:
    ConstraintType type;
    std::string description;
};

// 泛型类型约束检查器
class TypeConstraintChecker {
public:
    TypeConstraintChecker();
    ~TypeConstraintChecker();
    
    // 检查类型约束
    bool checkConstraints(const std::vector<TypeConstraint*>& constraints, Type* type);
    
    // 添加内置约束
    void addBuiltinConstraints();
    
    // 注册自定义约束
    void registerConstraint(const std::string& name, TypeConstraint* constraint);

private:
    std::map<std::string, TypeConstraint*> constraints;
};

} // namespace InterpreterTypes

#endif // GENERICS_H
