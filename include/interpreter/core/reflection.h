#ifndef REFLECTION_H
#define REFLECTION_H

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <typeinfo>
#include <functional>

namespace InterpreterCore {

// 反射信息基类
class ReflectionInfo {
public:
    virtual ~ReflectionInfo() = default;
    
    // 获取名称
    virtual std::string getName() const = 0;
    
    // 获取类型
    virtual std::string getType() const = 0;
    
    // 获取描述
    virtual std::string getDescription() const = 0;
    
    // 获取字符串表示
    virtual std::string toString() const = 0;
};

// 类型反射信息
class TypeReflection : public ReflectionInfo {
public:
    TypeReflection(Type* type);
    ~TypeReflection();
    
    std::string getName() const override;
    std::string getType() const override { return "Type"; }
    std::string getDescription() const override;
    std::string toString() const override;
    
    // 类型特定方法
    Type* getTypeInfo() const { return type; }
    bool isPrimitive() const;
    bool isComposite() const;
    bool isGeneric() const;
    std::vector<TypeReflection*> getBaseTypes() const;
    std::vector<TypeReflection*> getInterfaces() const;

private:
    Type* type;
};

// 函数反射信息
class FunctionReflection : public ReflectionInfo {
public:
    FunctionReflection(Function* function);
    ~FunctionReflection();
    
    std::string getName() const override;
    std::string getType() const override { return "Function"; }
    std::string getDescription() const override;
    std::string toString() const override;
    
    // 函数特定方法
    Function* getFunctionInfo() const { return function; }
    std::vector<TypeReflection*> getParameterTypes() const;
    TypeReflection* getReturnType() const;
    bool isStatic() const;
    bool isVirtual() const;
    bool isGeneric() const;
    std::vector<FunctionReflection*> getOverloads() const;

private:
    Function* function;
};

// 变量反射信息
class VariableReflection : public ReflectionInfo {
public:
    VariableReflection(const std::string& name, Type* type, Value* value = nullptr);
    ~VariableReflection();
    
    std::string getName() const override { return name; }
    std::string getType() const override { return "Variable"; }
    std::string getDescription() const override;
    std::string toString() const override;
    
    // 变量特定方法
    Type* getVariableType() const { return type; }
    Value* getValue() const { return value; }
    void setValue(Value* newValue) { value = newValue; }
    bool isConstant() const;
    bool isStatic() const;
    std::string getVisibility() const;

private:
    std::string name;
    Type* type;
    Value* value;
    bool isConst;
    bool isStatic;
    std::string visibility;
};

// 类反射信息
class ClassReflection : public ReflectionInfo {
public:
    ClassReflection(ClassDefinition* classDef);
    ~ClassReflection();
    
    std::string getName() const override;
    std::string getType() const override { return "Class"; }
    std::string getDescription() const override;
    std::string toString() const override;
    
    // 类特定方法
    ClassDefinition* getClassInfo() const { return classDef; }
    std::vector<ClassReflection*> getBaseClasses() const;
    std::vector<ClassReflection*> getDerivedClasses() const;
    std::vector<FunctionReflection*> getMethods() const;
    std::vector<VariableReflection*> getFields() const;
    std::vector<FunctionReflection*> getConstructors() const;
    std::vector<FunctionReflection*> getDestructors() const;
    bool isAbstract() const;
    bool isFinal() const;
    std::string getVisibility() const;

private:
    ClassDefinition* classDef;
};

// 结构体反射信息
class StructReflection : public ReflectionInfo {
public:
    StructReflection(StructDefinition* structDef);
    ~StructReflection();
    
    std::string getName() const override;
    std::string getType() const override { return "Struct"; }
    std::string getDescription() const override;
    std::string toString() const override;
    
    // 结构体特定方法
    StructDefinition* getStructInfo() const { return structDef; }
    std::vector<VariableReflection*> getFields() const;
    std::vector<FunctionReflection*> getMethods() const;
    bool isValueType() const;
    std::string getVisibility() const;

private:
    StructDefinition* structDef;
};

// 模块反射信息
class ModuleReflection : public ReflectionInfo {
public:
    ModuleReflection(const std::string& name);
    ~ModuleReflection();
    
    std::string getName() const override { return name; }
    std::string getType() const override { return "Module"; }
    std::string getDescription() const override;
    std::string toString() const override;
    
    // 模块特定方法
    std::vector<TypeReflection*> getTypes() const;
    std::vector<FunctionReflection*> getFunctions() const;
    std::vector<ClassReflection*> getClasses() const;
    std::vector<StructReflection*> getStructs() const;
    std::vector<VariableReflection*> getVariables() const;
    std::vector<ModuleReflection*> getSubModules() const;
    void addType(TypeReflection* type);
    void addFunction(FunctionReflection* function);
    void addClass(ClassReflection* classRef);
    void addStruct(StructReflection* structRef);
    void addVariable(VariableReflection* variable);

private:
    std::string name;
    std::vector<TypeReflection*> types;
    std::vector<FunctionReflection*> functions;
    std::vector<ClassReflection*> classes;
    std::vector<StructReflection*> structs;
    std::vector<VariableReflection*> variables;
    std::vector<ModuleReflection*> subModules;
};

// 反射系统
class ReflectionSystem {
public:
    static ReflectionSystem& getInstance();
    
    // 注册反射信息
    void registerType(TypeReflection* type);
    void registerFunction(FunctionReflection* function);
    void registerClass(ClassReflection* classRef);
    void registerStruct(StructReflection* structRef);
    void registerVariable(VariableReflection* variable);
    void registerModule(ModuleReflection* module);
    
    // 查找反射信息
    TypeReflection* findType(const std::string& name);
    FunctionReflection* findFunction(const std::string& name);
    ClassReflection* findClass(const std::string& name);
    StructReflection* findStruct(const std::string& name);
    VariableReflection* findVariable(const std::string& name);
    ModuleReflection* findModule(const std::string& name);
    
    // 获取所有反射信息
    std::vector<TypeReflection*> getAllTypes() const;
    std::vector<FunctionReflection*> getAllFunctions() const;
    std::vector<ClassReflection*> getAllClasses() const;
    std::vector<StructReflection*> getAllStructs() const;
    std::vector<VariableReflection*> getAllVariables() const;
    std::vector<ModuleReflection*> getAllModules() const;
    
    // 反射操作
    Value* createInstance(const std::string& typeName, const std::vector<Value*>& args);
    Value* invokeMethod(Value* instance, const std::string& methodName, const std::vector<Value*>& args);
    Value* getField(Value* instance, const std::string& fieldName);
    void setField(Value* instance, const std::string& fieldName, Value* value);
    
    // 类型检查
    bool isInstanceOf(Value* value, const std::string& typeName);
    bool isSubtypeOf(const std::string& subtype, const std::string& supertype);
    
    // 序列化和反序列化
    std::string serialize(Value* value);
    Value* deserialize(const std::string& data, const std::string& typeName);

private:
    ReflectionSystem();
    ~ReflectionSystem();
    
    // 禁用拷贝构造和赋值
    ReflectionSystem(const ReflectionSystem&) = delete;
    ReflectionSystem& operator=(const ReflectionSystem&) = delete;
    
    std::map<std::string, TypeReflection*> types;
    std::map<std::string, FunctionReflection*> functions;
    std::map<std::string, ClassReflection*> classes;
    std::map<std::string, StructReflection*> structs;
    std::map<std::string, VariableReflection*> variables;
    std::map<std::string, ModuleReflection*> modules;
};

// 反射工具类
class ReflectionUtils {
public:
    // 类型转换
    static Value* castTo(Value* value, const std::string& targetType);
    static bool canCastTo(Value* value, const std::string& targetType);
    
    // 类型信息
    static std::string getTypeName(Value* value);
    static std::vector<std::string> getTypeHierarchy(const std::string& typeName);
    static std::vector<std::string> getTypeInterfaces(const std::string& typeName);
    
    // 方法信息
    static std::vector<std::string> getMethodNames(const std::string& typeName);
    static std::vector<std::string> getMethodSignatures(const std::string& typeName, const std::string& methodName);
    
    // 字段信息
    static std::vector<std::string> getFieldNames(const std::string& typeName);
    static std::string getFieldType(const std::string& typeName, const std::string& fieldName);
    
    // 注解支持
    static std::vector<std::string> getAnnotations(const std::string& elementName);
    static bool hasAnnotation(const std::string& elementName, const std::string& annotationName);
    static std::string getAnnotationValue(const std::string& elementName, const std::string& annotationName);
};

} // namespace InterpreterCore

#endif // REFLECTION_H
