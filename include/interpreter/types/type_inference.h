#ifndef TYPE_INFERENCE_H
#define TYPE_INFERENCE_H

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace InterpreterTypes {

// 类型推断结果
struct TypeInferenceResult {
    Type* inferredType;
    bool success;
    std::string errorMessage;
    std::map<std::string, Type*> typeVariables;
    
    TypeInferenceResult() : inferredType(nullptr), success(false) {}
    TypeInferenceResult(Type* type) : inferredType(type), success(true) {}
    TypeInferenceResult(const std::string& error) : inferredType(nullptr), success(false), errorMessage(error) {}
};

// 类型约束
class TypeConstraint {
public:
    enum ConstraintType {
        EQUALITY,      // T1 = T2
        SUBTYPE,       // T1 <: T2
        INSTANCE,      // T1 instance of T2
        CONSTRUCTOR,   // T has constructor
        METHOD,        // T has method
        PROPERTY       // T has property
    };
    
    TypeConstraint(ConstraintType type, Type* left, Type* right = nullptr);
    ~TypeConstraint();
    
    // 获取约束类型
    ConstraintType getType() const { return constraintType; }
    
    // 获取左类型
    Type* getLeft() const { return leftType; }
    
    // 获取右类型
    Type* getRight() const { return rightType; }
    
    // 检查约束是否满足
    bool isSatisfied() const;
    
    // 获取字符串表示
    std::string toString() const;

private:
    ConstraintType constraintType;
    Type* leftType;
    Type* rightType;
};

// 类型环境
class TypeEnvironment {
public:
    TypeEnvironment();
    TypeEnvironment(TypeEnvironment* parent);
    ~TypeEnvironment();
    
    // 绑定类型变量
    void bind(const std::string& name, Type* type);
    
    // 查找类型变量
    Type* lookup(const std::string& name);
    
    // 检查是否已绑定
    bool isBound(const std::string& name) const;
    
    // 获取所有绑定
    std::map<std::string, Type*> getAllBindings() const;
    
    // 创建子环境
    std::shared_ptr<TypeEnvironment> createChild();
    
    // 合并环境
    void merge(const TypeEnvironment& other);

private:
    std::map<std::string, Type*> bindings;
    TypeEnvironment* parent;
};

// 类型推断器
class TypeInferencer {
public:
    TypeInferencer();
    ~TypeInferencer();
    
    // 推断表达式类型
    TypeInferenceResult inferExpression(Expression* expr, TypeEnvironment* env = nullptr);
    
    // 推断语句类型
    TypeInferenceResult inferStatement(Statement* stmt, TypeEnvironment* env = nullptr);
    
    // 推断函数类型
    TypeInferenceResult inferFunction(FunctionDefinition* func, TypeEnvironment* env = nullptr);
    
    // 推断类类型
    TypeInferenceResult inferClass(ClassDefinition* classDef, TypeEnvironment* env = nullptr);
    
    // 推断结构体类型
    TypeInferenceResult inferStruct(StructDefinition* structDef, TypeEnvironment* env = nullptr);
    
    // 类型统一
    bool unify(Type* type1, Type* type2, TypeEnvironment* env);
    
    // 类型约束求解
    bool solveConstraints(const std::vector<TypeConstraint*>& constraints, TypeEnvironment* env);
    
    // 创建类型变量
    Type* createTypeVariable(const std::string& name);
    
    // 实例化泛型类型
    Type* instantiateGeneric(Type* genericType, const std::vector<Type*>& typeArgs);

private:
    // 表达式类型推断
    TypeInferenceResult inferConstantExpression(ConstantExpression<int>* expr, TypeEnvironment* env);
    TypeInferenceResult inferConstantExpression(ConstantExpression<double>* expr, TypeEnvironment* env);
    TypeInferenceResult inferConstantExpression(ConstantExpression<bool>* expr, TypeEnvironment* env);
    TypeInferenceResult inferConstantExpression(ConstantExpression<std::string>* expr, TypeEnvironment* env);
    TypeInferenceResult inferVariableExpression(VariableExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferBinaryExpression(BinaryExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferUnaryExpression(UnaryExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferCallExpression(CallExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferTernaryExpression(TernaryExpression* expr, TypeEnvironment* env);
    
    // 高级表达式类型推断
    TypeInferenceResult inferArrayLiteralExpression(ArrayLiteralExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferDictLiteralExpression(DictLiteralExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferSetLiteralExpression(SetLiteralExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferRangeExpression(RangeExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferSliceExpression(SliceExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferNullCoalescingExpression(NullCoalescingExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferMatchExpression(MatchExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferAsyncExpression(AsyncExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferAwaitExpression(AwaitExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferTypeCheckExpression(TypeCheckExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferTypeCastExpression(TypeCastExpression* expr, TypeEnvironment* env);
    TypeInferenceResult inferCompoundAssignExpression(CompoundAssignExpression* expr, TypeEnvironment* env);
    
    // 辅助方法
    Type* getCommonSupertype(Type* type1, Type* type2);
    Type* getCommonSubtype(Type* type1, Type* type2);
    bool isSubtype(Type* subtype, Type* supertype);
    bool isCompatible(Type* type1, Type* type2);
    Type* promoteType(Type* type);
    std::vector<Type*> getTypeHierarchy(Type* type);
    
    // 类型变量管理
    int typeVariableCounter;
    std::map<std::string, Type*> typeVariables;
};

// 类型检查器
class TypeChecker {
public:
    TypeChecker();
    ~TypeChecker();
    
    // 检查表达式类型
    bool checkExpression(Expression* expr, Type* expectedType, TypeEnvironment* env = nullptr);
    
    // 检查语句类型
    bool checkStatement(Statement* stmt, TypeEnvironment* env = nullptr);
    
    // 检查函数类型
    bool checkFunction(FunctionDefinition* func, TypeEnvironment* env = nullptr);
    
    // 检查类类型
    bool checkClass(ClassDefinition* classDef, TypeEnvironment* env = nullptr);
    
    // 检查结构体类型
    bool checkStruct(StructDefinition* structDef, TypeEnvironment* env = nullptr);
    
    // 获取类型错误
    std::vector<std::string> getTypeErrors() const;
    
    // 清除类型错误
    void clearTypeErrors();

private:
    TypeInferencer* inferencer;
    std::vector<std::string> typeErrors;
    
    // 添加类型错误
    void addTypeError(const std::string& error);
    
    // 检查类型兼容性
    bool checkTypeCompatibility(Type* actual, Type* expected);
    
    // 检查赋值兼容性
    bool checkAssignmentCompatibility(Type* left, Type* right);
    
    // 检查函数调用兼容性
    bool checkFunctionCallCompatibility(FunctionSignature* signature, const std::vector<Type*>& argTypes);
};


// 类型推断管理器
class TypeInferenceManager {
public:
    static TypeInferenceManager& getInstance();
    
    // 推断类型
    TypeInferenceResult infer(Expression* expr, TypeEnvironment* env = nullptr);
    TypeInferenceResult infer(Statement* stmt, TypeEnvironment* env = nullptr);
    TypeInferenceResult infer(FunctionDefinition* func, TypeEnvironment* env = nullptr);
    
    // 检查类型
    bool check(Expression* expr, Type* expectedType, TypeEnvironment* env = nullptr);
    bool check(Statement* stmt, TypeEnvironment* env = nullptr);
    bool check(FunctionDefinition* func, TypeEnvironment* env = nullptr);
    
    // 获取类型推断器
    TypeInferencer* getInferencer() { return inferencer; }
    
    // 获取类型检查器
    TypeChecker* getChecker() { return checker; }

private:
    TypeInferenceManager();
    ~TypeInferenceManager();
    
    // 禁用拷贝构造和赋值
    TypeInferenceManager(const TypeInferenceManager&) = delete;
    TypeInferenceManager& operator=(const TypeInferenceManager&) = delete;
    
    TypeInferencer* inferencer;
    TypeChecker* checker;
};

} // namespace InterpreterTypes

#endif // TYPE_INFERENCE_H
