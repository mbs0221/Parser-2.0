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
    
    string name = decl->variableName;
    Value* value = nullptr;
    
    if (decl->initialValue) {
        value = visit(decl->initialValue);
    }
    
    // 如果变量没有初始值，设置为默认值（数字0）
    if (!value) {
        value = new Integer(0);
    }
    
    // 使用Variable类型存储
    scopeManager.defineVariable(name, value);
    
    LOG_DEBUG(std::string("Declaring variable '") + name + "' with value " + (value == nullptr ? "null" : value->toString()));
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
    if (!value) {
        reportError("Variable has no value: " + name);
        return nullptr;
    }

    LOG_DEBUG("VariableExpression: " + name + " = " + value->toString());
    
    return value;
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

    // 根据操作符类型进行处理
    switch (unary->operator_->Tag) {
        case NOT: {
            // 逻辑非操作
            if (Integer* intVal = dynamic_cast<Integer*>(operand)) {
                return new Bool(intVal->getValue() == 0);
            } else if (Double* doubleVal = dynamic_cast<Double*>(operand)) {
                return new Bool(doubleVal->getValue() == 0.0);
            } else if (Bool* boolVal = dynamic_cast<Bool*>(operand)) {
                return new Bool(!boolVal->getValue());
            } else if (String* strVal = dynamic_cast<String*>(operand)) {
                return new Bool(strVal->getValue().empty());
            } else {
                reportError("Invalid operand for logical NOT operator");
                return nullptr;
            }
        }
        case MINUS: {
            // 一元负号操作
            if (Integer* intVal = dynamic_cast<Integer*>(operand)) {
                return new Integer(-intVal->getValue());
            } else if (Double* doubleVal = dynamic_cast<Double*>(operand)) {
                return new Double(-doubleVal->getValue());
            } else {
                reportError("Invalid operand for unary minus operator");
                return nullptr;
            }
        }
        default:
            reportError("Unknown unary operator Tag " + to_string(unary->operator_->Tag));
            return nullptr;
    }
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
    LOG_DEBUG("BinaryExpression left: " + left->toString());
    LOG_DEBUG("BinaryExpression right: " + right->toString());

    if (!left || !right) {
        reportError("Invalid operands in binary expression");
        return nullptr;
    }

    // 2. 获取操作符
    Operator* op = binary->operator_;
    if (!op) {
        reportError("Invalid operator in binary expression");
        return nullptr;
    }
    int opTag = op->Tag;
    LOG_DEBUG("executeBinaryOperation called with operator: " + op->getSymbol());
    
    // 3. 处理赋值操作
    if (opTag == ASSIGN) {
        // 检查左操作数是否为变量引用
        if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(binary->left)) {
            // 更新变量值
            scopeManager.updateVariable(varExpr->name, right);
            LOG_DEBUG("Assigned value " + right->toString() + " to variable '" + varExpr->name + "'");
            return right;  // 赋值表达式返回右操作数的值
        } else {
            reportError("Left side of assignment must be a variable");
            return nullptr;
        }
    }
 
    // 4. 确定兼容类型
    string targetType = determineTargetType(left, right, op);
    if (targetType == "unknown") {
        reportError("Cannot determine compatible type for operation");
        return nullptr;
    }
    
    // 4. 创建CastExpression对左右值进行转换
    ConstantExpression* leftConst = new ConstantExpression(left);
    ConstantExpression* rightConst = new ConstantExpression(right);
    CastExpression* leftCast = new CastExpression(leftConst, targetType);
    CastExpression* rightCast = new CastExpression(rightConst, targetType);
    
    // 5. 执行转换
    Value* convertedLeft = visit(leftCast);
    Value* convertedRight = visit(rightCast);
    
    // 6. 直接计算
    Value* result = nullptr;
    if (targetType == "int") {
        Integer* leftInt = dynamic_cast<Integer*>(convertedLeft);
        Integer* rightInt = dynamic_cast<Integer*>(convertedRight);
        if (leftInt && rightInt) {
            result = calculate(leftInt, rightInt, opTag);
        }
    } else if (targetType == "double") {
        Double* leftDouble = dynamic_cast<Double*>(convertedLeft);
        Double* rightDouble = dynamic_cast<Double*>(convertedRight);
        if (leftDouble && rightDouble) {
            result = calculate(leftDouble, rightDouble, opTag);
        }
    } else if (targetType == "bool") {
        Bool* leftBool = dynamic_cast<Bool*>(convertedLeft);
        Bool* rightBool = dynamic_cast<Bool*>(convertedRight);
        if (leftBool && rightBool) {
            result = calculate(leftBool, rightBool, opTag);
        }
    } else if (targetType == "string") {
        String* leftStr = dynamic_cast<String*>(convertedLeft);
        String* rightStr = dynamic_cast<String*>(convertedRight);
        if (leftStr && rightStr) {
            result = calculate(leftStr, rightStr, opTag);
        }
    }
    
    // 7. 清理临时对象
    delete leftCast;
    delete rightCast;
    delete leftConst;
    delete rightConst;
    
    if (!result) {
        reportError("Type mismatch in binary expression: " + left->toString() + " " + std::string(1, opTag) + " " + right->toString());
    }
    
    return result;
}

// 辅助方法：确定运算的目标类型
string Interpreter::determineTargetType(Value* left, Value* right, Operator* op) {
    if (!op) return "unknown";
    
    int opTag = op->Tag;
    
    // 逻辑运算：统一转换为布尔类型
    if (opTag == AND_AND || opTag == OR_OR || opTag == '&' || opTag == '|') {
        return "bool";
    }
    
    // 比较运算：检查特殊类型组合
    if (opTag == EQ_EQ || opTag == NE_EQ || opTag == LT || opTag == GT || opTag == LE || opTag == GE ||
        opTag == '<' || opTag == '>' || opTag == '=' || opTag == '!') {
        // 字符串比较
        if (dynamic_cast<String*>(left) && dynamic_cast<String*>(right)) {
            return "string";
        }
        // 布尔比较
        if (dynamic_cast<Bool*>(left) && dynamic_cast<Bool*>(right)) {
            return "bool";
        }
    }
    
    // 数值类型转换：优先级 double > int > char > bool
    if (dynamic_cast<Double*>(left) || dynamic_cast<Double*>(right)) {
        return "double";
    } else if (dynamic_cast<Integer*>(left) || dynamic_cast<Integer*>(right)) {
        return "int";
    } else if (dynamic_cast<Char*>(left) || dynamic_cast<Char*>(right)) {
        return "int";  // 字符转换为整数进行运算
    } else if (dynamic_cast<Bool*>(left) || dynamic_cast<Bool*>(right)) {
        return "int";  // 布尔转换为整数进行运算
    }
    
    return "unknown";
}

Value* Interpreter::calculate(Integer* left, Integer* right, int op) {
    switch (op) {
        case PLUS: return new Integer(*left + *right);
        case MINUS: return new Integer(*left - *right);
        case MULTIPLY: return new Integer(*left * *right);
        case DIVIDE: return new Integer(*left / *right);
        case MODULO: return new Integer(*left % *right);
        default: return new Integer(0);
    }
}

Value* Interpreter::calculate(Double* left, Double* right, int op) {
    switch (op) {
        case PLUS: return new Double(*left + *right);
        case MINUS: return new Double(*left - *right);
        case MULTIPLY: return new Double(*left * *right);
        case DIVIDE: return new Double(*left / *right);
        default: return new Double(0.0);
    }
}

Value* Interpreter::calculate(Bool* left, Bool* right, int op) {
    switch (op) {
        case AND_AND: return new Bool(*left && *right);
        case OR_OR: return new Bool(*left || *right);
        default: return new Bool(false);
    }
}

Value* Interpreter::calculate(Char* left, Char* right, int op) {
    switch (op) {
        case PLUS: return new Char(*left + *right);
        default: return new Char(*left);
    }
}

Value* Interpreter::calculate(String* left, String* right, int op) {
    switch (op) {
        case PLUS: return new String(*left + *right);
        default: return new String("");
    }
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
        if (evaluatedArg) {
            evaluatedArgs.push_back(evaluatedArg);
        }
    }
    
    // 使用函数名调用
    string funcName = call->functionName;
    
    // 查找函数（内置函数或用户函数）
    Identifier* identifier = scopeManager.lookupIdentifier(funcName);
    if (!identifier) {
        throw RuntimeException("Function not found: " + funcName);
    }
    
    // 尝试转换为内置函数
    BuiltinFunction* builtinFunc = dynamic_cast<BuiltinFunction*>(identifier);
    if (builtinFunc) {
        LOG_DEBUG("Calling builtin function '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        
        // 将Value*转换为Variable*（内置函数期望的参数类型）
        vector<Variable*> args;
        for (Value* val : evaluatedArgs) {
            // 为内置函数参数创建临时变量，使用val的类型
            Type* argType = val ? val->valueType : nullptr;
            args.push_back(new Variable("", argType, val));
        }
        
        // 直接调用内置函数
        Value* result = builtinFunc->execute(args);
        
        // 清理临时变量
        for (Variable* var : args) {
            delete var;
        }
        
        return result;
    }
    
    // 尝试转换为用户函数
    UserFunction* userFunc = dynamic_cast<UserFunction*>(identifier);
    if (userFunc) {
        LOG_DEBUG("Calling user function '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        
        // 进入新的作用域并执行函数体
        return withScope([&]() -> Value* {
            // 绑定参数到局部变量
            const vector<pair<string, Type*>>& params = userFunc->prototype->parameters;
            for (size_t i = 0; i < params.size() && i < evaluatedArgs.size(); ++i) {
                scopeManager.defineVariable(params[i].first, evaluatedArgs[i]);
                LOG_DEBUG("Bound parameter '" + params[i].first + "'");
            }
            
            // 执行函数体
            Value* result = nullptr;
            try {
                execute(userFunc->body);
            } catch (const ReturnException& e) {
                result = static_cast<Value*>(e.getValue());
            }
            
            return result;
        });
    }
    
    // 如果既不是内置函数也不是用户函数
    throw RuntimeException("Identifier '" + funcName + "' is not a function");
}

Value* Interpreter::visit(MethodCallExpression* methodCall) {
    if (!methodCall) return nullptr;
    
    // 暂时简单实现，后续可以完善
    throw RuntimeException("Method call not implemented yet");
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

// printScope方法现在委托给ScopeManager，在头文件中已实现

// 结构体实例化求值 - 已移除，使用CallExpression替代

// 类实例化求值 - 已移除，使用CallExpression替代

// MemberAccessExpression已合并到AccessExpression中

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
    
    if (targetType == "int") {
        if (Integer* intVal = dynamic_cast<Integer*>(operandValue)) {
            return new Integer(intVal->getValue());
        } else if (Double* doubleVal = dynamic_cast<Double*>(operandValue)) {
            return new Integer((int)doubleVal->getValue());
        } else if (Bool* boolVal = dynamic_cast<Bool*>(operandValue)) {
            return new Integer(boolVal->getValue() ? 1 : 0);
        } else if (Char* charVal = dynamic_cast<Char*>(operandValue)) {
            return new Integer((int)charVal->getValue());
        } else if (String* strVal = dynamic_cast<String*>(operandValue)) {
            // 字符串转整数：尝试解析数字
            try {
                return new Integer(std::stoi(strVal->getValue()));
            } catch (...) {
                return new Integer(0);
            }
        }
    } else if (targetType == "double") {
        if (Integer* intVal = dynamic_cast<Integer*>(operandValue)) {
            return new Double((double)intVal->getValue());
        } else if (Double* doubleVal = dynamic_cast<Double*>(operandValue)) {
            return new Double(doubleVal->getValue());
        } else if (Bool* boolVal = dynamic_cast<Bool*>(operandValue)) {
            return new Double(boolVal->getValue() ? 1.0 : 0.0);
        } else if (Char* charVal = dynamic_cast<Char*>(operandValue)) {
            return new Double((double)charVal->getValue());
        } else if (String* strVal = dynamic_cast<String*>(operandValue)) {
            try {
                return new Double(std::stod(strVal->getValue()));
            } catch (...) {
                return new Double(0.0);
            }
        }
    } else if (targetType == "bool") {
        if (Integer* intVal = dynamic_cast<Integer*>(operandValue)) {
            return new Bool(intVal->getValue() != 0);
        } else if (Double* doubleVal = dynamic_cast<Double*>(operandValue)) {
            return new Bool(doubleVal->getValue() != 0.0);
        } else if (Bool* boolVal = dynamic_cast<Bool*>(operandValue)) {
            return new Bool(boolVal->getValue());
        } else if (Char* charVal = dynamic_cast<Char*>(operandValue)) {
            return new Bool(charVal->getValue() != '\0');
        } else if (String* strVal = dynamic_cast<String*>(operandValue)) {
            return new Bool(!strVal->getValue().empty());
        }
    } else if (targetType == "char") {
        if (Integer* intVal = dynamic_cast<Integer*>(operandValue)) {
            return new Char((char)intVal->getValue());
        } else if (Double* doubleVal = dynamic_cast<Double*>(operandValue)) {
            return new Char((char)doubleVal->getValue());
        } else if (Bool* boolVal = dynamic_cast<Bool*>(operandValue)) {
            return new Char(boolVal->getValue() ? '1' : '0');
        } else if (Char* charVal = dynamic_cast<Char*>(operandValue)) {
            return new Char(charVal->getValue());
        } else if (String* strVal = dynamic_cast<String*>(operandValue)) {
            string str = strVal->getValue();
            return new Char(str.empty() ? '\0' : str[0]);
        }
    } else if (targetType == "string") {
        if (Integer* intVal = dynamic_cast<Integer*>(operandValue)) {
            return new String(std::to_string(intVal->getValue()));
        } else if (Double* doubleVal = dynamic_cast<Double*>(operandValue)) {
            return new String(std::to_string(doubleVal->getValue()));
        } else if (Bool* boolVal = dynamic_cast<Bool*>(operandValue)) {
            return new String(boolVal->getValue() ? "true" : "false");
        } else if (Char* charVal = dynamic_cast<Char*>(operandValue)) {
            return new String(string(1, charVal->getValue()));
        } else if (String* strVal = dynamic_cast<String*>(operandValue)) {
            return new String(strVal->getValue());
        }
    }
    
    // 如果无法转换，返回原值
    reportError("Cannot cast to type: " + targetType);
    return operandValue;
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
    // 暂时简单实现，后续可以完善
    reportError("Struct definition not implemented yet");
}

void Interpreter::visit(ClassDefinition* classDef) {
    if (!classDef) return;
    // 暂时简单实现，后续可以完善
    reportError("Class definition not implemented yet");
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
    scopeManager.defineIdentifier("print", new BuiltinFunction("print", builtin_print));
    scopeManager.defineIdentifier("count", new BuiltinFunction("count", builtin_count));
    scopeManager.defineIdentifier("cin", new BuiltinFunction("cin", builtin_cin));
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
