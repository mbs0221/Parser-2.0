#include "interpreter.h"
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
    
    // 注册内置函数
    registerBuiltinFunctions();
}

// 解释器析构函数
Interpreter::~Interpreter() {
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
Expression* Interpreter::evaluate(AST* node) {
    if (!node) return nullptr;
    
    // 根据节点类型调用相应的求值方法
    switch (node->getType()) {
        case AST::Type::DECLARATION: // 初始化变量
            return evaluateDeclaration(static_cast<VariableDeclaration*>(node));
        case AST::Type::EXPRESSION: // 表达式求值
            return evaluate(static_cast<Expression*>(node));
        case AST::Type::STATEMENT: // 执行语句
            execute(static_cast<Statement*>(node));
            return nullptr;
        default:
            reportError("Unknown AST node type");
            return nullptr;
    }
}

// 求值表达式
Expression* Interpreter::evaluate(Expression* expr) {
    if (!expr) return nullptr;
    
    // 根据表达式类型进行求值
    if (NumberExpression* numExpr = dynamic_cast<NumberExpression*>(expr)) {
        return new NumberExpression(numExpr->getIntValue());  // 创建副本避免重复删除
    } else if (IdentifierExpression* idExpr = dynamic_cast<IdentifierExpression*>(expr)) {
        return evaluateIdentifierExpression(idExpr);
    } else if (UnaryExpression* unary = dynamic_cast<UnaryExpression*>(expr)) {
        return evaluateUnaryExpression(unary);
    } else if (ArithmeticExpression* arith = dynamic_cast<ArithmeticExpression*>(expr)) {
        return evaluateArithmeticExpression(arith);
    } else if (StringLiteral* strLit = dynamic_cast<StringLiteral*>(expr)) {
        return evaluateStringLiteral(strLit);
    } else if (StringConcatenationExpression* concat = dynamic_cast<StringConcatenationExpression*>(expr)) {
        return evaluateStringConcatenationExpression(concat);
    } else if (ArrayNode* array = dynamic_cast<ArrayNode*>(expr)) {
        return evaluateArrayNode(array);
    } else if (DictNode* dict = dynamic_cast<DictNode*>(expr)) {
        return evaluateDictNode(dict);
    } else if (StringNode* strNode = dynamic_cast<StringNode*>(expr)) {
        return evaluateStringNode(strNode);
    } else if (AccessExpression* access = dynamic_cast<AccessExpression*>(expr)) {
        return evaluateAccessExpression(access);
    } else if (CallExpression* call = dynamic_cast<CallExpression*>(expr)) {
        return evaluateCallExpression(call);
    } else if (AssignmentExpression* assign = dynamic_cast<AssignmentExpression*>(expr)) {
        return evaluateAssignmentExpression(assign);
    } else if (CharExpression* charExpr = dynamic_cast<CharExpression*>(expr)) {
        return evaluateCharExpression(charExpr);
    } else if (StructInstantiationExpression* structInst = dynamic_cast<StructInstantiationExpression*>(expr)) {
        return evaluateStructInstantiation(structInst);
    } else if (MemberAccessExpression* memberAccess = dynamic_cast<MemberAccessExpression*>(expr)) {
        return evaluateMemberAccess(memberAccess);
    }
    
    return nullptr;
}

// 声明求值
Expression* Interpreter::evaluateDeclaration(VariableDeclaration* decl) {
    if (!decl) return nullptr;
    
    // 处理多个变量声明
    for (const auto& var : decl->variables) {
        string name = var.name;
        Expression* value = nullptr;
        
        if (var.initializer) {
            value = evaluate(var.initializer);
        }
        
        // 如果变量没有初始值，设置为默认值（数字0）
        if (!value) {
            value = new NumberExpression(0);
        }
        
        defineVariable(name, value);
        LOG_DEBUG("Declaring variable '" + name + "' with value " + (value ? value->toString() : "null"));
    }
    
    return nullptr;
}





// 标识符表达式求值
Expression* Interpreter::evaluateIdentifierExpression(IdentifierExpression* idExpr) {
    if (!idExpr) return nullptr;
    
    string name = idExpr->getName();
    Expression* value = lookupVariable(name);
    
    if (!value) {
        reportError("Undefined variable: " + name);
        return nullptr;
    }
    
    return value;
}

// 赋值表达式求值
Expression* Interpreter::evaluateAssignmentExpression(AssignmentExpression* assign) {
    if (!assign || !assign->value) return nullptr;
    
    // 求值右侧表达式
    Expression* value = evaluate(assign->value);
    if (!value) return nullptr;
    
    // 更新变量值
    defineVariable(assign->variableName, value);
    LOG_DEBUG("Assigned value " + (value ? value->toString() : "null") + " to variable '" + assign->variableName + "'");
    
    return value;
}

// 一元表达式求值
Expression* Interpreter::evaluateUnaryExpression(UnaryExpression* unary) {
    if (!unary || !unary->operand || !unary->operator_) return nullptr;
    
    // 求值操作数
    Expression* operand = evaluate(unary->operand);
    if (!operand) return nullptr;
    
    // 根据操作符类型进行处理
    switch (unary->operator_->Tag) {
        case '!': {
            // 逻辑非操作
            if (NumberExpression* numExpr = dynamic_cast<NumberExpression*>(operand)) {
                return new NumberExpression(!(*numExpr));
            } else {
                // 对于非数字类型，转换为布尔值再取反
                bool boolValue = operand->toString() != "" && operand->toString() != "0";
                return new NumberExpression(boolValue ? 0 : 1);
            }
        }
        default:
            cout << "Error: Unknown unary operator Tag " << unary->operator_->Tag << endl;
            return nullptr;
    }
}

// 算术表达式求值
Expression* Interpreter::evaluateArithmeticExpression(ArithmeticExpression* arith) {
    if (!arith || !arith->left || !arith->right || !arith->operator_) return nullptr;
    
    // 求值左右操作数
    Expression* left = evaluate(arith->left);
    Expression* right = evaluate(arith->right);
    
    if (!left || !right) return nullptr;
    
    // 尝试转换为NumberExpression进行运算
    NumberExpression* leftNum = dynamic_cast<NumberExpression*>(left);
    NumberExpression* rightNum = dynamic_cast<NumberExpression*>(right);
    
    if (leftNum && rightNum) {
        try {
            switch (arith->operator_->Tag) {
                // 算术运算
                case '+':
                    return new NumberExpression(*leftNum + *rightNum);
                case '-':
                    return new NumberExpression(*leftNum - *rightNum);
                case '*':
                    return new NumberExpression(*leftNum * *rightNum);
                case '/':
                    return new NumberExpression(*leftNum / *rightNum);
                case '%':
                    return new NumberExpression(*leftNum % *rightNum);
                // 比较运算
                case EQ:  // ==
                    return new NumberExpression(*leftNum == *rightNum);
                case NE:  // !=
                    return new NumberExpression(*leftNum != *rightNum);
                case '<':
                    return new NumberExpression(*leftNum < *rightNum);
                case '>':
                    return new NumberExpression(*leftNum > *rightNum);
                case BE:  // <=
                    return new NumberExpression(*leftNum <= *rightNum);
                case GE:  // >=
                    return new NumberExpression(*leftNum >= *rightNum);
                // 逻辑运算
                case AND:  // &&
                    return new NumberExpression(*leftNum && *rightNum);
                case OR:   // ||
                    return new NumberExpression(*leftNum || *rightNum);
                default:
                    cout << "Error: Unknown operator Tag " << arith->operator_->Tag << endl;
                    return nullptr;
            }
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            return nullptr;
        }
    }
    
    // 尝试转换为StringLiteral进行运算
    StringLiteral* leftStr = dynamic_cast<StringLiteral*>(left);
    StringLiteral* rightStr = dynamic_cast<StringLiteral*>(right);
    
    // 尝试转换为CharExpression进行运算
    CharExpression* leftChar = dynamic_cast<CharExpression*>(left);
    CharExpression* rightChar = dynamic_cast<CharExpression*>(right);
    
    if (leftStr && rightStr) {
        switch (arith->operator_->Tag) {
            case '+':
                return new StringLiteral(*leftStr + *rightStr);
            case EQ:  // ==
                return new NumberExpression(*leftStr == *rightStr);
            case NE:  // !=
                return new NumberExpression(*leftStr != *rightStr);
            case '<':
                return new NumberExpression(*leftStr < *rightStr);
            case '>':
                return new NumberExpression(*leftStr > *rightStr);
            case BE:  // <=
                return new NumberExpression(*leftStr <= *rightStr);
            case GE:  // >=
                return new NumberExpression(*leftStr >= *rightStr);
            default:
                cout << "Error: Unsupported operator for strings" << endl;
                return nullptr;
        }
    }
    
    // 处理混合类型的加法运算
    if (arith->operator_->Tag == '+' || arith->operator_->Tag == '.') {
        if (leftNum && rightStr) {
            // 数字 + 字符串
            string numStr = to_string(leftNum->getIntValue());
            StringNode* result = new StringNode(numStr);
            result->append(rightStr->toString());
            return result;
        } else if (leftStr && rightNum) {
            // 字符串 + 数字
            StringNode* result = new StringNode(leftStr->toString());
            result->append(to_string(rightNum->getIntValue()));
            return result;
        } else if (leftStr && rightChar) {
            // 字符串 + 字符
            StringNode* result = new StringNode(leftStr->toString());
            result->append(string(1, rightChar->value));
            return result;
        } else if (leftChar && rightStr) {
            // 字符 + 字符串
            StringNode* result = new StringNode(string(1, leftChar->value));
            result->append(rightStr->toString());
            return result;
        } else if (leftChar && rightChar) {
            // 字符 + 字符
            string result = string(1, leftChar->value) + string(1, rightChar->value);
            return new StringLiteral(result);
        }
    }
    
    // 其他类型不匹配的情况
    cout << "Error: Type mismatch in arithmetic expression" << endl;
    return nullptr;
}

// 字符表达式求值
Expression* Interpreter::evaluateCharExpression(CharExpression* charExpr) {
    if (!charExpr) return nullptr;
    
    // 字符表达式直接返回
    return charExpr;
}

// 字符串字面量求值
Expression* Interpreter::evaluateStringLiteral(StringLiteral* strLit) {
    if (!strLit) return nullptr;
    
    // 字符串字面量直接返回
    return strLit;
}

// 字符串拼接表达式求值
Expression* Interpreter::evaluateStringConcatenationExpression(StringConcatenationExpression* concat) {
    if (!concat || !concat->left || !concat->right) return nullptr;
    
    // 求值左右操作数
    Expression* left = evaluate(concat->left);
    Expression* right = evaluate(concat->right);
    
    if (!left || !right) return nullptr;
    
    // 将左右操作数转换为字符串
    string leftStr = convertToString(left);
    string rightStr = convertToString(right);
    
    // 拼接字符串
    string result = leftStr + rightStr;
    
    // 返回新的字符串字面量
    return new StringLiteral(result);
}

// 将表达式转换为字符串的辅助函数
string Interpreter::convertToString(Expression* expr) {
    if (!expr) return "";
    
    // 根据表达式类型进行转换
    if (StringLiteral* strLit = dynamic_cast<StringLiteral*>(expr)) {
        return strLit->toString();
    } else if (CharExpression* charExpr = dynamic_cast<CharExpression*>(expr)) {
        return string(1, charExpr->value);
    } else if (NumberExpression* numExpr = dynamic_cast<NumberExpression*>(expr)) {
        return to_string(numExpr->getIntValue());

    } else if (IdentifierExpression* idExpr = dynamic_cast<IdentifierExpression*>(expr)) {
        // 对于标识符，先求值再转换
        Expression* value = evaluate(idExpr);
        if (value) {
            return convertToString(value);
        }
    }
    
    // 默认情况，调用toString方法
    return expr->toString();
}

// 数组节点求值
Expression* Interpreter::evaluateArrayNode(ArrayNode* array) {
    if (!array) return nullptr;
    
    // 对数组中的每个元素进行求值
    for (size_t i = 0; i < array->getElementCount(); ++i) {
        Expression* element = array->getElement(i);
        if (element) {
            Expression* evaluated = evaluate(element);
            if (evaluated) {
                array->setElement(i, evaluated);
            }
        }
    }
    
    return array;
}

// 字典节点求值
Expression* Interpreter::evaluateDictNode(DictNode* dict) {
    if (!dict) return nullptr;
    
    // 对字典中的每个值进行求值
    vector<string> keys = dict->getKeys();
    for (const string& key : keys) {
        Expression* value = dict->getEntry(key);
        if (value) {
            Expression* evaluated = evaluate(value);
            if (evaluated) {
                dict->setEntry(key, evaluated);
            }
        }
    }
    
    return dict;
}

// 字符串节点求值
Expression* Interpreter::evaluateStringNode(StringNode* strNode) {
    if (!strNode) return nullptr;
    
    // 字符串节点直接返回
    return strNode;
}

// 访问表达式求值
Expression* Interpreter::evaluateAccessExpression(AccessExpression* access) {
    if (!access || !access->target || !access->key) return nullptr;
    
    // 求值目标和键
    Expression* target = evaluate(access->target);
    Expression* key = evaluate(access->key);
    
    if (!target || !key) return nullptr;
    
    // 执行访问操作
    if (StringNode* strNode = dynamic_cast<StringNode*>(target)) {
        return strNode->access(key);
    } else if (ArrayNode* arrayNode = dynamic_cast<ArrayNode*>(target)) {
        return arrayNode->access(key);
    }  else if (DictNode* dictNode = dynamic_cast<DictNode*>(target)) {
        return dictNode->access(key);
    }

    cout << "Unknown target type" << endl;
    return nullptr;
}

// 函数调用表达式求值
Expression* Interpreter::evaluateCallExpression(CallExpression* call) {
    if (!call || !call->callee) return nullptr;
    
    // 检查是否是内置函数
    if (IdentifierExpression* idExpr = dynamic_cast<IdentifierExpression*>(call->callee)) {
        string funcName = idExpr->getName();
        if (isBuiltinFunction(funcName)) {
            return executeBuiltinFunction(funcName, call->arguments);
        }
    }
    
    // 求值所有参数
    vector<Expression*> evaluatedArgs;
    for (Expression* arg : call->arguments) {
        Expression* evaluatedArg = evaluate(arg);
        if (evaluatedArg) {
            evaluatedArgs.push_back(evaluatedArg);
        }
    }
    
    // 求值被调用者
    if (IdentifierExpression* idExpr = dynamic_cast<IdentifierExpression*>(call->callee)) {
        string funcName = idExpr->getName();
        
        // 首先检查是否是内置函数
        if (isBuiltinFunction(funcName)) {
            return executeBuiltinFunction(funcName, evaluatedArgs);
        }
        
        // 然后查找用户定义的函数
        FunctionDefinition* funcDef = lookupFunction(funcName);
        if (!funcDef) {
            cout << "Error: Function not found: " << funcName << endl;
            return nullptr;
        }
        
        LOG_DEBUG("Calling function '" + funcName + "' with " + to_string(evaluatedArgs.size()) + " arguments");
        return executeFunction(funcDef, evaluatedArgs);
    }
    
    cout << "Error: Function not found or not callable" << endl;
    return nullptr;
}

Expression* Interpreter::executeReturn(ReturnStatement* returnStmt) {
    if (!returnStmt || !returnStmt->returnValue) return nullptr;
    LOG_DEBUG("Executing return statement");
    Expression* result = evaluate(returnStmt->returnValue);
    LOG_DEBUG("Return value: " + (result ? result->toString() : "null"));
    return result;
}

// 执行函数
Expression* Interpreter::executeFunction(FunctionDefinition* funcDef, vector<Expression*>& args) {
    if (!funcDef || !funcDef->prototype || !funcDef->body) return nullptr;
    
    LOG_DEBUG("Executing function '" + funcDef->prototype->name + "'");
    
    // 进入新的作用域
    enterScope();
    
    // 绑定参数到局部变量
    const vector<string>& params = funcDef->prototype->parameters;
    for (size_t i = 0; i < params.size() && i < args.size(); ++i) {
        defineVariable(params[i], args[i]);
        LOG_DEBUG("Bound parameter '" + params[i] + "' = " + args[i]->toString());
    }
    
    // 执行函数体
    Expression* result = nullptr;
    LOG_DEBUG("Executing function body with " + to_string(funcDef->body->statements.size()) + " statements");
    for (size_t i = 0; i < funcDef->body->statements.size(); ++i) {
        Statement* stmt = funcDef->body->statements[i];
        LOG_DEBUG("Executing statement " + to_string(i) + ": " + (stmt ? typeid(*stmt).name() : "null"));
        
        ReturnResult returnResult = execute(stmt);
        if (returnResult.hasReturn) {
            // 遇到了return语句
            LOG_DEBUG("Function returned: " + (returnResult.value ? returnResult.value->toString() : "null"));
            result = returnResult.value;
            break;
        }
    }
    
    // 退出作用域
    exitScope();
    
    return result;
}

// 执行函数定义
void Interpreter::executeFunctionDefinition(FunctionDefinition* funcDef) {
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

// 字符串拼接
Expression* Interpreter::stringConcatenation(Expression* left, Expression* right) {
    StringLiteral* leftStr = dynamic_cast<StringLiteral*>(left);
    StringLiteral* rightStr = dynamic_cast<StringLiteral*>(right);
    
    if (leftStr && rightStr) {
        string result = leftStr->toString() + rightStr->toString();
        return new StringLiteral(result);
    }
    
    reportError("String concatenation requires string operands");
    return nullptr;
}

// 字符串比较
Expression* Interpreter::stringComparison(Expression* left, Expression* right, const string& op) {
    StringLiteral* leftStr = dynamic_cast<StringLiteral*>(left);
    StringLiteral* rightStr = dynamic_cast<StringLiteral*>(right);
    
    if (leftStr && rightStr) {
        string leftVal = leftStr->toString();
        string rightVal = rightStr->toString();
        bool result = false;
        
        if (op == "==") result = (leftVal == rightVal);
        else if (op == "!=") result = (leftVal != rightVal);
        else if (op == "<") result = (leftVal < rightVal);
        else if (op == ">") result = (leftVal > rightVal);
        else if (op == "<=") result = (leftVal <= rightVal);
        else if (op == ">=") result = (leftVal >= rightVal);
        
        // 返回布尔值（这里简化处理，实际应该返回布尔字面量）
        return new StringLiteral(result ? "true" : "false");
    }
    
    reportError("String comparison requires string operands");
    return nullptr;
}

// 字符串索引访问
Expression* Interpreter::stringIndexing(StringNode* str, Expression* index) {
    if (!str || !index) return nullptr;
    
    // 求值索引
    Expression* evaluatedIndex = evaluate(index);
    if (!evaluatedIndex) return nullptr;
    
    // 执行访问操作
    return str->access(evaluatedIndex);
}

// 字符串长度
Expression* Interpreter::stringLength(StringNode* str) {
    if (!str) return nullptr;
    
    // 返回字符串长度（这里简化处理，实际应该返回数字字面量）
    string lengthStr = to_string(str->length());
    return new StringLiteral(lengthStr);
}

// 字符串子串
Expression* Interpreter::stringSubstring(StringNode* str, Expression* start, Expression* length) {
    if (!str || !start || !length) return nullptr;
    
    // 求值起始位置和长度
    Expression* evaluatedStart = evaluate(start);
    Expression* evaluatedLength = evaluate(length);
    
    if (!evaluatedStart || !evaluatedLength) return nullptr;
    
    // 这里简化处理，实际应该解析数字并调用substring方法
    return str;
}

// 语句执行
ReturnResult Interpreter::execute(Statement* stmt) {
    if (!stmt) return ReturnResult();
    
    // 根据语句类型调用相应的执行方法
    if (ExpressionStatement* exprStmt = dynamic_cast<ExpressionStatement*>(stmt)) {
        return executeExpressionStatement(exprStmt);
    } else if (VariableDeclaration* varDecl = dynamic_cast<VariableDeclaration*>(stmt)) {
        return executeVariableDeclaration(varDecl);
    } else if (IfStatement* ifStmt = dynamic_cast<IfStatement*>(stmt)) {
        return executeIfStatement(ifStmt);
    } else if (WhileStatement* whileStmt = dynamic_cast<WhileStatement*>(stmt)) {
        return executeWhileStatement(whileStmt);
    } else if (BlockStatement* blockStmt = dynamic_cast<BlockStatement*>(stmt)) {
        return executeBlockStatement(blockStmt);
    } else if (FunctionDefinition* funcDef = dynamic_cast<FunctionDefinition*>(stmt)) {
        executeFunctionDefinition(funcDef);
        return ReturnResult();
    } else if (StructDefinition* structDef = dynamic_cast<StructDefinition*>(stmt)) {
        registerStructDefinition(structDef);
        return ReturnResult();
    } else if (ReturnStatement* returnStmt = dynamic_cast<ReturnStatement*>(stmt)) {
        Expression* value = executeReturn(returnStmt);
        return ReturnResult(value);
    }
    
    return ReturnResult();
}

// 程序执行
void Interpreter::execute(Program* program) {
    if (!program) return;
    
    for (Statement* stmt : program->statements) {
        execute(stmt);
    }
}

// 表达式语句执行
ReturnResult Interpreter::executeExpressionStatement(ExpressionStatement* stmt) {
    if (!stmt || !stmt->expression) return ReturnResult();
    
    evaluate(stmt->expression);
    return ReturnResult();
}

// 变量声明执行
ReturnResult Interpreter::executeVariableDeclaration(VariableDeclaration* decl) {
    if (!decl) return ReturnResult();
    
    // 处理多个变量声明
    for (const auto& var : decl->variables) {
        string name = var.name;
        Expression* value = nullptr;
        
        LOG_DEBUG("Declaring variable '" + name + "'");
        
        if (var.initializer) {
            LOG_DEBUG("Evaluating initializer for '" + name + "': " + var.initializer->toString());
            value = evaluate(var.initializer);
            LOG_DEBUG("Initializer result: " + (value ? value->toString() : "null"));
        } else {
            // 如果没有初始化器，使用默认值0
            value = new NumberExpression(0);
        }
        
        defineVariable(name, value);
        LOG_DEBUG("Defined variable '" + name + "' with value " + (value ? value->toString() : "null"));
    }
    
    return ReturnResult();
}

// 条件语句执行
ReturnResult Interpreter::executeIfStatement(IfStatement* ifStmt) {
    if (!ifStmt || !ifStmt->condition) return ReturnResult();
    
    Expression* condition = evaluate(ifStmt->condition);
    
    // 检查条件是否为真（非零值）
    bool conditionValue = false;
    if (NumberExpression* numExpr = dynamic_cast<NumberExpression*>(condition)) {
        conditionValue = (numExpr->getIntValue() != 0);
        LOG_DEBUG("Condition value: " + to_string(numExpr->getIntValue()) + " (bool: " + (conditionValue ? "true" : "false") + ")");
    }
    
    if (conditionValue && ifStmt->thenBranch) {
        LOG_DEBUG("Executing then branch");
        // 为then分支创建独立作用域
        enterScope();
        ReturnResult result = execute(ifStmt->thenBranch);
        exitScope();  // 退出then分支作用域
        if (result.hasReturn) {
            return result;  // 传递return值
        }
    } else if (!conditionValue && ifStmt->elseBranch) {
        LOG_DEBUG("Executing else branch");
        // 为else分支创建独立作用域
        enterScope();
        ReturnResult result = execute(ifStmt->elseBranch);
        exitScope();  // 退出else分支作用域
        if (result.hasReturn) {
            return result;  // 传递return值
        }
    }
    
    return ReturnResult();
}

// 循环语句执行
ReturnResult Interpreter::executeWhileStatement(WhileStatement* whileStmt) {
    if (!whileStmt || !whileStmt->condition || !whileStmt->body) return ReturnResult();
    
    while (true) {
        // 评估循环条件
        Expression* condition = evaluate(whileStmt->condition);
        
        // 检查条件是否为真（非零值）
        bool conditionValue = false;
        if (NumberExpression* numExpr = dynamic_cast<NumberExpression*>(condition)) {
            conditionValue = (numExpr->getIntValue() != 0);
            LOG_DEBUG("While condition value: " + to_string(numExpr->getIntValue()) + " (bool: " + (conditionValue ? "true" : "false") + ")");
        }
        
        // 如果条件为假，退出循环
        if (!conditionValue) {
            LOG_DEBUG("While condition is false, exiting loop");
            break;
        }
        
        // 执行循环体（不创建新作用域，让变量赋值影响外层作用域）
        LOG_DEBUG("Executing while loop body");
        ReturnResult result;
        
        // 如果循环体是块语句，直接执行其中的语句而不创建新作用域
        if (BlockStatement* block = dynamic_cast<BlockStatement*>(whileStmt->body)) {
            for (Statement* stmt : block->statements) {
                result = execute(stmt);
                if (result.hasReturn) {
                    return result;  // 传递return值
                }
            }
        } else {
            // 如果不是块语句，正常执行
            result = execute(whileStmt->body);
            if (result.hasReturn) {
                return result;  // 传递return值
            }
        }
    }
    
    return ReturnResult();
}

// 语句块执行
ReturnResult Interpreter::executeBlockStatement(BlockStatement* block) {
    if (!block) return ReturnResult();
    
    enterScope();
    
    for (Statement* stmt : block->statements) {
        ReturnResult result = execute(stmt);
        if (result.hasReturn) {
            exitScope();
            return result;  // 传递return值
        }
    }
    
    exitScope();
    return ReturnResult();
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
    
    builtinFunctions["length"] = [this](vector<Expression*>& args) -> Expression* {
        return executeStringLength(args);
    };
    
    builtinFunctions["substring"] = [this](vector<Expression*>& args) -> Expression* {
        return executeStringSubstring(args);
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
            cout << evaluated->toString();
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
        return new NumberExpression(array->getElementCount());
    }

    // 如果是字符串，返回字符串长度
    StringLiteral* str = dynamic_cast<StringLiteral*>(arg);
    if (str) {
        return new NumberExpression(str->length());
    }

    // 如果是字典，返回字典的键值对数量
    DictNode* dict = dynamic_cast<DictNode*>(arg);
    if (dict) {
        return new NumberExpression(dict->getEntryCount());
    }
    
    // 对于其他类型，返回1（表示单个元素）
    return new NumberExpression(1);
}

Expression* Interpreter::executeCin(vector<Expression*>& args) {
    string input;
    cin >> input;
    return new StringLiteral(input);
}

Expression* Interpreter::executeStringLength(vector<Expression*>& args) {
    if (args.empty()) return nullptr;
    
    Expression* arg = evaluate(args[0]);
    if (!arg) return nullptr;
    
    StringLiteral* str = dynamic_cast<StringLiteral*>(arg);
    if (str) {
        return new StringLiteral(to_string(str->length()));
    }
    
    reportError("length() requires a string argument");
    return nullptr;
}

Expression* Interpreter::executeStringSubstring(vector<Expression*>& args) {
    if (args.size() < 3) return nullptr;
    
    Expression* strArg = evaluate(args[0]);
    Expression* startArg = evaluate(args[1]);
    Expression* lengthArg = evaluate(args[2]);
    
    if (!strArg || !startArg || !lengthArg) return nullptr;
    
    StringLiteral* str = dynamic_cast<StringLiteral*>(strArg);
    if (str) {
        // 这里简化处理，实际应该解析数字并调用substring方法
        return str;
    }
    
    reportError("substring() requires string arguments");
    return nullptr;
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
        cout << "  " << pair.first << " = " << pair.second->toString() << endl;
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

// 结构体实例化求值
Expression* Interpreter::evaluateStructInstantiation(StructInstantiationExpression* structInst) {
    if (!structInst) return nullptr;
    
    string structName = structInst->structName;
    
    // 查找结构体定义
    auto it = structDefinitions.find(structName);
    if (it == structDefinitions.end()) {
        reportError("Undefined struct: " + structName);
        return nullptr;
    }
    
    StructDefinition* structDef = it->second;
    
    // 创建一个字典来存储结构体实例
    DictNode* instance = new DictNode();
    
    // 初始化所有成员为默认值
    for (const auto& member : structDef->members) {
        if (member.defaultValue) {
            instance->setEntry(member.name, evaluate(member.defaultValue));
        } else {
            // 根据类型设置默认值
            if (member.type == "string") {
                instance->setEntry(member.name, new StringLiteral(""));
            } else if (member.type == "int" || member.type == "double") {
                instance->setEntry(member.name, new NumberExpression(0));
            } else {
                instance->setEntry(member.name, new StringLiteral(""));
            }
        }
    }
    
    // 应用提供的字段值
    for (const auto& field : structInst->fieldValues) {
        instance->setEntry(field.first, evaluate(field.second));
    }
    
    return instance;
}

// 成员访问求值
Expression* Interpreter::evaluateMemberAccess(MemberAccessExpression* memberAccess) {
    if (!memberAccess) return nullptr;
    
    // 求值对象
    Expression* object = evaluate(memberAccess->object);
    if (!object) return nullptr;
    
    string memberName = memberAccess->memberName;
    
    // 检查对象是否是结构体实例（DictNode）
    if (DictNode* dict = dynamic_cast<DictNode*>(object)) {
        Expression* member = dict->getEntry(memberName);
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
