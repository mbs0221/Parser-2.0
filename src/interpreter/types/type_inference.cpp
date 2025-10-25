#include "interpreter/types/type_inference.h"
#include "interpreter/values/value.h"
#include "interpreter/values/container_values.h"
#include "common/logger.h"
#include <algorithm>
#include <sstream>

namespace InterpreterTypes {

// ==================== TypeConstraint 实现 ====================

TypeConstraint::TypeConstraint(ConstraintType type, Type* left, Type* right)
    : constraintType(type), leftType(left), rightType(right) {
}

TypeConstraint::~TypeConstraint() {
    // 注意：不删除leftType和rightType，因为它们可能被其他地方使用
}

bool TypeConstraint::isSatisfied() const {
    if (!leftType || !rightType) {
        return false;
    }
    
    switch (constraintType) {
        case EQUALITY:
            return leftType->equals(rightType);
        case SUBTYPE:
            return leftType->isSubtypeOf(rightType);
        case INSTANCE:
            return leftType->isInstanceOf(rightType);
        default:
            return true; // 其他约束类型暂时返回true
    }
}

std::string TypeConstraint::toString() const {
    std::ostringstream oss;
    
    switch (constraintType) {
        case EQUALITY:
            oss << leftType->toString() << " = " << rightType->toString();
            break;
        case SUBTYPE:
            oss << leftType->toString() << " <: " << rightType->toString();
            break;
        case INSTANCE:
            oss << leftType->toString() << " instance of " << rightType->toString();
            break;
        case CONSTRUCTOR:
            oss << leftType->toString() << " has constructor";
            break;
        case METHOD:
            oss << leftType->toString() << " has method";
            break;
        case PROPERTY:
            oss << leftType->toString() << " has property";
            break;
    }
    
    return oss.str();
}

// ==================== TypeEnvironment 实现 ====================

TypeEnvironment::TypeEnvironment() : parent(nullptr) {
}

TypeEnvironment::TypeEnvironment(TypeEnvironment* parent) : parent(parent) {
}

TypeEnvironment::~TypeEnvironment() {
    // 注意：不删除parent，因为它可能被其他地方使用
}

void TypeEnvironment::bind(const std::string& name, Type* type) {
    bindings[name] = type;
}

Type* TypeEnvironment::lookup(const std::string& name) {
    auto it = bindings.find(name);
    if (it != bindings.end()) {
        return it->second;
    }
    
    if (parent) {
        return parent->lookup(name);
    }
    
    return nullptr;
}

bool TypeEnvironment::isBound(const std::string& name) const {
    if (bindings.find(name) != bindings.end()) {
        return true;
    }
    
    if (parent) {
        return parent->isBound(name);
    }
    
    return false;
}

std::map<std::string, Type*> TypeEnvironment::getAllBindings() const {
    std::map<std::string, Type*> allBindings = bindings;
    
    if (parent) {
        auto parentBindings = parent->getAllBindings();
        allBindings.insert(parentBindings.begin(), parentBindings.end());
    }
    
    return allBindings;
}

std::shared_ptr<TypeEnvironment> TypeEnvironment::createChild() {
    return std::make_shared<TypeEnvironment>(this);
}

void TypeEnvironment::merge(const TypeEnvironment& other) {
    for (const auto& binding : other.bindings) {
        bindings[binding.first] = binding.second;
    }
}

// ==================== TypeInferencer 实现 ====================

TypeInferencer::TypeInferencer() : typeVariableCounter(0) {
}

TypeInferencer::~TypeInferencer() {
    // 清理类型变量
    for (auto& pair : typeVariables) {
        delete pair.second;
    }
}

TypeInferenceResult TypeInferencer::inferExpression(Expression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("表达式为空");
    }
    
    // 根据表达式类型进行推断
    if (ConstantExpression<int>* constExpr = dynamic_cast<ConstantExpression<int>*>(expr)) {
        return inferConstantExpression(constExpr, env);
    } else if (ConstantExpression<double>* constExpr = dynamic_cast<ConstantExpression<double>*>(expr)) {
        return inferConstantExpression(constExpr, env);
    } else if (ConstantExpression<bool>* constExpr = dynamic_cast<ConstantExpression<bool>*>(expr)) {
        return inferConstantExpression(constExpr, env);
    } else if (ConstantExpression<std::string>* constExpr = dynamic_cast<ConstantExpression<std::string>*>(expr)) {
        return inferConstantExpression(constExpr, env);
    } else if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(expr)) {
        return inferVariableExpression(varExpr, env);
    } else if (BinaryExpression* binExpr = dynamic_cast<BinaryExpression*>(expr)) {
        return inferBinaryExpression(binExpr, env);
    } else if (UnaryExpression* unaryExpr = dynamic_cast<UnaryExpression*>(expr)) {
        return inferUnaryExpression(unaryExpr, env);
    } else if (CallExpression* callExpr = dynamic_cast<CallExpression*>(expr)) {
        return inferCallExpression(callExpr, env);
    } else if (TernaryExpression* ternaryExpr = dynamic_cast<TernaryExpression*>(expr)) {
        return inferTernaryExpression(ternaryExpr, env);
    } else {
        return TypeInferenceResult("不支持的表达式类型");
    }
}

TypeInferenceResult TypeInferencer::inferStatement(Statement* stmt, TypeEnvironment* env) {
    if (!stmt) {
        return TypeInferenceResult("语句为空");
    }
    
    // 根据语句类型进行推断
    if (VariableDefinition* varDef = dynamic_cast<VariableDefinition*>(stmt)) {
        // 变量定义语句通常返回void类型
        return TypeInferenceResult(new Type("void", 0));
    } else if (ExpressionStatement* exprStmt = dynamic_cast<ExpressionStatement*>(stmt)) {
        return inferExpression(exprStmt->getExpression(), env);
    } else if (ReturnStatement* returnStmt = dynamic_cast<ReturnStatement*>(stmt)) {
        if (returnStmt->getExpression()) {
            return inferExpression(returnStmt->getExpression(), env);
        } else {
            return TypeInferenceResult(new Type("void", 0));
        }
    } else {
        // 其他语句类型通常返回void
        return TypeInferenceResult(new Type("void", 0));
    }
}

TypeInferenceResult TypeInferencer::inferFunction(FunctionDefinition* func, TypeEnvironment* env) {
    if (!func) {
        return TypeInferenceResult("函数定义为空");
    }
    
    // 创建新的类型环境
    TypeEnvironment* funcEnv = env ? new TypeEnvironment(env) : new TypeEnvironment();
    
    // 推断参数类型
    std::vector<Type*> paramTypes;
    for (const auto& param : func->getParameters()) {
        Type* paramType = new Type(param.second->toString(), param.second->getSize());
        paramTypes.push_back(paramType);
        funcEnv->bind(param.first, paramType);
    }
    
    // 推断返回类型
    Type* returnType = new Type("void", 0);
    if (func->getReturnType()) {
        returnType = new Type(func->getReturnType()->toString(), func->getReturnType()->getSize());
    }
    
    // 创建函数类型
    Type* functionType = new Type("function", 0);
    
    delete funcEnv;
    return TypeInferenceResult(functionType);
}

TypeInferenceResult TypeInferencer::inferClass(ClassDefinition* classDef, TypeEnvironment* env) {
    if (!classDef) {
        return TypeInferenceResult("类定义为空");
    }
    
    // 创建类类型
    Type* classType = new Type(classDef->getName(), 0);
    return TypeInferenceResult(classType);
}

TypeInferenceResult TypeInferencer::inferStruct(StructDefinition* structDef, TypeEnvironment* env) {
    if (!structDef) {
        return TypeInferenceResult("结构体定义为空");
    }
    
    // 创建结构体类型
    Type* structType = new Type(structDef->getName(), 0);
    return TypeInferenceResult(structType);
}

bool TypeInferencer::unify(Type* type1, Type* type2, TypeEnvironment* env) {
    if (!type1 || !type2) {
        return false;
    }
    
    // 如果类型相等，统一成功
    if (type1->equals(type2)) {
        return true;
    }
    
    // 如果一个是类型变量，绑定到另一个类型
    if (isTypeVariable(type1)) {
        if (env) {
            env->bind(type1->toString(), type2);
        }
        return true;
    }
    
    if (isTypeVariable(type2)) {
        if (env) {
            env->bind(type2->toString(), type1);
        }
        return true;
    }
    
    // 检查子类型关系
    if (type1->isSubtypeOf(type2) || type2->isSubtypeOf(type1)) {
        return true;
    }
    
    return false;
}

bool TypeInferencer::solveConstraints(const std::vector<TypeConstraint*>& constraints, TypeEnvironment* env) {
    for (TypeConstraint* constraint : constraints) {
        if (!constraint->isSatisfied()) {
            return false;
        }
    }
    return true;
}

Type* TypeInferencer::createTypeVariable(const std::string& name) {
    std::string varName = name.empty() ? ("T" + std::to_string(typeVariableCounter++)) : name;
    Type* typeVar = new Type(varName, 0);
    typeVariables[varName] = typeVar;
    return typeVar;
}

Type* TypeInferencer::instantiateGeneric(Type* genericType, const std::vector<Type*>& typeArgs) {
    if (!genericType) {
        return nullptr;
    }
    
    // 简化的泛型实例化
    // 实际实现需要更复杂的类型替换逻辑
    return genericType;
}

// ==================== 表达式类型推断实现 ====================

TypeInferenceResult TypeInferencer::inferConstantExpression(ConstantExpression<int>* expr, TypeEnvironment* env) {
    return TypeInferenceResult(new Type("int", 4));
}

TypeInferenceResult TypeInferencer::inferConstantExpression(ConstantExpression<double>* expr, TypeEnvironment* env) {
    return TypeInferenceResult(new Type("double", 8));
}

TypeInferenceResult TypeInferencer::inferConstantExpression(ConstantExpression<bool>* expr, TypeEnvironment* env) {
    return TypeInferenceResult(new Type("bool", 1));
}

TypeInferenceResult TypeInferencer::inferConstantExpression(ConstantExpression<std::string>* expr, TypeEnvironment* env) {
    return TypeInferenceResult(new Type("string", 0));
}

TypeInferenceResult TypeInferencer::inferVariableExpression(VariableExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("变量表达式为空");
    }
    
    if (!env) {
        return TypeInferenceResult("类型环境为空");
    }
    
    Type* varType = env->lookup(expr->getName());
    if (!varType) {
        return TypeInferenceResult("未找到变量类型: " + expr->getName());
    }
    
    return TypeInferenceResult(varType);
}

TypeInferenceResult TypeInferencer::inferBinaryExpression(BinaryExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("二元表达式为空");
    }
    
    // 推断左操作数类型
    TypeInferenceResult leftResult = inferExpression(expr->getLeft(), env);
    if (!leftResult.success) {
        return leftResult;
    }
    
    // 推断右操作数类型
    TypeInferenceResult rightResult = inferExpression(expr->getRight(), env);
    if (!rightResult.success) {
        return rightResult;
    }
    
    // 根据操作符推断结果类型
    std::string op = expr->getOperator()->getSymbol();
    
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
        // 算术操作符
        return TypeInferenceResult(getCommonSupertype(leftResult.inferredType, rightResult.inferredType));
    } else if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
        // 比较操作符
        return TypeInferenceResult(new Type("bool", 1));
    } else if (op == "&&" || op == "||") {
        // 逻辑操作符
        return TypeInferenceResult(new Type("bool", 1));
    } else if (op == "=") {
        // 赋值操作符
        return TypeInferenceResult(leftResult.inferredType);
    } else {
        return TypeInferenceResult("不支持的二元操作符: " + op);
    }
}

TypeInferenceResult TypeInferencer::inferUnaryExpression(UnaryExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("一元表达式为空");
    }
    
    // 推断操作数类型
    TypeInferenceResult operandResult = inferExpression(expr->getOperand(), env);
    if (!operandResult.success) {
        return operandResult;
    }
    
    // 根据操作符推断结果类型
    std::string op = expr->getOperator()->getSymbol();
    
    if (op == "!" || op == "not") {
        // 逻辑非
        return TypeInferenceResult(new Type("bool", 1));
    } else if (op == "-" || op == "+") {
        // 算术正负号
        return TypeInferenceResult(operandResult.inferredType);
    } else if (op == "++" || op == "--") {
        // 自增自减
        return TypeInferenceResult(operandResult.inferredType);
    } else {
        return TypeInferenceResult("不支持的一元操作符: " + op);
    }
}

TypeInferenceResult TypeInferencer::inferCallExpression(CallExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("调用表达式为空");
    }
    
    // 推断函数类型
    TypeInferenceResult funcResult = inferExpression(expr->getCallee(), env);
    if (!funcResult.success) {
        return funcResult;
    }
    
    // 简化的函数调用类型推断
    // 实际实现需要更复杂的函数签名匹配
    return TypeInferenceResult(new Type("unknown", 0));
}

TypeInferenceResult TypeInferencer::inferTernaryExpression(TernaryExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("三元表达式为空");
    }
    
    // 推断条件类型
    TypeInferenceResult conditionResult = inferExpression(expr->getCondition(), env);
    if (!conditionResult.success) {
        return conditionResult;
    }
    
    // 推断真值类型
    TypeInferenceResult trueResult = inferExpression(expr->getTrueValue(), env);
    if (!trueResult.success) {
        return trueResult;
    }
    
    // 推断假值类型
    TypeInferenceResult falseResult = inferExpression(expr->getFalseValue(), env);
    if (!falseResult.success) {
        return falseResult;
    }
    
    // 返回真值和假值的公共超类型
    return TypeInferenceResult(getCommonSupertype(trueResult.inferredType, falseResult.inferredType));
}

// ==================== 高级表达式类型推断实现 ====================

TypeInferenceResult TypeInferencer::inferArrayLiteralExpression(ArrayLiteralExpression* expr, TypeEnvironment* env) {
    if (!expr || expr->getElements().empty()) {
        return TypeInferenceResult(new Type("Array<unknown>", 0));
    }
    
    // 推断所有元素的类型
    std::vector<Type*> elementTypes;
    for (Expression* element : expr->getElements()) {
        TypeInferenceResult elementResult = inferExpression(element, env);
        if (!elementResult.success) {
            return elementResult;
        }
        elementTypes.push_back(elementResult.inferredType);
    }
    
    // 找到所有元素的公共超类型
    Type* commonType = elementTypes[0];
    for (size_t i = 1; i < elementTypes.size(); ++i) {
        commonType = getCommonSupertype(commonType, elementTypes[i]);
    }
    
    return TypeInferenceResult(new Type("Array<" + commonType->toString() + ">", 0));
}

TypeInferenceResult TypeInferencer::inferDictLiteralExpression(DictLiteralExpression* expr, TypeEnvironment* env) {
    if (!expr || expr->getPairs().empty()) {
        return TypeInferenceResult(new Type("Dict<unknown, unknown>", 0));
    }
    
    // 推断键和值的类型
    Type* keyType = nullptr;
    Type* valueType = nullptr;
    
    for (const auto& pair : expr->getPairs()) {
        TypeInferenceResult keyResult = inferExpression(pair.first, env);
        TypeInferenceResult valueResult = inferExpression(pair.second, env);
        
        if (!keyResult.success || !valueResult.success) {
            return TypeInferenceResult("字典字面量类型推断失败");
        }
        
        if (!keyType) {
            keyType = keyResult.inferredType;
            valueType = valueResult.inferredType;
        } else {
            keyType = getCommonSupertype(keyType, keyResult.inferredType);
            valueType = getCommonSupertype(valueType, valueResult.inferredType);
        }
    }
    
    return TypeInferenceResult(new Type("Dict<" + keyType->toString() + ", " + valueType->toString() + ">", 0));
}

TypeInferenceResult TypeInferencer::inferSetLiteralExpression(SetLiteralExpression* expr, TypeEnvironment* env) {
    if (!expr || expr->getElements().empty()) {
        return TypeInferenceResult(new Type("Set<unknown>", 0));
    }
    
    // 推断所有元素的类型
    std::vector<Type*> elementTypes;
    for (Expression* element : expr->getElements()) {
        TypeInferenceResult elementResult = inferExpression(element, env);
        if (!elementResult.success) {
            return elementResult;
        }
        elementTypes.push_back(elementResult.inferredType);
    }
    
    // 找到所有元素的公共超类型
    Type* commonType = elementTypes[0];
    for (size_t i = 1; i < elementTypes.size(); ++i) {
        commonType = getCommonSupertype(commonType, elementTypes[i]);
    }
    
    return TypeInferenceResult(new Type("Set<" + commonType->toString() + ">", 0));
}

TypeInferenceResult TypeInferencer::inferRangeExpression(RangeExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("范围表达式为空");
    }
    
    // 推断开始和结束类型
    TypeInferenceResult startResult = inferExpression(expr->getStart(), env);
    TypeInferenceResult endResult = inferExpression(expr->getEnd(), env);
    
    if (!startResult.success || !endResult.success) {
        return TypeInferenceResult("范围表达式类型推断失败");
    }
    
    // 范围表达式通常返回范围类型
    return TypeInferenceResult(new Type("Range<" + startResult.inferredType->toString() + ">", 0));
}

TypeInferenceResult TypeInferencer::inferSliceExpression(SliceExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("切片表达式为空");
    }
    
    // 推断目标类型
    TypeInferenceResult targetResult = inferExpression(expr->getTarget(), env);
    if (!targetResult.success) {
        return targetResult;
    }
    
    // 切片表达式返回与目标相同的类型
    return TypeInferenceResult(targetResult.inferredType);
}

TypeInferenceResult TypeInferencer::inferNullCoalescingExpression(NullCoalescingExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("空值合并表达式为空");
    }
    
    // 推断左操作数类型
    TypeInferenceResult leftResult = inferExpression(expr->getLeft(), env);
    if (!leftResult.success) {
        return leftResult;
    }
    
    // 推断右操作数类型
    TypeInferenceResult rightResult = inferExpression(expr->getRight(), env);
    if (!rightResult.success) {
        return rightResult;
    }
    
    // 返回右操作数的类型（非空类型）
    return TypeInferenceResult(rightResult.inferredType);
}

TypeInferenceResult TypeInferencer::inferMatchExpression(MatchExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("匹配表达式为空");
    }
    
    // 推断输入类型
    TypeInferenceResult inputResult = inferExpression(expr->getInput(), env);
    if (!inputResult.success) {
        return inputResult;
    }
    
    // 推断所有分支的类型
    std::vector<Type*> branchTypes;
    for (Expression* branch : expr->getBranches()) {
        TypeInferenceResult branchResult = inferExpression(branch, env);
        if (!branchResult.success) {
            return branchResult;
        }
        branchTypes.push_back(branchResult.inferredType);
    }
    
    // 找到所有分支的公共超类型
    if (branchTypes.empty()) {
        return TypeInferenceResult(new Type("void", 0));
    }
    
    Type* commonType = branchTypes[0];
    for (size_t i = 1; i < branchTypes.size(); ++i) {
        commonType = getCommonSupertype(commonType, branchTypes[i]);
    }
    
    return TypeInferenceResult(commonType);
}

TypeInferenceResult TypeInferencer::inferAsyncExpression(AsyncExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("异步表达式为空");
    }
    
    // 推断表达式类型
    TypeInferenceResult exprResult = inferExpression(expr->getExpression(), env);
    if (!exprResult.success) {
        return exprResult;
    }
    
    // 异步表达式返回Promise类型
    return TypeInferenceResult(new Type("Promise<" + exprResult.inferredType->toString() + ">", 0));
}

TypeInferenceResult TypeInferencer::inferAwaitExpression(AwaitExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("等待表达式为空");
    }
    
    // 推断Promise类型
    TypeInferenceResult promiseResult = inferExpression(expr->getExpression(), env);
    if (!promiseResult.success) {
        return promiseResult;
    }
    
    // 等待表达式返回Promise内部的类型
    // 简化实现：假设Promise<T>返回T
    return TypeInferenceResult(new Type("unknown", 0));
}

TypeInferenceResult TypeInferencer::inferTypeCheckExpression(TypeCheckExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("类型检查表达式为空");
    }
    
    // 类型检查表达式总是返回bool
    return TypeInferenceResult(new Type("bool", 1));
}

TypeInferenceResult TypeInferencer::inferTypeCastExpression(TypeCastExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("类型转换表达式为空");
    }
    
    // 推断目标类型
    TypeInferenceResult targetResult = inferExpression(expr->getTarget(), env);
    if (!targetResult.success) {
        return targetResult;
    }
    
    // 类型转换表达式返回目标类型
    return TypeInferenceResult(targetResult.inferredType);
}

TypeInferenceResult TypeInferencer::inferCompoundAssignExpression(CompoundAssignExpression* expr, TypeEnvironment* env) {
    if (!expr) {
        return TypeInferenceResult("复合赋值表达式为空");
    }
    
    // 推断左操作数类型
    TypeInferenceResult leftResult = inferExpression(expr->getLeft(), env);
    if (!leftResult.success) {
        return leftResult;
    }
    
    // 复合赋值表达式返回左操作数的类型
    return TypeInferenceResult(leftResult.inferredType);
}

// ==================== 辅助方法实现 ====================

Type* TypeInferencer::getCommonSupertype(Type* type1, Type* type2) {
    if (!type1 || !type2) {
        return nullptr;
    }
    
    if (type1->equals(type2)) {
        return type1;
    }
    
    // 简化的公共超类型查找
    // 实际实现需要更复杂的类型层次结构
    if (type1->toString() == "int" && type2->toString() == "double") {
        return new Type("double", 8);
    }
    
    if (type1->toString() == "double" && type2->toString() == "int") {
        return new Type("double", 8);
    }
    
    // 默认返回第一个类型
    return type1;
}

Type* TypeInferencer::getCommonSubtype(Type* type1, Type* type2) {
    if (!type1 || !type2) {
        return nullptr;
    }
    
    if (type1->equals(type2)) {
        return type1;
    }
    
    // 简化的公共子类型查找
    return type1;
}

bool TypeInferencer::isSubtype(Type* subtype, Type* supertype) {
    if (!subtype || !supertype) {
        return false;
    }
    
    return subtype->isSubtypeOf(supertype);
}

bool TypeInferencer::isCompatible(Type* type1, Type* type2) {
    if (!type1 || !type2) {
        return false;
    }
    
    return type1->equals(type2) || isSubtype(type1, type2) || isSubtype(type2, type1);
}

Type* TypeInferencer::promoteType(Type* type) {
    if (!type) {
        return nullptr;
    }
    
    // 类型提升规则
    if (type->toString() == "char") {
        return new Type("int", 4);
    }
    
    if (type->toString() == "float") {
        return new Type("double", 8);
    }
    
    return type;
}

std::vector<Type*> TypeInferencer::getTypeHierarchy(Type* type) {
    std::vector<Type*> hierarchy;
    
    if (!type) {
        return hierarchy;
    }
    
    // 简化的类型层次结构
    hierarchy.push_back(type);
    
    return hierarchy;
}

bool TypeInferencer::isTypeVariable(Type* type) {
    if (!type) {
        return false;
    }
    
    std::string typeName = type->toString();
    return typeName.length() > 0 && typeName[0] == 'T';
}

// ==================== TypeChecker 实现 ====================

TypeChecker::TypeChecker() {
    inferencer = new TypeInferencer();
}

TypeChecker::~TypeChecker() {
    delete inferencer;
}

bool TypeChecker::checkExpression(Expression* expr, Type* expectedType, TypeEnvironment* env) {
    if (!expr) {
        addTypeError("表达式为空");
        return false;
    }
    
    TypeInferenceResult result = inferencer->inferExpression(expr, env);
    if (!result.success) {
        addTypeError("表达式类型推断失败: " + result.errorMessage);
        return false;
    }
    
    if (expectedType && !checkTypeCompatibility(result.inferredType, expectedType)) {
        addTypeError("类型不兼容: 期望 " + expectedType->toString() + ", 得到 " + result.inferredType->toString());
        return false;
    }
    
    return true;
}

bool TypeChecker::checkStatement(Statement* stmt, TypeEnvironment* env) {
    if (!stmt) {
        addTypeError("语句为空");
        return false;
    }
    
    TypeInferenceResult result = inferencer->inferStatement(stmt, env);
    if (!result.success) {
        addTypeError("语句类型推断失败: " + result.errorMessage);
        return false;
    }
    
    return true;
}

bool TypeChecker::checkFunction(FunctionDefinition* func, TypeEnvironment* env) {
    if (!func) {
        addTypeError("函数定义为空");
        return false;
    }
    
    TypeInferenceResult result = inferencer->inferFunction(func, env);
    if (!result.success) {
        addTypeError("函数类型推断失败: " + result.errorMessage);
        return false;
    }
    
    return true;
}

bool TypeChecker::checkClass(ClassDefinition* classDef, TypeEnvironment* env) {
    if (!classDef) {
        addTypeError("类定义为空");
        return false;
    }
    
    TypeInferenceResult result = inferencer->inferClass(classDef, env);
    if (!result.success) {
        addTypeError("类类型推断失败: " + result.errorMessage);
        return false;
    }
    
    return true;
}

bool TypeChecker::checkStruct(StructDefinition* structDef, TypeEnvironment* env) {
    if (!structDef) {
        addTypeError("结构体定义为空");
        return false;
    }
    
    TypeInferenceResult result = inferencer->inferStruct(structDef, env);
    if (!result.success) {
        addTypeError("结构体类型推断失败: " + result.errorMessage);
        return false;
    }
    
    return true;
}

std::vector<std::string> TypeChecker::getTypeErrors() const {
    return typeErrors;
}

void TypeChecker::clearTypeErrors() {
    typeErrors.clear();
}

void TypeChecker::addTypeError(const std::string& error) {
    typeErrors.push_back(error);
}

bool TypeChecker::checkTypeCompatibility(Type* actual, Type* expected) {
    if (!actual || !expected) {
        return false;
    }
    
    return actual->equals(expected) || actual->isSubtypeOf(expected);
}

bool TypeChecker::checkAssignmentCompatibility(Type* left, Type* right) {
    if (!left || !right) {
        return false;
    }
    
    return checkTypeCompatibility(right, left);
}

bool TypeChecker::checkFunctionCallCompatibility(FunctionSignature* signature, const std::vector<Type*>& argTypes) {
    if (!signature) {
        return false;
    }
    
    const std::vector<Type*>& paramTypes = signature->getParameterTypes();
    if (paramTypes.size() != argTypes.size()) {
        return false;
    }
    
    for (size_t i = 0; i < paramTypes.size(); ++i) {
        if (!checkTypeCompatibility(argTypes[i], paramTypes[i])) {
            return false;
        }
    }
    
    return true;
}


// ==================== TypeInferenceManager 实现 ====================

TypeInferenceManager& TypeInferenceManager::getInstance() {
    static TypeInferenceManager instance;
    return instance;
}

TypeInferenceManager::TypeInferenceManager() {
    inferencer = new TypeInferencer();
    checker = new TypeChecker();
}

TypeInferenceManager::~TypeInferenceManager() {
    delete inferencer;
    delete checker;
}

TypeInferenceResult TypeInferenceManager::infer(Expression* expr, TypeEnvironment* env) {
    return inferencer->inferExpression(expr, env);
}

TypeInferenceResult TypeInferenceManager::infer(Statement* stmt, TypeEnvironment* env) {
    return inferencer->inferStatement(stmt, env);
}

TypeInferenceResult TypeInferenceManager::infer(FunctionDefinition* func, TypeEnvironment* env) {
    return inferencer->inferFunction(func, env);
}

bool TypeInferenceManager::check(Expression* expr, Type* expectedType, TypeEnvironment* env) {
    return checker->checkExpression(expr, expectedType, env);
}

bool TypeInferenceManager::check(Statement* stmt, TypeEnvironment* env) {
    return checker->checkStatement(stmt, env);
}

bool TypeInferenceManager::check(FunctionDefinition* func, TypeEnvironment* env) {
    return checker->checkFunction(func, env);
}


} // namespace InterpreterTypes
