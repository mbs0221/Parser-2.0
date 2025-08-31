#ifndef TYPE_CONVERTER_H
#define TYPE_CONVERTER_H

#include "interpreter/type_registry.h"
#include "parser/function.h"
#include <string>
#include <vector>
#include <mutex>

// 前向声明
class Interpreter;
class StructDefinition;
class ClassDefinition;
class Type;

// 类型转换器 - 负责将AST类型定义转换为运行时类型
class TypeConverter {
private:
    TypeRegistry* typeRegistry;
    Interpreter* interpreter;
    mutable std::mutex converterMutex; // 线程安全锁
    
    // 辅助方法
    ObjectType* convertASTTypeToRuntimeType(lexer::Type* astType);

public:
    // 构造函数
    TypeConverter(TypeRegistry* registry = nullptr, Interpreter* interp = nullptr);
    
    // 依赖注入
    void setTypeRegistry(TypeRegistry* registry);
    void setInterpreter(Interpreter* interp);
    
    // 获取器方法
    TypeRegistry* getTypeRegistry() const { return typeRegistry; }
    Interpreter* getInterpreter() const { return interpreter; }
    
    // 类型转换接口（线程安全）
    StructType* convertStructDefinition(StructDefinition* structDef, const vector<pair<string, Value*>>& memberInitialValues);
    ClassType* convertClassDefinition(ClassDefinition* classDef, const vector<pair<string, Value*>>& memberInitialValues);
    
    // 辅助方法
    void initializeStructFromDict(Dict* instance, StructDefinition* structDef, Dict* memberDict);
    void initializeClassFromDict(Dict* instance, ClassDefinition* classDef, Dict* memberDict);
    void initializeClassFromArgs(Dict* instance, ClassDefinition* classDef, const vector<Value*>& args);
    
    // 析构函数
    ~TypeConverter();
};

#endif // TYPE_CONVERTER_H