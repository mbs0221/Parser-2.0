#include "interpreter/interpreter.h"
#include "parser/expression.h"
#include "parser/function.h"
#include "parser/inter.h"
#include "lexer/lexer.h"
#include "interpreter/logger.h"
#include "lexer/value.h"
#include "interpreter/builtin.h"
#include <iostream>
#include <sstream>
#include <typeinfo>

using namespace std;

// 解释器构造函数
Interpreter::Interpreter() {
    // ScopeManager在构造函数中自动初始化
    
    // 注册内置函数到作用域管理器
    registerBuiltinFunctionsToScope();
}

// 解释器析构函数
Interpreter::~Interpreter() {
    // ScopeManager在析构函数中自动清理
}

// visit(AST*)方法已移除，使用具体的visit方法

// 语句访问方法 - 无返回值
void Interpreter::visit(Statement* stmt) {
    if (!stmt) return;
    
    // 使用动态分发调用对应的visit方法
    if (ImportStatement* importStmt = dynamic_cast<ImportStatement*>(stmt)) {
        visit(importStmt);
    } else if (ExpressionStatement* exprStmt = dynamic_cast<ExpressionStatement*>(stmt)) {
        visit(exprStmt);
    } else if (VariableDeclaration* varDecl = dynamic_cast<VariableDeclaration*>(stmt)) {
        visit(varDecl);
    } else if (IfStatement* ifStmt = dynamic_cast<IfStatement*>(stmt)) {
        visit(ifStmt);
    } else if (WhileStatement* whileStmt = dynamic_cast<WhileStatement*>(stmt)) {
        visit(whileStmt);
    } else if (ForStatement* forStmt = dynamic_cast<ForStatement*>(stmt)) {
        visit(forStmt);
    } else if (DoWhileStatement* doWhileStmt = dynamic_cast<DoWhileStatement*>(stmt)) {
        visit(doWhileStmt);
    } else if (BlockStatement* blockStmt = dynamic_cast<BlockStatement*>(stmt)) {
        visit(blockStmt);
    } else if (BreakStatement* breakStmt = dynamic_cast<BreakStatement*>(stmt)) {
        visit(breakStmt);
    } else if (ContinueStatement* continueStmt = dynamic_cast<ContinueStatement*>(stmt)) {
        visit(continueStmt);
    } else if (ReturnStatement* returnStmt = dynamic_cast<ReturnStatement*>(stmt)) {
        visit(returnStmt);
    } else if (ThrowStatement* throwStmt = dynamic_cast<ThrowStatement*>(stmt)) {
        visit(throwStmt);
    } else if (TryStatement* tryStmt = dynamic_cast<TryStatement*>(stmt)) {
        visit(tryStmt);
    } else if (SwitchStatement* switchStmt = dynamic_cast<SwitchStatement*>(stmt)) {
        visit(switchStmt);
    } else {
        reportError("Unknown statement type");
    }
}

// 执行语句
void Interpreter::execute(Statement* stmt) {
    if (!stmt) return;
    stmt->accept(this);
}

// 执行程序
void Interpreter::execute(Program* program) {
    if (!program) return;
    program->accept(this);
}

// 求值表达式 - 使用访问者模式
Value* Interpreter::visit(Expression* expr) {
    if (!expr) return nullptr;
    
    // 使用动态分发调用对应的visit方法
    if (ConstantExpression* constExpr = dynamic_cast<ConstantExpression*>(expr)) {
        return visit(constExpr);
    } else if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(expr)) {
        return visit(varExpr);
    } else if (UnaryExpression* unaryExpr = dynamic_cast<UnaryExpression*>(expr)) {
        return visit(unaryExpr);
    } else if (AssignExpression* assignExpr = dynamic_cast<AssignExpression*>(expr)) {
        return visit(assignExpr);
    } else if (BinaryExpression* binaryExpr = dynamic_cast<BinaryExpression*>(expr)) {
        return visit(binaryExpr);
    } else if (CastExpression* castExpr = dynamic_cast<CastExpression*>(expr)) {
        return visit(castExpr);
    } else if (AccessExpression* accessExpr = dynamic_cast<AccessExpression*>(expr)) {
        return visit(accessExpr);
    } else if (CallExpression* callExpr = dynamic_cast<CallExpression*>(expr)) {
        return visit(callExpr);
    } else if (MethodCallExpression* methodExpr = dynamic_cast<MethodCallExpression*>(expr)) {
        return visit(methodExpr);
    }
    
    reportError("Unknown expression type");
    return nullptr;
}

// 声明求值 - 语句类型，不需要返回值
void Interpreter::visit(VariableDeclaration* decl) {
    if (!decl) return;
    
    LOG_DEBUG("VariableDeclaration: processing " + to_string(decl->variables.size()) + " variables");
    
    // 处理多个变量声明
    for (size_t i = 0; i < decl->variables.size(); ++i) {
        const auto& var = decl->variables[i];
        string name = var.first;
        Expression* initialValue = var.second;
        Value* value = nullptr;
        
        LOG_DEBUG("VariableDeclaration: processing variable " + to_string(i) + ": " + name);
        
        if (initialValue) {
            LOG_DEBUG("VariableDeclaration: evaluating initial value for " + name);
            value = visit(initialValue);
            LOG_DEBUG("VariableDeclaration: initial value result: " + string(value ? "valid" : "null"));
        }
        
        // 如果变量没有初始值，设置为默认值（数字0）
        if (!value) {
            LOG_DEBUG("VariableDeclaration: creating default value for " + name);
            value = new Integer(0);
        }
        
        // 使用Variable类型存储
        LOG_DEBUG("VariableDeclaration: defining variable " + name);
        scopeManager.defineVariable(name, value);
        
        LOG_DEBUG(std::string("Declaring variable '") + name + "' with value " + (value == nullptr ? "null" : value->toString()));
    }
}

// 变量引用表达式求值 - 查询作用域中的变量定义
Value* Interpreter::visit(VariableExpression* varExpr) {
    if (!varExpr) {
        reportError("Null variable expression");
        return nullptr;
    }
    
    string name = varExpr->name;
    Variable* variable = scopeManager.lookupVariable(name);
    
    if (!variable) {
        reportError("Undefined variable: " + name);
        return nullptr;
    }
    
    Value* value = variable->getValue();
    LOG_DEBUG("VariableExpression: looking up variable '" + name + "', value pointer: " + (value ? "valid" : "null"));
    
    if (!value) {
        reportError("Variable has no value: " + name);
        return nullptr;
    }

    LOG_DEBUG("VariableExpression: about to call toString() on value");
    
    // 添加额外的安全检查
    if (value == nullptr) {
        reportError("Value became null after check: " + name);
        return nullptr;
    }
    
    try {
        string valueStr = value->toString();
        LOG_DEBUG("VariableExpression: " + name + " = " + valueStr);
        return value;
    } catch (...) {
        reportError("Exception in toString() for variable: " + name);
        return nullptr;
    }
}



// 赋值表达式现在使用BinaryExpression处理，在visit(BinaryExpression*)中实现

// 一元表达式求值
Value* Interpreter::visit(UnaryExpression* unary) {
    if (!unary || !unary->operand || !unary->operator_) {
        reportError("Invalid unary expression");
        return nullptr;
    }
    
    // 处理操作数
    Value* operand = visit(unary->operand);
    if (!operand) {
        reportError("Invalid operand in unary expression");
        return nullptr;
    }

    // 根据操作符类型确定目标类型
    string targetType;
    switch (unary->operator_->Tag) {
        case '!': targetType = "bool"; break;   // 逻辑非
        case '-': targetType = "double"; break; // 一元负号
        case '+': targetType = "double"; break; // 一元正号
        case '~': targetType = "int"; break;    // 位取反
        default:
            reportError("Unknown unary operator Tag " + to_string(unary->operator_->Tag));
            return nullptr;
    }
    
    // 使用泛型calculate方法进行计算，内部会自动进行类型转换
    Value* result = nullptr;
    if (targetType == "int") {
        result = calculate<Integer>(unary->operand, unary->operator_->Tag);
    } else if (targetType == "double") {
        result = calculate<Double>(unary->operand, unary->operator_->Tag);
    } else if (targetType == "bool") {
        result = calculate<Bool>(unary->operand, unary->operator_->Tag);
    } else if (targetType == "char") {
        result = calculate<Char>(unary->operand, unary->operator_->Tag);
    } else {
        reportError("Unsupported operand type for unary operator");
        return nullptr;
    }
    return result;
}

// 二元运算表达式求值 - 返回Value类型
Value* Interpreter::visit(BinaryExpression* binary) {
    if (!binary || !binary->left || !binary->right || !binary->operator_) {
        reportError("Invalid binary expression");
        return nullptr;
    }
    
    // 1. 计算左右表达式的值
    Value* left = visit(binary->left);
    Value* right = visit(binary->right);
    
    if (!left || !right) {
        reportError("Invalid operands in binary expression");
        return nullptr;
    }
    
    LOG_DEBUG("BinaryExpression left: " + left->toString());
    LOG_DEBUG("BinaryExpression right: " + right->toString());

    // 2. 获取操作符
    Operator* op = binary->operator_;
    if (!op) {
        reportError("Invalid operator in binary expression");
        return nullptr;
    }
    int opTag = op->Tag;
    LOG_DEBUG("executeBinaryOperation called with operator: " + op->getSymbol());
 
    // 4. 确定兼容类型
    string targetType = determineTargetType(left, right, op);
    if (targetType == "unknown") {
        reportError("Cannot determine compatible type for operation");
        return nullptr;
    }
    
    CastExpression* leftCast = new CastExpression(binary->left, targetType);
    CastExpression* rightCast = new CastExpression(binary->right, targetType);    

    Value* result = nullptr;
    try {
        if (targetType == "int") {
            result = calculate<Integer>(leftCast, rightCast, opTag);
        } else if (targetType == "double") {
            result = calculate<Double>(leftCast, rightCast, opTag);
        } else if (targetType == "bool") {
            result = calculate<Bool>(leftCast, rightCast, opTag);
        } else if (targetType == "string") {
            result = calculate<String>(leftCast, rightCast, opTag);
        } else if (targetType == "char") {
            result = calculate<Char>(leftCast, rightCast, opTag);
        }
    } catch (const std::exception& e) {
        delete leftCast;
        delete rightCast;
        reportError("Binary operation error: " + string(e.what()));
        return nullptr;
    }

    delete leftCast;
    delete rightCast;
    
    if (!result) {
        reportError("Type mismatch in binary expression: " + left->toString() + " " + std::string(1, opTag) + " " + right->toString());
    }
    
    return result;
}

// 赋值表达式求值 - 专门处理赋值操作
Value* Interpreter::visit(AssignExpression* assign) {
    if (!assign || !assign->left || !assign->right) {
        reportError("Invalid assignment expression");
        return nullptr;
    }
    
    // 检查左操作数是否为变量引用
    if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(assign->left)) {
        // 计算右边的值
        Value* rightValue = visit(assign->right);
        if (!rightValue) {
            reportError("Failed to evaluate right side of assignment");
            return nullptr;
        }
        
        // 获取变量的当前类型
        Variable* variable = scopeManager.lookupVariable(varExpr->name);
        string targetType;
        
        if (variable && variable->getValue()) {
            // 如果变量已定义，使用左边变量的类型
            targetType = variable->getValue()->getTypeName();
        } else {
            // 如果变量未定义，使用右边值的类型
            targetType = rightValue->getTypeName();
        }
        
        // 将右边的值cast到目标类型
        CastExpression* rightCast = new CastExpression(assign->right, targetType);
        Value* castedValue = visit(rightCast);
        delete rightCast;
        
        if (!castedValue) {
            reportError("Failed to cast value to type " + targetType + " for assignment");
            delete rightValue;
            return nullptr;
        }
        
        // 更新变量值
        scopeManager.updateVariable(varExpr->name, castedValue);
        LOG_DEBUG("Assigned value " + castedValue->toString() + " to variable '" + varExpr->name + "'");
        return castedValue;  // 赋值表达式返回转换后的值
    } else {
        reportError("Left side of assignment must be a variable");
        return nullptr;
    }
}

// 辅助方法：确定运算的目标类型
string Interpreter::determineTargetType(Value* left, Value* right, Operator* op) {
    if (!op) return "unknown";
    
    int opTag = op->Tag;

    // 逻辑运算：统一转换为布尔类型
    if (opTag == AND_AND || opTag == OR_OR) {
        return "bool";
    }
    
    // 比较运算：统一返回布尔类型
    if (opTag == EQ_EQ || opTag == NE_EQ || opTag == '<' || opTag == '>' || opTag == LT || opTag == GT || opTag == LE || opTag == GE) {
        return "bool";
    }
    
    // 位运算：统一转换为整数类型
    if (opTag == '&' || opTag == '|' || opTag == '^' || 
        opTag == BIT_AND || opTag == BIT_OR || opTag == BIT_XOR || 
        opTag == LEFT_SHIFT || opTag == RIGHT_SHIFT) {
        return "int";
    }
    
    // 字符串类型：如果任一操作数是字符串，则返回string
    // 数值类型转换：优先级 double > int > char > bool
    if ((dynamic_cast<String*>(left) || dynamic_cast<String*>(right))) {
        return "string";
    } else if (dynamic_cast<Double*>(left) || dynamic_cast<Double*>(right)) {
        return "double";
    } else if (dynamic_cast<Integer*>(left) || dynamic_cast<Integer*>(right)) {
        return "int";
    } else if (dynamic_cast<Char*>(left) || dynamic_cast<Char*>(right)) {
        // 对于字符运算，根据操作符类型决定返回类型
        if (opTag == '+') {
            return "char";  // 字符拼接返回字符
        } else {
            return "int";   // 其他运算转换为整数
        }
    } else if (dynamic_cast<Bool*>(left) || dynamic_cast<Bool*>(right)) {
        return "int";  // 布尔转换为整数进行运算
    }
    
    return "unknown";
}





// 访问表达式求值 - 统一处理数组/字典访问和成员访问
Value* Interpreter::visit(AccessExpression* access) {
    if (!access || !access->target || !access->key) return nullptr;
    
    // 求值目标
    Value* target = visit(access->target);
    if (!target) return nullptr;
    
    // 求值键
    Value* key = visit(access->key);
    if (!key) return nullptr;
    
    // 检查键是否是字符串常量（成员访问）
    if (String* strKey = dynamic_cast<String*>(key)) {
        string memberName = strKey->getValue();
        
        // 检查对象是否是结构体实例（Dict）
        if (Dict* dict = dynamic_cast<Dict*>(target)) {
            Value* member = dict->getEntry(memberName);
            if (member) {
                return member;
            } else {
                reportError("Member '" + memberName + "' not found in struct instance");
                return nullptr;
            }
        } else {
            reportError("Cannot access member '" + memberName + "' on non-struct object");
            return nullptr;
        }
    } else {
        // 数组/字典访问：使用表达式作为键
        // 检查目标是否是数组或字典
        if (Array* array = dynamic_cast<Array*>(target)) {
            if (Integer* index = dynamic_cast<Integer*>(key)) {
                int idx = index->getValue();
                if (idx >= 0 && idx < (int)array->size()) {
                    return array->getElement(idx);
                } else {
                    reportError("Array index out of bounds: " + to_string(idx));
                    return nullptr;
                }
            } else {
                reportError("Array index must be an integer");
                return nullptr;
            }
        } else if (Dict* dict = dynamic_cast<Dict*>(target)) {
            return dict->access(key);
        } else {
            reportError("Cannot access non-array/non-dict object with key");
            return nullptr;
        }
    }
}

// 函数调用表达式求值
Value* Interpreter::visit(CallExpression* call) {
    if (!call) return nullptr;
    
    // 求值所有参数
    vector<Value*> evaluatedArgs;
    for (Expression* arg : call->arguments) {
        Value* evaluatedArg = visit(arg);
        evaluatedArgs.push_back(evaluatedArg); // 即使为nullptr也要添加，保持索引对应关系
    }
    
    // 使用函数名调用
    string funcName = call->functionName;
    
    // 查找标识符（可能是函数、类或结构体）
    Identifier* identifier = scopeManager.lookupIdentifier(funcName);
    if (!identifier) {
        throw RuntimeException("Identifier not found: " + funcName);
    }
    
    // 检查是否为类或结构体定义（实例化）
    ClassDefinition* classDef = dynamic_cast<ClassDefinition*>(identifier);
    if (classDef) {
        LOG_DEBUG("Instantiating class '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        return instantiateClass(classDef, evaluatedArgs);
    }
    
    StructDefinition* structDef = dynamic_cast<StructDefinition*>(identifier);
    if (structDef) {
        LOG_DEBUG("Instantiating struct '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        return instantiateStruct(structDef, evaluatedArgs);
    }
    
    // 尝试转换为函数定义
    FunctionDefinition* funcDef = dynamic_cast<FunctionDefinition*>(identifier);
    if (!funcDef) {
        throw RuntimeException("Identifier '" + funcName + "' is not a function, class, or struct");
    }
    
    // 统一处理函数调用
    if (funcDef->isBuiltin()) {
        // 内置函数调用
        LOG_DEBUG("Calling builtin function '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        
        // 将参数转换为Variable*，支持引用参数
        vector<Variable*> args;
        for (size_t i = 0; i < call->arguments.size(); ++i) {
            Expression* argExpr = call->arguments[i];
            Value* val = evaluatedArgs[i];
            
            // 检查是否为变量引用（用于 cin(number) 的情况）
            if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(argExpr)) {
                // 查找原始变量
                Identifier* originalVar = scopeManager.lookupIdentifier(varExpr->name);
                if (Variable* originalVariable = dynamic_cast<Variable*>(originalVar)) {
                    // 使用原始变量的引用
                    args.push_back(originalVariable);
                } else {
                    // 如果找不到原始变量，创建临时变量
                    Type* argType = val ? val->valueType : nullptr;
                    args.push_back(new Variable("", argType, val));
                }
            } else {
                // 非变量引用，创建临时变量
                Type* argType = val ? val->valueType : nullptr;
                args.push_back(new Variable("", argType, val));
            }
        }
        
        // 统一调用函数
        Value* result = funcDef->execute(args);
        
        // 清理临时变量（只清理非引用的变量）
        for (size_t i = 0; i < args.size(); ++i) {
            Expression* argExpr = call->arguments[i];
            if (!dynamic_cast<VariableExpression*>(argExpr)) {
                delete args[i];
            }
        }
        
        return result;
    } else {
        // 用户函数调用
        LOG_DEBUG("Calling user function '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        
        // 进入新的作用域并执行函数体
        return withScope([&]() -> Value* {
            // 绑定参数到局部变量
            const vector<pair<string, Type*>>& params = funcDef->prototype->parameters;
            for (size_t i = 0; i < params.size() && i < evaluatedArgs.size(); ++i) {
                scopeManager.defineVariable(params[i].first, evaluatedArgs[i]);
                LOG_DEBUG("Bound parameter '" + params[i].first + "'");
            }
            
            // 执行函数体
            Value* result = nullptr;
            try {
                execute(funcDef->body);
            } catch (const ReturnException& e) {
                result = static_cast<Value*>(e.getValue());
            }
            
            return result;
        });
    }
}

Value* Interpreter::visit(MethodCallExpression* methodCall) {
    if (!methodCall) return nullptr;
    
    // 获取对象
    Value* object = visit(methodCall->object);
    if (!object) {
        reportError("Cannot call method on null object");
        return nullptr;
    }
    
    // 获取方法名
    string methodName = methodCall->methodName;
    
    // 获取参数
    vector<Value*> args;
    for (Expression* argExpr : methodCall->arguments) {
        Value* arg = visit(argExpr);
        if (arg) {
            args.push_back(arg);
        }
    }
    
    // 检查对象类型并调用相应的方法
    if (Dict* dict = dynamic_cast<Dict*>(object)) {
        // 对于字典对象，查找方法
        Value* method = dict->getEntry(methodName);
        if (method) {
            // 如果找到方法，调用它
            if (FunctionDefinition* func = dynamic_cast<FunctionDefinition*>(method)) {
                // 将对象作为第一个参数传递（类似this指针）
                vector<Value*> methodArgs;
                methodArgs.push_back(object);
                methodArgs.insert(methodArgs.end(), args.begin(), args.end());
                
                return func->executeWithInterpreter(methodArgs, this);
            } else {
                // 如果不是方法，返回该值
                return method;
            }
        } else {
            reportError("Method '" + methodName + "' not found in object");
            return nullptr;
        }
    } else {
        // 对于其他类型的对象，可以添加内置方法支持
        reportError("Method calls not supported for this object type");
        return nullptr;
    }
}

void Interpreter::visit(ReturnStatement* returnStmt) {
    if (!returnStmt) return;
    
    Value* result = nullptr;
    if (returnStmt->returnValue) {
        LOG_DEBUG("Executing return statement with value");
        result = visit(returnStmt->returnValue);
    } else {
        LOG_DEBUG("Executing return statement without value");
    }
    
    // 抛出带有返回值的ReturnException
    throw ReturnException(result);
}


// 执行函数定义
void Interpreter::visit(UserFunction* userFunc) {
    if (!userFunc || !userFunc->prototype) return;
    
    string funcName = userFunc->prototype->name;
    
    scopeManager.defineFunction(funcName, userFunc);
    
    LOG_DEBUG("Registered function '" + funcName + "'");
}

// 导入语句执行
void Interpreter::visit(ImportStatement* importStmt) {
    if (!importStmt || !importStmt->moduleName) return;
    
    string moduleName = importStmt->moduleName->getValue();
    LOG_DEBUG("Importing module: " + moduleName);
    
    // 检查文件是否存在
    ifstream file(moduleName);
    if (!file.is_open()) {
        LOG_ERROR("Error: Cannot open module file '" + moduleName + "'");
        return;
    }
    file.close();
    
    // 创建新的解析器来解析导入的模块
    Parser parser;
    Program* importedProgram = parser.parse(moduleName);
    
    if (!importedProgram) {
        cout << "Error: Failed to parse module '" + moduleName + "'" << endl;
        return;
    }
    
    // 执行导入的模块（在当前作用域中）
    LOG_DEBUG("Executing imported module: " + moduleName);
    execute(importedProgram);
    
    // 清理导入的程序
    delete importedProgram;
    
    LOG_DEBUG("Module import completed: " + moduleName);
}

// 程序执行
void Interpreter::visit(Program* program) {
    if (!program) return;
    
    for (Statement* stmt : program->statements) {
        execute(stmt);
    }
}

// 表达式语句执行
void Interpreter::visit(ExpressionStatement* stmt) {
    if (!stmt || !stmt->expression) return;
    
    visit(stmt->expression);         
}



// 条件语句执行
void Interpreter::visit(IfStatement* ifStmt) {
    if (!ifStmt || !ifStmt->condition) return;
    
    Value* conditionValue = visit(ifStmt->condition);
    
    // 检查条件是否为真（非零值）
    bool conditionBool = false;
    if (Integer* intVal = dynamic_cast<Integer*>(conditionValue)) {
        conditionBool = (intVal->getValue() != 0);
        LOG_DEBUG(std::string("Condition value: ") + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
    } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
        conditionBool = boolVal->getValue();
        LOG_DEBUG(std::string("Condition value: ") + (conditionBool ? "true" : "false"));
    }
    
    if (conditionBool && ifStmt->thenStatement) {
        LOG_DEBUG("Executing then branch");
        // 为then分支创建独立作用域
        withScopeVoid([&]() {
            execute(ifStmt->thenStatement);
        });
    } else if (!conditionBool && ifStmt->elseStatement) {
        LOG_DEBUG("Executing else branch");
        // 为else分支创建独立作用域
        withScopeVoid([&]() {
            execute(ifStmt->elseStatement);
        });
    }
}

// 循环语句执行
void Interpreter::visit(WhileStatement* whileStmt) {
    if (!whileStmt || !whileStmt->condition || !whileStmt->body) return;
    
    while (true) {
        // 评估循环条件
        Value* conditionValue = visit(whileStmt->condition);
        
        // 检查条件是否为真（非零值）
        bool conditionBool = false;
        if (Integer* intVal = dynamic_cast<Integer*>(conditionValue)) {
            conditionBool = (intVal->getValue() != 0);
            LOG_DEBUG(std::string("While condition value: ") + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
        } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
            conditionBool = boolVal->getValue();
            LOG_DEBUG(std::string("While condition value: ") + (conditionBool ? "true" : "false"));
        }
        
        // 如果条件为假，退出循环
        if (!conditionBool) {
            LOG_DEBUG("While condition is false, exiting loop");
            break;
        }
        
        // 执行循环体（无论是单个语句还是语句块，都会抛出相同的异常）
        // 注意：不创建新作用域，让循环体内的变量赋值影响外层作用域
        try {
            if (BlockStatement* block = dynamic_cast<BlockStatement*>(whileStmt->body)) {
                // 如果是块语句，直接执行其中的语句而不创建新作用域
                for (Statement* stmt : block->statements) {
                    execute(stmt);
                }
            } else {
                // 如果是单个语句，正常执行
                execute(whileStmt->body);
            }
        } catch (const BreakException&) {
            return;  // 退出循环
        } catch (const ContinueException&) {
            continue;  // 跳过当前迭代，继续下一次循环
        } catch (const ReturnException&) {
            throw;  // 重新抛出return异常
        }
    }
}

// for循环语句执行
void Interpreter::visit(ForStatement* forStmt) {
    if (!forStmt || !forStmt->condition || !forStmt->body) return;
    
    // 执行初始化表达式
    if (forStmt->initializer) {
        visit(forStmt->initializer);
    }
    
    while (true) {
        // 评估循环条件
        Value* conditionValue = visit(forStmt->condition);
        
        // 检查条件是否为真（非零值）
        bool conditionBool = false;
        if (Integer* intVal = dynamic_cast<Integer*>(conditionValue)) {
            conditionBool = (intVal->getValue() != 0);
            LOG_DEBUG(std::string("For condition value: ") + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
        } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
            conditionBool = boolVal->getValue();
            LOG_DEBUG(std::string("For condition value: ") + (conditionBool ? "true" : "false"));
        }
        
        // 如果条件为假，退出循环
        if (!conditionBool) {
            LOG_DEBUG("For condition is false, exiting loop");
            break;
        }
        
        // 执行循环体
        LOG_DEBUG("Executing for loop body");
        try {
            execute(forStmt->body);
        } catch (const BreakException&) {
            return;  // 退出循环
        } catch (const ContinueException&) {
            // 执行增量表达式，然后继续下一次循环
            if (forStmt->increment) {
                visit(forStmt->increment);
            }
            continue;  // 跳过当前迭代，继续下一次循环
        } catch (const ReturnException&) {
            throw;  // 重新抛出return异常
        }
        
        // 执行增量表达式
        if (forStmt->increment) {
            visit(forStmt->increment);
        }
    }
}

// do-while循环语句执行
void Interpreter::visit(DoWhileStatement* doWhileStmt) {
    if (!doWhileStmt || !doWhileStmt->condition || !doWhileStmt->body) return;
    
    do {
        // 执行循环体
        LOG_DEBUG("Executing do-while loop body");
        try {
            execute(doWhileStmt->body);
        } catch (const BreakException&) {
            return;  // 退出循环
        } catch (const ContinueException&) {
            continue;  // 跳过当前迭代，继续下一次循环
        } catch (const ReturnException&) {
            throw;  // 重新抛出return异常
        }
        
        // 评估循环条件
        Value* conditionValue = visit(doWhileStmt->condition);
        
        // 检查条件是否为真（非零值）
        bool conditionBool = false;
        if (Integer* intVal = dynamic_cast<Integer*>(conditionValue)) {
            conditionBool = (intVal->getValue() != 0);
            LOG_DEBUG(std::string("Do-while condition value: ") + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
        } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
            conditionBool = boolVal->getValue();
            LOG_DEBUG(std::string("Do-while condition value: ") + (conditionBool ? "true" : "false"));
        }
        
        // 如果条件为假，退出循环
        if (!conditionBool) {
            LOG_DEBUG("Do-while condition is false, exiting loop");
            break;
        }
    } while (true);
}

// 语句块执行
void Interpreter::visit(BlockStatement* block) {
    if (!block) return;
    
    withScopeVoid([&]() {
        for (Statement* stmt : block->statements) {
            execute(stmt);
        }
    });
}

// withScope函数已在头文件中定义为模板

// 错误处理
void Interpreter::reportError(const string& message) {
    cerr << "Error: " << message << endl;
}

void Interpreter::reportTypeError(const string& expected, const string& actual) {
    cerr << "Type Error: expected " << expected << ", got " << actual << endl;
}

// 添加缺失的visit方法实现
Value* Interpreter::visit(ConstantExpression* constExpr) {
    if (!constExpr || !constExpr->value) return nullptr;
    LOG_DEBUG("ConstantExpression: " + constExpr->getLocation());
    
    // 直接返回value，因为现在parseConstant已经创建了正确的Value类型
    return constExpr->value;
}

// 类型转换表达式求值
Value* Interpreter::visit(CastExpression* castExpr) {
    if (!castExpr || !castExpr->operand) return nullptr;
    
    LOG_DEBUG("CastExpression: " + castExpr->getLocation());
    
    // 先求值操作数
    Value* operandValue = visit(castExpr->operand);
    if (!operandValue) return nullptr;
    
    // 根据目标类型进行转换
    string targetType = castExpr->getTargetTypeName();
    
    // 确定目标类型
    Type* targetTypeObj = nullptr;
    if (targetType == "int") {
        targetTypeObj = Type::Int;
    } else if (targetType == "double") {
        targetTypeObj = Type::Double;
    } else if (targetType == "bool") {
        targetTypeObj = Type::Bool;
    } else if (targetType == "char") {
        targetTypeObj = Type::Char;
    } else if (targetType == "string") {
        targetTypeObj = Type::String;
    } else {
        reportError("Unknown target type: " + targetType);
        return operandValue;
    }
    
    // 使用convert方法进行类型转换
    try {
        return operandValue->convert(targetTypeObj);
    } catch (const std::exception& e) {
        reportError("Cast error: " + string(e.what()));
        return operandValue;
    }
}

void Interpreter::visit(SwitchStatement* switchStmt) {
    if (!switchStmt) return;
    // 暂时简单实现，后续可以完善
    reportError("Switch statement not implemented yet");
}

void Interpreter::visit(FunctionPrototype* funcProto) {
    if (!funcProto) return;
    // 函数原型不需要执行，只是声明
    LOG_DEBUG("Function prototype: " + funcProto->name);
}

void Interpreter::visit(BreakStatement* breakStmt) {
    if (!breakStmt) return;
    // 暂时简单实现，后续可以完善
    reportError("Break statement not implemented yet");
}

void Interpreter::visit(ContinueStatement* continueStmt) {
    if (!continueStmt) return;
    // 暂时简单实现，后续可以完善
    reportError("Continue statement not implemented yet");
}

void Interpreter::visit(StructDefinition* structDef) {
    if (!structDef) return;
    
    // 将结构体定义注册到作用域中
    scopeManager.defineStruct(structDef->name, structDef);
    
    LOG_DEBUG("Registered struct '" + structDef->name + "' with " + to_string(structDef->members.size()) + " members");
}

void Interpreter::visit(ClassDefinition* classDef) {
    if (!classDef) return;
    
    // 将类定义注册到作用域中
    scopeManager.defineClass(classDef->name, classDef);
    
    LOG_DEBUG("Registered class '" + classDef->name + "' with " + to_string(classDef->members.size()) + " members and " + to_string(classDef->methods.size()) + " methods");
}

void Interpreter::visit(ThrowStatement* throwStmt) {
    if (!throwStmt) return;
    // 暂时简单实现，后续可以完善
    reportError("Throw statement not implemented yet");
}

void Interpreter::visit(TryStatement* tryStmt) {
    if (!tryStmt) return;
    // 暂时简单实现，后续可以完善
    reportError("Try statement not implemented yet");
}

// BuiltinFunction不是AST节点，不需要visit方法

// 注册内置函数到作用域管理器
void Interpreter::registerBuiltinFunctionsToScope() {
    ::registerBuiltinFunctionsToScope(scopeManager);
}

// 内置函数通过visit方法自动执行

// ==================== 缺少的虚函数实现 ====================

// Identifier访问方法
void Interpreter::visit(Identifier* id) {
    if (!id) return;
    // Identifier主要用于类型检查，不需要执行
    LOG_DEBUG("Visiting identifier: " + id->name);
}

// Variable访问方法
void Interpreter::visit(Variable* var) {
    if (!var) return;
    // Variable主要用于类型检查，不需要执行
    LOG_DEBUG("Visiting variable: " + var->name);
}

// ClassMethod访问方法
void Interpreter::visit(ClassMethod* method) {
    if (!method) return;
    // ClassMethod主要用于类型检查，不需要执行
    LOG_DEBUG("Visiting class method: " + method->name);
}

// ==================== 类/结构体实例化方法 ====================

// 类实例化
Value* Interpreter::instantiateClass(ClassDefinition* classDef, vector<Value*>& args) {
    if (!classDef) return nullptr;
    
    // 创建字典来存储实例的成员
    Dict* instance = new Dict();
    
    // 根据参数顺序初始化成员
    const vector<StructMember>& members = classDef->members;
    for (size_t i = 0; i < members.size() && i < args.size(); ++i) {
        const StructMember& member = members[i];
        instance->setEntry(member.name, args[i]);
    }
    
    // 为未初始化的成员设置默认值
    for (size_t i = args.size(); i < members.size(); ++i) {
        const StructMember& member = members[i];
        if (member.defaultValue) {
            // 如果有默认值表达式，求值它
            Value* defaultVal = visit(member.defaultValue);
            instance->setEntry(member.name, defaultVal);
        } else {
            // 否则使用类型的默认值
            Value* defaultVal = createDefaultValue(member.type);
            instance->setEntry(member.name, defaultVal);
        }
    }
    
    return instance;
}

// 结构体实例化
Value* Interpreter::instantiateStruct(StructDefinition* structDef, vector<Value*>& args) {
    if (!structDef) return nullptr;
    
    // 创建字典来存储实例的成员
    Dict* instance = new Dict();
    
    // 根据参数顺序初始化成员
    const vector<StructMember>& members = structDef->members;
    for (size_t i = 0; i < members.size() && i < args.size(); ++i) {
        const StructMember& member = members[i];
        instance->setEntry(member.name, args[i]);
    }
    
    // 为未初始化的成员设置默认值
    for (size_t i = args.size(); i < members.size(); ++i) {
        const StructMember& member = members[i];
        if (member.defaultValue) {
            // 如果有默认值表达式，求值它
            Value* defaultVal = visit(member.defaultValue);
            instance->setEntry(member.name, defaultVal);
        } else {
            // 否则使用类型的默认值
            Value* defaultVal = createDefaultValue(member.type);
            instance->setEntry(member.name, defaultVal);
        }
    }
    

    
    return instance;
}

// 创建默认值
Value* Interpreter::createDefaultValue(Type* type) {
    if (!type) return new Integer(0);
    
    if (type == Type::Int) {
        return new Integer(0);
    } else if (type == Type::Double) {
        return new Double(0.0);
    } else if (type == Type::Char) {
        return new Char('\0');
    } else if (type == Type::Bool) {
        return new Bool(false);
    } else if (type == Type::String) {
        return new String("");
    } else {
        return new Integer(0); // 默认返回整数0
    }
}


