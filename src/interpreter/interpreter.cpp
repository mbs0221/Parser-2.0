#include "interpreter.h"
#include "expression.h"
#include "function.h"
#include "inter.h"
#include "lexer.h"
#include "logger.h"
#include "value.h"
#include "builtin.h"
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

// 作用域管理 - 直接使用scopeManager

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
void Interpreter::visit(Expression* expr) {
    if (!expr) return;
    
    expr->accept(this);
}

// 声明求值 - 语句类型，不需要返回值
void Interpreter::visit(VariableDeclaration* decl) {
    if (!decl) return;
    
    // 处理多个变量声明
    for (const auto& var : decl->variables) {
        string name = var.name;
        Value* value = nullptr;
        
        if (var.initializer) {
            value = visit(var.initializer);
        }
        
        // 如果变量没有初始值，设置为默认值（数字0）
        if (!value) {
            value = new Integer(0);
        }
        
        // 使用Variable类型存储
        scopeManager.defineVariable(name, value);
        
        LOG_DEBUG("Declaring variable '" + name + "' with value " + (value == nullptr ? "null" : value->toString()));
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
    if (!value) {
        reportError("Variable has no value: " + name);
        return nullptr;
    }

    LOG_DEBUG("VariableExpression: " + name + " = " + value->toString());
    
    return value;
}

template<typename T>
void Interpreter::executeCastOperation(CastExpression<T>* cast) {
    if (!cast || !cast->operand) {
        reportError("Invalid cast expression");
        return;
    }
    
    // 后序遍历：先处理操作数
    visit(cast->operand);
    
    // 从栈中pop出操作数
    Expression* operand = popResult();
    if (!operand) {
        reportError("Invalid operand in cast expression");
        return;
    }

    // 如果操作数已经是目标类型，直接返回
    if (typeid(*operand) == typeid(T)) {
        LOG_DEBUG("Operand is already of target type, no conversion needed");
        pushResult(operand);
        return;
    }
    
    // 尝试将操作数转换为LeafExpression，然后调用convert方法
    LeafExpression* leafExpr = dynamic_cast<LeafExpression*>(operand);
    if (leafExpr) {
        // 使用LeafExpression的convert方法进行类型转换
        Expression* converted = leafExpr->convert<T>();
        if (converted) {
            LOG_DEBUG("Successfully converted operand to target type");
            pushResult(converted);
        } else {
            reportError("Failed to convert operand to target type");
        }
    } else {
        // 对于非LeafExpression类型，尝试特殊处理
        if (typeid(T) == typeid(StringLiteral)) {
            // 特殊处理：将任何类型转换为字符串
            StringLiteral* strExpr = dynamic_cast<StringLiteral*>(operand);
            if (strExpr) {
                pushResult(strExpr);
            } else {
                // 对于其他类型，创建默认字符串
                pushResult(new StringLiteral(""));
            }
        } else {
            reportError("Cannot convert non-leaf expression to target type");
        }
    }
}

// 赋值表达式现在使用BinaryExpression处理，在visit(BinaryExpression*)中实现

// 一元表达式求值 - 后序遍历模式
void Interpreter::visit(UnaryExpression* unary) {
    if (!unary || !unary->operand || !unary->operator_) {
        reportError("Invalid unary expression");
        return;
    }
    
    // 后序遍历：先处理操作数
    Value* operand = visit(unary->operand);
    
    // 从栈中pop出操作数
    if (!operand) {
        reportError("Invalid operand in unary expression");
        return nullptr;
    }

    // 如果操作数不是BoolExpression，则转换为BoolExpression
    if (typeid(*operand) != typeid(BoolExpression)) {
        operand = new CastExpression<BoolExpression>(operand);
        operand = visit(operand);
    }
    
    // 根据操作符类型进行处理
    switch (unary->operator_->Tag) {
        case '!': {
            BoolExpression* expr = dynamic_cast<BoolExpression*>(operand);
            if (expr) {
                pushResult(new BoolExpression(!expr->getBool()));
            } else {
                reportError("Invalid operand for logical NOT operator");
            }
            break;
        }
        case '-': {
            IntExpression* intOperand = dynamic_cast<IntExpression*>(operand);
            if (intOperand) {
                pushResult(new IntExpression(-(*intOperand)));
            } else {
                DoubleExpression* doubleOperand = dynamic_cast<DoubleExpression*>(operand);
                if (doubleOperand) {
                    pushResult(new DoubleExpression(-(*doubleOperand)));
                } else {
                    reportError("Invalid operand for unary minus operator");
                }
            }
            break;
        }
        default:
            reportError("Unknown unary operator Tag " + to_string(unary->operator_->Tag));
            break;
    }
}

Expression* Interpreter::insertCastExpression(Expression* target, Expression* source) {
    if (typeid(*target) == typeid(IntExpression)) {
        return new CastExpression<IntExpression>(source);
    } else if (typeid(*target) == typeid(DoubleExpression)) {
        return new CastExpression<DoubleExpression>(source);
    } else if (typeid(*target) == typeid(BoolExpression)) {
        return new CastExpression<BoolExpression>(source);
    } else if (typeid(*target) == typeid(CharExpression)) {
        return new CastExpression<CharExpression>(source);
    } else if (typeid(*target) == typeid(StringLiteral)) {
        return new CastExpression<StringLiteral>(source);
    }
    reportError("Cannot convert expression to target type");
    return source;
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
    if (opTag == '=') {
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
    if (opTag == AND || opTag == OR) {
        return "bool";
    }
    
    // 比较运算：检查特殊类型组合
    if (opTag == EQ || opTag == NE || opTag == '<' || opTag == '>' || opTag == LE || opTag == GE) {
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
        case '+': return new Integer(left->getValue() + right->getValue());
        case '-': return new Integer(left->getValue() - right->getValue());
        case '*': return new Integer(left->getValue() * right->getValue());
        case '/': return new Integer(left->getValue() / right->getValue());
        case '%': return new Integer(left->getValue() % right->getValue());
        default: return new Integer(0);
    }
}

Value* Interpreter::calculate(Double* left, Double* right, int op) {
    switch (op) {
        case '+': return new Double(left->getValue() + right->getValue());
        case '-': return new Double(left->getValue() - right->getValue());
        case '*': return new Double(left->getValue() * right->getValue());
        case '/': return new Double(left->getValue() / right->getValue());
        default: return new Double(0.0);
    }
}

Value* Interpreter::calculate(Bool* left, Bool* right, int op) {
    switch (op) {
        case AND: return new Bool(left->getValue() && right->getValue());
        case OR: return new Bool(left->getValue() || right->getValue());
        default: return new Bool(false);
    }
}

Value* Interpreter::calculate(Char* left, Char* right, int op) {
    switch (op) {
        case '+': return new String(string(1, left->getValue()) + string(1, right->getValue()));
        default: return new Char(left->getValue());
    }
}

Value* Interpreter::calculate(String* left, String* right, int op) {
    switch (op) {
        case '+': return new String(left->getValue() + right->getValue());
        default: return new String(left->getValue());
    }
}

CharExpression* Interpreter::calculate(CharExpression* left, CharExpression* right, int op) {
    switch (op) {
        case '+': return new CharExpression(left->value + right->value);
        default: return new CharExpression(0);
    }
}

DoubleExpression* Interpreter::calculate(DoubleExpression* left, DoubleExpression* right, int op) {
    switch (op) {
        case '+': return new DoubleExpression(*left + *right);
        case '-': return new DoubleExpression(*left - *right);
        case '*': return new DoubleExpression(*left * *right);
        case '/': return new DoubleExpression(*left / *right);
        default: return new DoubleExpression(0);
    }
}

StringLiteral* Interpreter::calculate(StringLiteral* left, StringLiteral* right, int op) {
    switch (op) {
        case '+': return new StringLiteral(*left + *right);
        default: return new StringLiteral("");
    }
}

// 字符表达式求值
void Interpreter::visit(CharExpression* charExpr) {
    if (!charExpr) return;
    LOG_DEBUG("CharExpression: " + to_string(charExpr->value));
    
    // 字符表达式直接推入结果栈
    pushResult(charExpr);
}

// 字符串字面量求值
void Interpreter::visit(StringLiteral* strLit) {
    if (!strLit) return;
    LOG_DEBUG("StringLiteral: " + strLit->toString());

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
            visit(element);
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
            visit(value);
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
    visit(access->target);
    Expression* target = popResult();
    visit(access->key);
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
    if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(call->callee)) {
        string funcName = varExpr->name;
        if (isBuiltinFunction(funcName)) {
            Expression* result = executeBuiltinFunction(funcName, call->arguments);
            pushResult(result);
            return;
        }
    }
    
    // 求值所有参数
    vector<Expression*> evaluatedArgs;
    for (Expression* arg : call->arguments) {
        visit(arg);
        Expression* evaluatedArg = popResult();
        if (evaluatedArg) {
            evaluatedArgs.push_back(evaluatedArg);
        }
    }
    
    // 求值被调用者
    if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(call->callee)) {
        string funcName = varExpr->name;
        
        // 首先检查是否是内置函数
        if (isBuiltinFunction(funcName)) {
            Expression* result = executeBuiltinFunction(funcName, evaluatedArgs);
            pushResult(result);
            return;
        }
        
        // 然后查找用户定义的函数
        UserFunction* funcDef = lookupFunction(funcName);
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
            
            visit(stmt);
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
    visit(returnStmt->returnValue);
    Expression* result = popResult();
    LOG_DEBUG("Return value: " + (result == nullptr ? "null" : result->getLocation()));
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
        LOG_DEBUG("Condition value: " + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
    } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
        conditionBool = boolVal->getValue();
        LOG_DEBUG("Condition value: " + (conditionBool ? "true" : "false"));
    }
    
    if (conditionBool && ifStmt->thenBranch) {
        LOG_DEBUG("Executing then branch");
        // 为then分支创建独立作用域
        scopeManager.enterScope();
        try {
            execute(ifStmt->thenBranch);
        } catch (const ControlFlowException&) {
            scopeManager.exitScope();
            throw;  // 重新抛出控制流异常
        }
        scopeManager.exitScope();  // 退出then分支作用域
    } else if (!conditionBool && ifStmt->elseBranch) {
        LOG_DEBUG("Executing else branch");
        // 为else分支创建独立作用域
        scopeManager.enterScope();
        try {
            execute(ifStmt->elseBranch);
        } catch (const ControlFlowException&) {
            scopeManager.exitScope();
            throw;  // 重新抛出控制流异常
        }
        scopeManager.exitScope();  // 退出else分支作用域
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
            LOG_DEBUG("While condition value: " + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
        } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
            conditionBool = boolVal->getValue();
            LOG_DEBUG("While condition value: " + (conditionBool ? "true" : "false"));
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
            LOG_DEBUG("For condition value: " + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
        } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
            conditionBool = boolVal->getValue();
            LOG_DEBUG("For condition value: " + (conditionBool ? "true" : "false"));
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
            LOG_DEBUG("Do-while condition value: " + to_string(intVal->getValue()) + " (bool: " + (conditionBool ? "true" : "false") + ")");
        } else if (Bool* boolVal = dynamic_cast<Bool*>(conditionValue)) {
            conditionBool = boolVal->getValue();
            LOG_DEBUG("Do-while condition value: " + (conditionBool ? "true" : "false"));
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
    
    scopeManager.enterScope();
    
    try {
        for (Statement* stmt : block->statements) {
            execute(stmt);
        }
    } catch (const ControlFlowException&) {
        scopeManager.exitScope();
        throw;  // 重新抛出控制流异常
    }

    scopeManager.exitScope();  
}

// 错误处理
void Interpreter::reportError(const string& message) {
    cerr << "Error: " << message << endl;
}

void Interpreter::reportTypeError(const string& expected, const string& actual) {
    cerr << "Type Error: expected " << expected << ", got " << actual << endl;
}

// printScope方法现在委托给ScopeManager，在头文件中已实现

void Interpreter::printCallStack() {
    cout << "Call stack:" << endl;
    for (const string& call : callStack) {
        cout << "  " << call << endl;
    }
}

// 结构体实例化求值
Value* Interpreter::visit(StructInstantiationExpression* structInst) {
    if (!structInst) return nullptr;
    
    string structName = structInst->structName->getName();
    
    // 查找结构体定义
    StructDefinition* structDef = scopeManager.lookupStruct(structName);
    if (!structDef) {
        reportError("Undefined struct: " + structName);
        return nullptr;
    }
    
    // 创建一个字典来存储结构体实例
    Dict* instance = new Dict();
    
    // 初始化所有成员为默认值
    for (const auto& member : structDef->members) {
        if (member.defaultValue) {
            Value* defaultValue = visit(member.defaultValue);
            instance->setEntry(member.name, defaultValue);
        } else {
            // 根据类型设置默认值
            if (member.type == "string") {
                instance->setEntry(member.name, new String(""));
            } else if (member.type == "int") {
                instance->setEntry(member.name, new Integer(0));
            } else if (member.type == "double") {
                instance->setEntry(member.name, new Double(0.0));
            } else {
                instance->setEntry(member.name, new String(""));
            }
        }
    }
    
    // 应用提供的字段值
    for (const auto& field : structInst->fieldValues) {
        Value* fieldValue = visit(field.second);
        instance->setEntry(field.first, fieldValue);
    }
    
    return instance;
}

// 类实例化求值
Value* Interpreter::visit(ClassInstantiationExpression* classInst) {
    if (!classInst) return nullptr;
    
    string className = classInst->className->getName();
    
    // 查找类定义
    ClassDefinition* classDef = scopeManager.lookupClass(className);
    if (!classDef) {
        reportError("Undefined class: " + className);
        return nullptr;
    }
    
    // 创建一个字典来存储类实例
    Dict* instance = new Dict();
    
    // 初始化所有公共成员为默认值
    for (const auto& member : classDef->members) {
        if (member.visibility == "public") {
            if (member.defaultValue) {
                Value* defaultValue = visit(member.defaultValue);
                instance->setEntry(member.name, defaultValue);
            } else {
                // 根据类型设置默认值
                if (member.type == "string") {
                    instance->setEntry(member.name, new String(""));
                } else if (member.type == "int" || member.type == "double") {
                    instance->setEntry(member.name, new Integer(0));
                } else {
                    instance->setEntry(member.name, new String(""));
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
                Value* argValue = visit(classInst->arguments[argIndex]);
                instance->setEntry(member.name, argValue);
                argIndex++;
            }
        }
    }
    
    return instance;
}

// 成员访问求值
Value* Interpreter::visit(MemberAccessExpression* memberAccess) {
    if (!memberAccess) return nullptr;
    
    // 求值对象
    Value* object = visit(memberAccess->object);
    if (!object) return nullptr;
    
    string memberName = memberAccess->memberName;
    
    // 检查对象是否是结构体实例（Dict）
    if (Dict* dict = dynamic_cast<Dict*>(object)) {
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

// 注册内置函数到作用域管理器
void Interpreter::registerBuiltinFunctionsToScope() {
    scopeManager.defineIdentifier("print", new BuiltinFunction("print", builtin_print, true));
    scopeManager.defineIdentifier("count", new BuiltinFunction("count", builtin_count, true));
    scopeManager.defineIdentifier("cin", new BuiltinFunction("cin", builtin_cin, true));
}

// 检查是否为内置函数
bool Interpreter::isBuiltinFunction(const string& funcName) {
    Identifier* identifier = scopeManager.lookupIdentifier(funcName);
    if (identifier && identifier->getIdentifierType() == "FunctionDefinition") {
        BuiltinFunction* builtinFunc = dynamic_cast<BuiltinFunction*>(identifier);
        return builtinFunc && builtinFunc->isBuiltin();
    }
    return false;
}

// 执行内置函数
Value* Interpreter::executeBuiltinFunction(const string& funcName, vector<Expression*>& args) {
    Identifier* identifier = scopeManager.lookupIdentifier(funcName);
    if (identifier && identifier->getIdentifierType() == "FunctionDefinition") {
        BuiltinFunction* builtinFunc = dynamic_cast<BuiltinFunction*>(identifier);
        if (builtinFunc && builtinFunc->isBuiltin()) {
            return builtinFunc->execute(args);
        }
    }
    reportError("Builtin function not found: " + funcName);
    return nullptr;
}
