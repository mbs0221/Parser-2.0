#ifndef INTERPRETER_OBJECT_FACTORY_H
#define INTERPRETER_OBJECT_FACTORY_H

#include <string>
#include <vector>
#include <map>
#include <functional>

// 前向声明
class Value;
class ObjectType;
class TypeRegistry;
class Interpreter;
class StructType;
class ClassType;
class Dict;

using namespace std;

// ==================== ObjectFactory类 ====================
// 用于创建指定Object子类型的Value对象的工厂类
class ObjectFactory {
private:
    bool initialized;
    TypeRegistry* typeRegistry;
    Interpreter* interpreter;
    
    // 初始化工厂映射
    void initializeCreators();

public:
    // 构造函数和析构函数
    ObjectFactory();
    ~ObjectFactory();
    
    // 根据ObjectType类型创建对应的Value对象
    Value* createValue(ObjectType* type);
    
    // 根据类型名称创建对应的Value对象
    Value* createValue(const string& typeName);
    
    // 根据ObjectType类型创建默认值
    Value* createDefaultValue(ObjectType* type);
    
    // 根据类型名称创建默认值
    Value* createDefaultValue(const string& typeName);
    
    // 创建null值
    Value* createNull();
    
    // 创建指定类型的null值（用于类型转换）
    Value* createNullForType(ObjectType* type);
    
    // 创建指定类型的null值（用于类型转换）
    Value* createNullForType(const string& typeName);
    
    // 检查类型是否支持null值
    bool supportsNull(ObjectType* type);
    
    // 检查类型名称是否支持null值
    bool supportsNull(const string& typeName);
    
    // 静态便捷方法（保持向后兼容）
    static Value* createValueStatic(ObjectType* type);
    static Value* createValueStatic(const string& typeName);
    static Value* createDefaultValueStatic(ObjectType* type);
    static Value* createDefaultValueStatic(const string& typeName);
    static Value* createNullStatic();
    static Value* createNullForTypeStatic(ObjectType* type);
    static Value* createNullForTypeStatic(const string& typeName);
    static bool supportsNullStatic(ObjectType* type);
    static bool supportsNullStatic(const string& typeName);
    
    // ==================== 结构体和类实例化方法 ====================
    
    // 结构体实例化
    Value* createStructInstance(StructType* structType, const vector<Value*>& args);
    Value* createStructInstance(const string& structTypeName, const vector<Value*>& args);
    
    // 类实例化
    Value* createClassInstance(ClassType* classType, const vector<Value*>& args);
    Value* createClassInstance(const string& classTypeName, const vector<Value*>& args);
    
    // 静态便捷方法实现
    static Value* createStructInstanceStatic(StructType* structType, const vector<Value*>& args);
    static Value* createStructInstanceStatic(const string& structTypeName, const vector<Value*>& args);
    static Value* createClassInstanceStatic(ClassType* classType, const vector<Value*>& args);
    static Value* createClassInstanceStatic(const string& classTypeName, const vector<Value*>& args);
    
    // ==================== 依赖注入实现 ====================
    
    void setTypeRegistry(TypeRegistry* registry);
    void setInterpreter(Interpreter* interp);
    
    // ==================== 通用实例化辅助方法实现 ====================
    
    void initializeFromDict(Dict* instance, ClassType* type, Dict* memberDict);
    void initializeFromArgs(Dict* instance, ClassType* type, const vector<Value*>& args);
    Value* createMemberDefaultValue(ClassType* type, const string& memberName);
    void initializeMethods(Dict* instance, ClassType* type);
};

// ==================== 全局工厂方法（向后兼容） ====================
// 全局工厂方法：根据ObjectType*类型创建默认值
Value* createDefaultValue(ObjectType* type);

// 全局工厂方法：根据类型名称创建默认值
Value* createDefaultValue(const string& typeName);

#endif // INTERPRETER_OBJECT_FACTORY_H 