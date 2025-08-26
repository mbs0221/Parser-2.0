#include "interpreter.h"
#include "inter.h"
#include "lexer.h"
#include "logger.h"
#include <iostream>
#include <sstream>
#include <typeinfo>

using namespace std;

// 解释器构造函数
Interpreter::Interpreter() {
    // 初始化全局作用域
    globalScope = new Scope();
    currentScope = globalScope;
    scopes.push_back(globalScope);
    
    // 初始化完成
    
    // 注册内置函数
    registerBuiltinFunctions();
}

// 解释器析构函数
Interpreter::~Interpreter() {
    // 清理完成
    
    // 清理作用域
    for (auto scope : scopes) {
        if (scope) {
            // 暂时不删除Expression对象，避免重复删除
            // 清理作用域中的变量
            for (auto& pair : scope->variables) {
                if (pair.second) {
                    // delete pair.second;  // 暂时注释掉，避免重复删除
                }
            }
            // 清理作用域中的函数
            for (auto& pair : scope->functions) {
                if (pair.second) {
                    // delete pair.second;  // 暂时注释掉，避免重复删除
                }
            }
            delete scope;
        }
    }
}

// 进入新作用域
void Interpreter::enterScope() {
    Scope* newScope = new Scope();
    scopes.push_back(newScope);
    currentScope = newScope;
}

// 退出当前作用域
void Interpreter::exitScope() {
    if (scopes.size() > 1) {
        LOG_DEBUG("Exiting scope, cleaning up variables");
        
        // 清理当前作用域中的变量
        for (auto& pair : currentScope->variables) {
            LOG_DEBUG("Cleaning up variable: " + pair.first);
            if (pair.second) {
                delete pair.second;
                pair.second = nullptr;
            }
        }
        
        // 清理当前作用域中的函数
        for (auto& pair : currentScope->functions) {
            LOG_DEBUG("Cleaning up function: " + pair.first);
            if (pair.second) {
                delete pair.second;
                pair.second = nullptr;
            }
        }
        
        delete currentScope;
        scopes.pop_back();
        currentScope = scopes.back();
        
        LOG_DEBUG("Scope exited, current scope depth: " + to_string(scopes.size()));
    }
}

// 定义变量
void Interpreter::defineVariable(const string& name, Expression* value) {
    currentScope->variables[name] = value;
}

// 定义函数
void Interpreter::defineFunction(const string& name, FunctionDefinition* func) {
    currentScope->functions[name] = func;
}

// 查找变量
Expression* Interpreter::lookupVariable(const string& name) {
    // 从内层作用域向外层查找
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto scope = *it;
        auto found = scope->variables.find(name);
        if (found != scope->variables.end()) {
            return found->second;
        }
    }
    return nullptr;
}

// 查找函数
FunctionDefinition* Interpreter::lookupFunction(const string& name) {
    // 从内层作用域向外层查找
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto scope = *it;
        auto found = scope->functions.find(name);
        if (found != scope->functions.end()) {
            return found->second;
        }
    }
    return nullptr;
}

// 主求值方法
void Interpreter::visit(AST* node) {
    if (!node) return;
    
    // 使用访问者模式统一分发
    node->accept(this);
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
Expression* Interpreter::evaluate(Expression* expr) {
    if (!expr) return nullptr;
    
    // 清空结果栈
    clearResultStack();
    
    // 使用访问者模式，让表达式自己决定如何被求值
    expr->accept(this);
    
    // 从结果栈获取结果
    return popResult();
}

// 结果栈操作实现
void Interpreter::pushResult(Expression* result) {
    resultStack.push_back(result);
}

Expression* Interpreter::popResult() {
    if (resultStack.empty()) {
        return nullptr;
    }
    Expression* result = resultStack.back();
    resultStack.pop_back();
    return result;
}

Expression* Interpreter::peekResult() {
    if (resultStack.empty()) {
        return nullptr;
    }
    return resultStack.back();
}

void Interpreter::clearResultStack() {
    resultStack.clear();
}

// 声明求值 - 语句类型，不需要pushResult
void Interpreter::visit(VariableDeclaration* decl) {
    if (!decl) return;
    
    // 处理多个变量声明
    for (const auto& var : decl->variables) {
        string name = var.name;
        Expression* value = nullptr;
        
        if (var.initializer) {
            var.initializer->accept(this);
            value = popResult();
        }
        
        // 如果变量没有初始值，设置为默认值（数字0）
        if (!value) {
            value = new IntExpression(0);
        }
        
        defineVariable(name, value);
        LOG_DEBUG("Declaring variable '" + name + "' with value " + (value ? value->getLocation() : "null"));
    }
}

// 标识符表达式求值
void Interpreter::visit(IdentifierExpression* idExpr) {
    if (!idExpr) {
        reportError("Null identifier expression");
        return;
    }
    
    string name = idExpr->getName();
    Expression* value = lookupVariable(name);
    
    if (!value) {
        reportError("Undefined variable: " + name);
        return;
    }
    
    pushResult(value);
}



template<typename T>
void Interpreter::executeCastOperation(CastExpression<T>* cast) {
    if (!cast || !cast->operand) {
        reportError("Invalid cast expression");
        return;
    }
    
    // 后序遍历：先处理操作数
    cast->operand->accept(this);
    
    // 从栈中pop出操作数
    Expression* operand = popResult();
    if (!operand) {
        reportError("Invalid operand in cast expression");
        return;
    }
    
    // 使用模板特化进行类型转换
    Expression* result = performCast<T>(operand);
    pushResult(result);
}

// 类型转换模板函数 - 使用泛型转换方法
template<typename T>
Expression* Interpreter::performCast(Expression* operand) {
    LOG_DEBUG("Performing cast on operand: " + operand->getLocation());
    
    // 直接检查具体类型并进行转换
    if (IntExpression* intExpr = dynamic_cast<IntExpression*>(operand)) {
        if (typeid(T) == typeid(IntExpression)) {
            return new IntExpression(intExpr->value);
        } else if (typeid(T) == typeid(DoubleExpression)) {
            return new DoubleExpression((double)intExpr->value);
        } else if (typeid(T) == typeid(StringLiteral)) {
            return new StringLiteral(std::to_string(intExpr->value));
        } else if (typeid(T) == typeid(CharExpression)) {
            return new CharExpression((char)intExpr->value);
        } else if (typeid(T) == typeid(BoolExpression)) {
            return new BoolExpression(intExpr->value != 0);
        }
    } else if (DoubleExpression* doubleExpr = dynamic_cast<DoubleExpression*>(operand)) {
        if (typeid(T) == typeid(IntExpression)) {
            return new IntExpression((int)doubleExpr->value);
        } else if (typeid(T) == typeid(DoubleExpression)) {
            return new DoubleExpression(doubleExpr->value);
        } else if (typeid(T) == typeid(StringLiteral)) {
            return new StringLiteral(std::to_string(doubleExpr->value));
        } else if (typeid(T) == typeid(CharExpression)) {
            return new CharExpression((char)doubleExpr->value);
        } else if (typeid(T) == typeid(BoolExpression)) {
            return new BoolExpression(doubleExpr->value != 0.0);
        }
    } else if (BoolExpression* boolExpr = dynamic_cast<BoolExpression*>(operand)) {
        if (typeid(T) == typeid(IntExpression)) {
            return new IntExpression(boolExpr->value ? 1 : 0);
        } else if (typeid(T) == typeid(DoubleExpression)) {
            return new DoubleExpression(boolExpr->value ? 1.0 : 0.0);
        } else if (typeid(T) == typeid(StringLiteral)) {
            return new StringLiteral(boolExpr->value ? "true" : "false");
        } else if (typeid(T) == typeid(CharExpression)) {
            return new CharExpression(boolExpr->value ? '1' : '0');
        } else if (typeid(T) == typeid(BoolExpression)) {
            return new BoolExpression(boolExpr->value);
        }
    } else if (CharExpression* charExpr = dynamic_cast<CharExpression*>(operand)) {
        if (typeid(T) == typeid(IntExpression)) {
            return new IntExpression((int)charExpr->value);
        } else if (typeid(T) == typeid(DoubleExpression)) {
            return new DoubleExpression((double)charExpr->value);
        } else if (typeid(T) == typeid(StringLiteral)) {
            return new StringLiteral(std::string(1, charExpr->value));
        } else if (typeid(T) == typeid(CharExpression)) {
            return new CharExpression(charExpr->value);
        } else if (typeid(T) == typeid(BoolExpression)) {
            return new BoolExpression(charExpr->value != 0);
        }
    }
    
    LOG_DEBUG("Cast failed, unsupported type conversion");
    return nullptr;
}

// 赋值表达式求值 - 后序遍历模式
void Interpreter::visit(AssignmentExpression* assign) {     
    if (!assign || !assign->value) {
        reportError("Invalid assignment expression");
        return;
    }
    
    // 后序遍历：先处理右侧表达式
    assign->value->accept(this);
    
    // 从栈中pop出值
    Expression* value = popResult();
    if (!value) {
        reportError("Invalid value in assignment expression");
        return;
    }
    
    // 更新变量值
    defineVariable(assign->variableName, value);
    LOG_DEBUG("Assigned value " + (value ? value->getLocation() : "null") + " to variable '" + assign->variableName + "'");
    
    pushResult(value);
}

// 一元表达式求值 - 后序遍历模式
void Interpreter::visit(UnaryExpression* unary) {
    if (!unary || !unary->operand || !unary->operator_) {
        reportError("Invalid unary expression");
        return;
    }
    
    // 后序遍历：先处理操作数
    unary->operand->accept(this);
    
    // 从栈中pop出操作数
    Expression* operand = popResult();
    if (!operand) {
        reportError("Invalid operand in unary expression");
        return;
    }
    
    // 根据操作符类型进行处理
    switch (unary->operator_->Tag) {
        case '!': {
            // 逻辑非操作
            if (IntExpression* numExpr = dynamic_cast<IntExpression*>(operand)) {
                pushResult(new IntExpression(!(*numExpr)));
                return;
            }
            // 对于非数字类型，转换为布尔值再取反
            bool boolValue = operand->getLocation() != "" && operand->getLocation() != "0";
            pushResult(new IntExpression(boolValue ? 0 : 1));
            return;
        }
        case '-': {
            // 一元负号操作 - 使用运算符重载
            if (IntExpression* numExpr = dynamic_cast<IntExpression*>(operand)) {
                pushResult(new IntExpression(-(*numExpr)));
                return;
            }
            reportError("Unary minus requires numeric operand");
            return;
        }
        default:
            reportError("Unknown unary operator Tag " + to_string(unary->operator_->Tag));
            return;
    }
}

// 二元运算表达式求值 - 后序遍历模式
void Interpreter::visit(BinaryExpression* binary) {
    if (!binary || !binary->left || !binary->right || !binary->operator_) {
        reportError("Invalid binary expression");
        return;
    }
    
    // 后序遍历：先处理左子树
    binary->left->accept(this);
    
    // 再处理右子树
    binary->right->accept(this);
    
    // 从栈中pop出右操作数和左操作数（注意顺序）
    Expression* right = popResult();
    Expression* left = popResult();
    
    if (!left || !right) {
        reportError("Invalid operands in binary expression");
        return;
    }
    
    // 根据操作符类型进行相应的运算
    auto opType = binary->getOperatorType();
    
    switch (opType) {
        case BinaryExpression::OperatorType::ARITHMETIC: {
            executeArithmeticOperation(binary, left, right);
            break;
        }
        
        case BinaryExpression::OperatorType::COMPARISON: {
            executeComparisonOperation(binary, left, right);
            break;
        }
        
        case BinaryExpression::OperatorType::LOGICAL: {
            executeLogicalOperation(binary, left, right);
            break;
        }
    }
}

// 二元运算辅助方法实现

void Interpreter::executeArithmeticOperation(BinaryExpression* binary, Expression* left, Expression* right) {
    LOG_DEBUG("executeArithmeticOperation called with operator: " + std::string(1, binary->operator_->Tag));
    LOG_DEBUG("Left operand type: " + left->getLocation());
    LOG_DEBUG("Right operand type: " + right->getLocation());
    
    // 字符串拼接
    if (binary->operator_->Tag == '+') {
        StringLiteral* leftStr = dynamic_cast<StringLiteral*>(left);
        StringLiteral* rightStr = dynamic_cast<StringLiteral*>(right);
        if (leftStr && rightStr) {
            pushResult(new StringLiteral(*leftStr + *rightStr));
            return;
        }
    }
    
    // 数值运算：自动类型转换
    IntExpression* leftInt = dynamic_cast<IntExpression*>(left);
    IntExpression* rightInt = dynamic_cast<IntExpression*>(right);
    DoubleExpression* leftDouble = dynamic_cast<DoubleExpression*>(left);
    DoubleExpression* rightDouble = dynamic_cast<DoubleExpression*>(right);
    
    LOG_DEBUG("Type check - leftInt: " + std::to_string(leftInt != nullptr) + 
              ", rightInt: " + std::to_string(rightInt != nullptr) + 
              ", leftDouble: " + std::to_string(leftDouble != nullptr) + 
              ", rightDouble: " + std::to_string(rightDouble != nullptr));
    
    // 如果类型不一致，进行转换
    if (leftInt && rightDouble) {
        left = new CastExpression<DoubleExpression>(left);
        left->accept(this);
        left = popResult();
        leftDouble = dynamic_cast<DoubleExpression*>(left);
        leftInt = nullptr;  // 重新设置类型
    } else if (leftDouble && rightInt) {
        right = new CastExpression<DoubleExpression>(right);
        right->accept(this);
        right = popResult();
        rightDouble = dynamic_cast<DoubleExpression*>(right);
        rightInt = nullptr;  // 重新设置类型
    } else if (!leftInt && !leftDouble) {
        left = new CastExpression<IntExpression>(left);
        left->accept(this);
        left = popResult();
        leftInt = dynamic_cast<IntExpression*>(left);
    } else if (!rightInt && !rightDouble) {
        right = new CastExpression<IntExpression>(right);
        right->accept(this);
        right = popResult();
        rightInt = dynamic_cast<IntExpression*>(right);
    }
    
    // 重新检查转换后的类型
    leftInt = dynamic_cast<IntExpression*>(left);
    rightInt = dynamic_cast<IntExpression*>(right);
    leftDouble = dynamic_cast<DoubleExpression*>(left);
    rightDouble = dynamic_cast<DoubleExpression*>(right);
    
    // 执行运算
    if (leftInt && rightInt) {
        try {
            switch (binary->operator_->Tag) {
                case '+': pushResult(new IntExpression(*leftInt + *rightInt)); break;
                case '-': pushResult(new IntExpression(*leftInt - *rightInt)); break;
                case '*': pushResult(new IntExpression(*leftInt * *rightInt)); break;
                case '/': pushResult(new IntExpression(*leftInt / *rightInt)); break;
                case '%': pushResult(new IntExpression(*leftInt % *rightInt)); break;
                default: reportError("Unknown arithmetic operator"); break;
            }
        } catch (const std::exception& e) {
            reportError("Arithmetic operation error: " + std::string(e.what()));
        }
    } else if (leftDouble && rightDouble) {
        try {
            switch (binary->operator_->Tag) {
                case '+': pushResult(new DoubleExpression(*leftDouble + *rightDouble)); break;
                case '-': pushResult(new DoubleExpression(*leftDouble - *rightDouble)); break;
                case '*': pushResult(new DoubleExpression(*leftDouble * *rightDouble)); break;
                case '/': pushResult(new DoubleExpression(*leftDouble / *rightDouble)); break;
                case '%': pushResult(new DoubleExpression(*leftDouble % *rightDouble)); break;
                default: reportError("Unknown arithmetic operator"); break;
            }
        } catch (const std::exception& e) {
            reportError("Arithmetic operation error: " + std::string(e.what()));
        }
    } else {
        reportError("Invalid operands for arithmetic operation");
    }
}

void Interpreter::executeComparisonOperation(BinaryExpression* binary, Expression* left, Expression* right) {
    // 比较运算：自动类型转换
    IntExpression* leftInt = dynamic_cast<IntExpression*>(left);
    IntExpression* rightInt = dynamic_cast<IntExpression*>(right);
    DoubleExpression* leftDouble = dynamic_cast<DoubleExpression*>(left);
    DoubleExpression* rightDouble = dynamic_cast<DoubleExpression*>(right);
    
    // 如果类型不一致，进行转换
    if (leftInt && rightDouble) {
        left = new CastExpression<DoubleExpression>(left);
        left->accept(this);
        left = popResult();
        leftDouble = dynamic_cast<DoubleExpression*>(left);
        leftInt = nullptr;
    } else if (leftDouble && rightInt) {
        right = new CastExpression<DoubleExpression>(right);
        right->accept(this);
        right = popResult();
        rightDouble = dynamic_cast<DoubleExpression*>(right);
        rightInt = nullptr;
    } else if (!leftInt && !leftDouble) {
        left = new CastExpression<IntExpression>(left);
        left->accept(this);
        left = popResult();
        leftInt = dynamic_cast<IntExpression*>(left);
    } else if (!rightInt && !rightDouble) {
        right = new CastExpression<IntExpression>(right);
        right->accept(this);
        right = popResult();
        rightInt = dynamic_cast<IntExpression*>(right);
    }
    
    // 执行比较运算
    if (leftInt && rightInt) {
        switch (binary->operator_->Tag) {
            case EQ: pushResult(new IntExpression(*leftInt == *rightInt)); break;
            case NE: pushResult(new IntExpression(*leftInt != *rightInt)); break;
            case '<': pushResult(new IntExpression(*leftInt < *rightInt)); break;
            case '>': pushResult(new IntExpression(*leftInt > *rightInt)); break;
            case BE: pushResult(new IntExpression(*leftInt <= *rightInt)); break;
            case GE: pushResult(new IntExpression(*leftInt >= *rightInt)); break;
            default: reportError("Unknown comparison operator"); break;
        }
    } else if (leftDouble && rightDouble) {
        switch (binary->operator_->Tag) {
            case EQ: pushResult(new IntExpression(*leftDouble == *rightDouble)); break;
            case NE: pushResult(new IntExpression(*leftDouble != *rightDouble)); break;
            case '<': pushResult(new IntExpression(*leftDouble < *rightDouble)); break;
            case '>': pushResult(new IntExpression(*leftDouble > *rightDouble)); break;
            case BE: pushResult(new IntExpression(*leftDouble <= *rightDouble)); break;
            case GE: pushResult(new IntExpression(*leftDouble >= *rightDouble)); break;
            default: reportError("Unknown comparison operator"); break;
        }
    } else {
        reportError("Invalid operands for comparison operation");
    }
}

void Interpreter::executeLogicalOperation(BinaryExpression* binary, Expression* left, Expression* right) {
    // 逻辑运算：自动类型转换
    BoolExpression* leftBool = dynamic_cast<BoolExpression*>(left);
    BoolExpression* rightBool = dynamic_cast<BoolExpression*>(right);
    
    // 如果类型不一致，进行转换
    if (!leftBool) {
        left = new CastExpression<BoolExpression>(left);
        left->accept(this);
        left = popResult();
        leftBool = dynamic_cast<BoolExpression*>(left);
    }
    if (!rightBool) {
        right = new CastExpression<BoolExpression>(right);
        right->accept(this);
        right = popResult();
        rightBool = dynamic_cast<BoolExpression*>(right);
    }
    
    // 执行逻辑运算
    if (leftBool && rightBool) {
        switch (binary->operator_->Tag) {
            case AND: pushResult(new BoolExpression(*leftBool && *rightBool)); break;
            case OR: pushResult(new BoolExpression(*leftBool || *rightBool)); break;
            default: reportError("Unknown logical operator"); break;
        }
    } else {
        reportError("Invalid operands for logical operation");
    }
}

// 字符表达式求值
void Interpreter::visit(CharExpression* charExpr) {
    if (!charExpr) return;
    
    // 字符表达式直接推入结果栈
    pushResult(charExpr);
}

// 字符串字面量求值
void Interpreter::visit(StringLiteral* strLit) {
    if (!strLit) return;
    
    // 字符串字面量直接推入结果栈
    pushResult(strLit);
}

// 数组节点求值
void Interpreter::visit(ArrayNode* array) {
    if (!array) return;
    
    // 对数组中的每个元素进行求值
    for (size_t i = 0; i < array->getElementCount(); ++i) {
        Expression* element = array->getElement(i);
        if (element) {
            element->accept(this);
            Expression* evaluated = popResult();
            if (evaluated) {
                array->setElement(i, evaluated);
            }
        }
    }
    
    pushResult(array);
}

// 字典节点求值
void Interpreter::visit(DictNode* dict) {
    if (!dict) return;
    
    // 对字典中的每个值进行求值
    vector<string> keys = dict->getKeys();
    for (const string& key : keys) {
        Expression* value = dict->getEntry(key);
        if (value) {
            value->accept(this);
            Expression* evaluated = popResult();
            if (evaluated) {
                dict->setEntry(key, evaluated);
            }
        }
    }
    
    pushResult(dict);
}



// 访问表达式求值 - 使用运行时类型检查消除dynamic_cast
void Interpreter::visit(AccessExpression* access) {
    if (!access || !access->target || !access->key) return;
    
    // 求值目标和键
    evaluate(access->target);
    Expression* target = popResult();
    evaluate(access->key);
    Expression* key = popResult();
    
    if (!target || !key) return;
    
    // 检查目标是否为支持访问操作的类型
    AccessibleExpression* accessibleTarget = dynamic_cast<AccessibleExpression*>(target);
    if (!accessibleTarget) {
        reportError("Target expression does not support access operations");
        return;
    }
    
    // 直接调用access方法，无需进一步的dynamic_cast
    Expression* result = accessibleTarget->access(key);
    pushResult(result);
}

// 函数调用表达式求值
void Interpreter::visit(CallExpression* call) {
    if (!call || !call->callee) return;
    
    // 检查是否是内置函数
    if (IdentifierExpression* idExpr = dynamic_cast<IdentifierExpression*>(call->callee)) {
        string funcName = idExpr->getName();
        if (isBuiltinFunction(funcName)) {
            Expression* result = executeBuiltinFunction(funcName, call->arguments);
            pushResult(result);
            return;
        }
    }
    
    // 求值所有参数
    vector<Expression*> evaluatedArgs;
    for (Expression* arg : call->arguments) {
        arg->accept(this);
        Expression* evaluatedArg = popResult();
        if (evaluatedArg) {
            evaluatedArgs.push_back(evaluatedArg);
        }
    }
    
    // 求值被调用者
    if (IdentifierExpression* idExpr = dynamic_cast<IdentifierExpression*>(call->callee)) {
        string funcName = idExpr->getName();
        
        // 首先检查是否是内置函数
        if (isBuiltinFunction(funcName)) {
            Expression* result = executeBuiltinFunction(funcName, evaluatedArgs);
            pushResult(result);
            return;
        }
        
        // 然后查找用户定义的函数
        FunctionDefinition* funcDef = lookupFunction(funcName);
        if (!funcDef) {
            cout << "Error: Function not found: " << funcName << endl;
            return;
        }
        
        LOG_DEBUG("Calling function '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        
        // 进入新的作用域
        enterScope();
        
        // 绑定参数到局部变量
        const vector<string>& params = funcDef->prototype->parameters;
        for (size_t i = 0; i < params.size() && i < evaluatedArgs.size(); ++i) {
            Expression* paramValue = evaluatedArgs[i];
            if (paramValue) {
                defineVariable(params[i], paramValue);
                LOG_DEBUG("Bound parameter '" + params[i] + "'");
            }
        }
        
        // 执行函数体
        Expression* result = nullptr;
        LOG_DEBUG("Executing function body with " + to_string(funcDef->body->statements.size()) + " statements");
        for (size_t i = 0; i < funcDef->body->statements.size(); ++i) {
            Statement* stmt = funcDef->body->statements[i];
            LOG_DEBUG("Executing statement " + to_string(i) + ": " + (stmt ? typeid(*stmt).name() : "null"));
            
            stmt->accept(this);
            // 检查是否有返回值
            if (!resultStack.empty()) {
                result = popResult();
                break;
            }
        }
        
        // 退出作用域
        exitScope();
        
        pushResult(result);
        return;
    }
    
    cout << "Error: Function not found or not callable" << endl;
}

void Interpreter::visit(MethodCallExpression* methodCall) {
    if (!methodCall) return;
    
    // 暂时简单实现，后续可以完善
    reportError("Method call not implemented yet");
}

void Interpreter::visit(ReturnStatement* returnStmt) {
    if (!returnStmt || !returnStmt->returnValue) return;
    LOG_DEBUG("Executing return statement");
    Expression* result = evaluate(returnStmt->returnValue);
    LOG_DEBUG("Return value: " + (result ? result->getLocation() : "null"));
    pushResult(result);
}



// 执行函数定义
void Interpreter::visit(FunctionDefinition* funcDef) {
    if (!funcDef || !funcDef->prototype) return;
    
    string funcName = funcDef->prototype->name;
    
    // 先注册函数原型，创建一个前向声明用于递归调用
    FunctionDefinition* forwardDecl = new FunctionDefinition(funcDef->prototype, nullptr);
    defineFunction(funcName, forwardDecl);
    
    // 立即替换为完整的函数定义
    defineFunction(funcName, funcDef);
    
    LOG_DEBUG("Registered function '" + funcName + "'");        
    
    // 清理前向声明
    delete forwardDecl;
}

// 导入语句执行
void Interpreter::visit(ImportStatement* importStmt) {
    if (!importStmt) return;
    
    string moduleName = importStmt->moduleName;
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
    
    evaluate(stmt->expression);         
}



// 条件语句执行
void Interpreter::visit(IfStatement* ifStmt) {
    if (!ifStmt || !ifStmt->condition) return;
    
    Expression* condition = evaluate(ifStmt->condition);
    
    // 检查条件是否为真（非零值）
    bool conditionValue = false;
    if (IntExpression* numExpr = dynamic_cast<IntExpression*>(condition)) {
        conditionValue = (numExpr->getIntValue() != 0);
        LOG_DEBUG("Condition value: " + to_string(numExpr->getIntValue()) + " (bool: " + (conditionValue ? "true" : "false") + ")");
    }
    
    if (conditionValue && ifStmt->thenBranch) {
        LOG_DEBUG("Executing then branch");
        // 为then分支创建独立作用域
        enterScope();
        try {
            execute(ifStmt->thenBranch);
        } catch (const ControlFlowException&) {
            exitScope();
            throw;  // 重新抛出控制流异常
        }
        exitScope();  // 退出then分支作用域
    } else if (!conditionValue && ifStmt->elseBranch) {
        LOG_DEBUG("Executing else branch");
        // 为else分支创建独立作用域
        enterScope();
        try {
            execute(ifStmt->elseBranch);
        } catch (const ControlFlowException&) {
            exitScope();
            throw;  // 重新抛出控制流异常
        }
        exitScope();  // 退出else分支作用域
    }
}

// 循环语句执行
void Interpreter::visit(WhileStatement* whileStmt) {
    if (!whileStmt || !whileStmt->condition || !whileStmt->body) return;
    
    while (true) {
        // 评估循环条件
        Expression* condition = evaluate(whileStmt->condition);
        
        // 检查条件是否为真（非零值）
        bool conditionValue = false;
        if (IntExpression* numExpr = dynamic_cast<IntExpression*>(condition)) {
            conditionValue = (numExpr->getIntValue() != 0);
            LOG_DEBUG("While condition value: " + to_string(numExpr->getIntValue()) + " (bool: " + (conditionValue ? "true" : "false") + ")");
        }
        
        // 如果条件为假，退出循环
        if (!conditionValue) {
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
        evaluate(forStmt->initializer);
    }
    
    while (true) {
        // 评估循环条件
        Expression* condition = evaluate(forStmt->condition);
        
        // 检查条件是否为真（非零值）
        bool conditionValue = false;
        if (IntExpression* numExpr = dynamic_cast<IntExpression*>(condition)) {
            conditionValue = (numExpr->getIntValue() != 0);
            LOG_DEBUG("For condition value: " + to_string(numExpr->getIntValue()) + " (bool: " + (conditionValue ? "true" : "false") + ")");
        }
        
        // 如果条件为假，退出循环
        if (!conditionValue) {
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
                evaluate(forStmt->increment);
            }
            continue;  // 跳过当前迭代，继续下一次循环
        } catch (const ReturnException&) {
            throw;  // 重新抛出return异常
        }
        
        // 执行增量表达式
        if (forStmt->increment) {
            evaluate(forStmt->increment);
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
        Expression* condition = evaluate(doWhileStmt->condition);
        
        // 检查条件是否为真（非零值）
        bool conditionValue = false;
        if (IntExpression* numExpr = dynamic_cast<IntExpression*>(condition)) {
            conditionValue = (numExpr->getIntValue() != 0);
            LOG_DEBUG("Do-while condition value: " + to_string(numExpr->getIntValue()) + " (bool: " + (conditionValue ? "true" : "false") + ")");
        }
        
        // 如果条件为假，退出循环
        if (!conditionValue) {
            LOG_DEBUG("Do-while condition is false, exiting loop");
            break;
        }
    } while (true);
}

// 语句块执行
void Interpreter::visit(BlockStatement* block) {
    if (!block) return;
    
    enterScope();
    
    try {
        for (Statement* stmt : block->statements) {
            execute(stmt);
        }
    } catch (const ControlFlowException&) {
        exitScope();
        throw;  // 重新抛出控制流异常
    }

    exitScope();  
}

// 注册内置函数
void Interpreter::registerBuiltinFunctions() {
    // 注册到内置函数映射中
    builtinFunctions["print"] = [this](vector<Expression*>& args) -> Expression* {
        return executePrint(args);
    };
    
    builtinFunctions["count"] = [this](vector<Expression*>& args) -> Expression* {
        return executeCount(args);
    };
    
    builtinFunctions["cin"] = [this](vector<Expression*>& args) -> Expression* {
        return executeCin(args);
    };

    // 将内置函数作为全局符号注册到全局作用域
    if (globalScope) {
        // 创建内置函数标识符并注册到全局作用域
        for (const auto& pair : builtinFunctions) {
            const string& funcName = pair.first;
            globalScope->variables[funcName] = new IdentifierExpression(funcName);
        }
    }
}

// 内置函数实现
Expression* Interpreter::executePrint(vector<Expression*>& args) {
    for (Expression* arg : args) {
        Expression* evaluated = evaluate(arg);
        if (evaluated) {
            if (LeafExpression* leaf = dynamic_cast<LeafExpression*>(evaluated)) {
                cout << leaf->toString();
            } else {
                cout << "Uncalculated expression: " << evaluated->getLocation() << endl;
            }
        }
    }

    return nullptr;
}

Expression* Interpreter::executeCount(vector<Expression*>& args) {
    if (args.empty()) {
        reportError("count() requires at least one argument");
        return nullptr;
    }
    
    Expression* arg = evaluate(args[0]);
    if (!arg) {
        reportError("count() argument evaluation failed");
        return nullptr;
    }

    // 如果是数组，返回数组长度
    if (ArrayNode* array = dynamic_cast<ArrayNode*>(arg)) {
        return new IntExpression((int)array->getElementCount());
    }

    // 如果是字符串，返回字符串长度
    if (StringLiteral* str = dynamic_cast<StringLiteral*>(arg)) {
        return new IntExpression((int)str->length());
    }

    // 如果是字典，返回字典的键值对数量
    if (DictNode* dict = dynamic_cast<DictNode*>(arg)) {
        return new IntExpression(dict->getEntryCount());
    }
    
    // 对于其他类型，返回1（表示单个元素）
    return new IntExpression(1);
}

Expression* Interpreter::executeCin(vector<Expression*>& args) {
    string input;
    cin >> input;
    return new StringLiteral(input);
}

// 错误处理
void Interpreter::reportError(const string& message) {
    cerr << "Error: " << message << endl;
}

void Interpreter::reportTypeError(const string& expected, const string& actual) {
    cerr << "Type Error: expected " << expected << ", got " << actual << endl;
}

// 调试方法
void Interpreter::printScope() {
    cout << "Current scope variables:" << endl;
    for (const auto& pair : currentScope->variables) {
        cout << "  " << pair.first << "(uncalculated)" << endl;
    }
    cout << "Current scope functions:" << endl;
    for (const auto& pair : currentScope->functions) {
        cout << "  " << pair.first << "()" << endl;
    }
}

void Interpreter::printCallStack() {
    cout << "Call stack:" << endl;
    for (const string& call : callStack) {
        cout << "  " << call << endl;
    }
}

// 内置函数检查器实现
bool Interpreter::isBuiltinFunction(const string& funcName) {
    for (auto func : builtinFunctions) {
        if (func.first == funcName) {
            return true;
        }
    }
    return false;
}

Expression* Interpreter::executeBuiltinFunction(const string& funcName, vector<Expression*>& args) {
    for (auto func : builtinFunctions) {
        if (func.first == funcName) {
            return func.second(args);
        }
    }
    return nullptr;
}

// 注册结构体定义
void Interpreter::registerStructDefinition(StructDefinition* structDef) {
    if (!structDef) return;
    
    string structName = structDef->name;
    structDefinitions[structName] = structDef;
    LOG_DEBUG("Registered struct definition: " + structName);
}

// 注册类定义
void Interpreter::registerClassDefinition(ClassDefinition* classDef) {
    if (!classDef) return;
    
    string className = classDef->name;
    classDefinitions[className] = classDef;
    LOG_DEBUG("Registered class definition: " + className);
}

// 结构体实例化求值
void Interpreter::visit(StructInstantiationExpression* structInst) {
    if (!structInst) return;
    
    string structName = structInst->structName->getName();
    
    // 查找结构体定义
    auto it = structDefinitions.find(structName);
    if (it == structDefinitions.end()) {
        reportError("Undefined struct: " + structName);
        return;
    }
    
    StructDefinition* structDef = it->second;
    
    // 创建一个字典来存储结构体实例
    DictNode* instance = new DictNode();
    
    // 初始化所有成员为默认值
    for (const auto& member : structDef->members) {
        if (member.defaultValue) {
            member.defaultValue->accept(this);
            Expression* defaultValue = popResult();
            instance->setEntry(member.name, defaultValue);
        } else {
            // 根据类型设置默认值
            if (member.type == "string") {
                instance->setEntry(member.name, new StringLiteral(""));
            } else if (member.type == "int") {
                instance->setEntry(member.name, new IntExpression(0));
            } else if (member.type == "double") {
                instance->setEntry(member.name, new DoubleExpression(0));
            } else {
                instance->setEntry(member.name, new StringLiteral(""));
            }
        }
    }
    
    // 应用提供的字段值
    for (const auto& field : structInst->fieldValues) {
        field.second->accept(this);
        Expression* fieldValue = popResult();
        instance->setEntry(field.first, fieldValue);
    }
    
    pushResult(instance);
}

// 类实例化求值
void Interpreter::visit(ClassInstantiationExpression* classInst) {
    if (!classInst) return;
    
    string className = classInst->className->getName();
    
    // 查找类定义
    auto it = classDefinitions.find(className);
    if (it == classDefinitions.end()) {
        reportError("Undefined class: " + className);
        return;
    }
    
    ClassDefinition* classDef = it->second;
    
    // 创建一个字典来存储类实例
    DictNode* instance = new DictNode();
    
    // 初始化所有公共成员为默认值
    for (const auto& member : classDef->members) {
        if (member.visibility == "public") {
            if (member.defaultValue) {
                member.defaultValue->accept(this);
                Expression* defaultValue = popResult();
                instance->setEntry(member.name, defaultValue);
            } else {
                // 根据类型设置默认值
                if (member.type == "string") {
                    instance->setEntry(member.name, new StringLiteral(""));
                } else if (member.type == "int" || member.type == "double") {
                    instance->setEntry(member.name, new IntExpression(0));
                } else {
                    instance->setEntry(member.name, new StringLiteral(""));
                }
            }
        }
    }
    
    // 如果有构造函数，调用构造函数
    if (!classInst->arguments.empty()) {
        // 这里可以添加构造函数调用的逻辑
        // 暂时简单地将参数赋值给前几个公共成员
        size_t argIndex = 0;
        for (const auto& member : classDef->members) {
            if (member.visibility == "public" && argIndex < classInst->arguments.size()) {
                classInst->arguments[argIndex]->accept(this);
                Expression* argValue = popResult();
                instance->setEntry(member.name, argValue);
                argIndex++;
            }
        }
    }
    
    pushResult(instance);
}

// 成员访问求值
void Interpreter::visit(MemberAccessExpression* memberAccess) {
    if (!memberAccess) return;
    
    // 求值对象
    memberAccess->object->accept(this);
    Expression* object = popResult();
    if (!object) return;
    
    string memberName = memberAccess->memberName;
    
    // 检查对象是否是结构体实例（DictNode）
    if (DictNode* dict = dynamic_cast<DictNode*>(object)) {
        Expression* member = dict->getEntry(memberName);
        if (member) {
            pushResult(member);
        } else {
            reportError("Member '" + memberName + "' not found in struct instance");
            return;
        }
    } else {
        reportError("Cannot access member '" + memberName + "' on non-struct object");
    }
}

// 添加缺失的visit方法实现
void Interpreter::visit(IntExpression* intExpr) {
    if (!intExpr) return;
    pushResult(intExpr);
}

void Interpreter::visit(DoubleExpression* doubleExpr) {
    if (!doubleExpr) return;
    pushResult(doubleExpr);
}

void Interpreter::visit(BoolExpression* boolExpr) {
    if (!boolExpr) return;
    pushResult(boolExpr);
}

void Interpreter::visit(SwitchStatement* switchStmt) {
    if (!switchStmt) return;
    // 暂时简单实现，后续可以完善
    reportError("Switch statement not implemented yet");
}

void Interpreter::visit(CaseStatement* caseStmt) {
    if (!caseStmt) return;
    // 暂时简单实现，后续可以完善
    reportError("Case statement not implemented yet");
}

void Interpreter::visit(DefaultStatement* defaultStmt) {
    if (!defaultStmt) return;
    // 暂时简单实现，后续可以完善
    reportError("Default statement not implemented yet");
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

// CastExpression访问方法实现
void Interpreter::visit(CastExpression<IntExpression>* expr) {
    if (!expr) return;
    // 执行类型转换操作
    executeCastOperation<IntExpression>(expr);
}

void Interpreter::visit(CastExpression<DoubleExpression>* expr) {
    if (!expr) return;
    // 执行类型转换操作
    executeCastOperation<DoubleExpression>(expr);
}

void Interpreter::visit(CastExpression<CharExpression>* expr) {
    if (!expr) return;
    // 执行类型转换操作
    executeCastOperation<CharExpression>(expr);
}

void Interpreter::visit(CastExpression<BoolExpression>* expr) {
    if (!expr) return;
    // 执行类型转换操作
    executeCastOperation<BoolExpression>(expr);
}

void Interpreter::visit(StructDefinition* structDef) {
    if (!structDef) return;
    registerStructDefinition(structDef);
}

void Interpreter::visit(ClassDefinition* classDef) {
    if (!classDef) return;
    registerClassDefinition(classDef);
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

void Interpreter::visit(CatchStatement* catchStmt) {
    if (!catchStmt) return;
    // 暂时简单实现，后续可以完善
    reportError("Catch statement not implemented yet");
}

void Interpreter::visit(FinallyStatement* finallyStmt) {
    if (!finallyStmt) return;
    // 暂时简单实现，后续可以完善
    reportError("Finally statement not implemented yet");
}
