#include "interpreter.h"
#include "lexer.h"
#include <iostream>
#include <sstream>

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
        delete scope;
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
        delete currentScope;
        scopes.pop_back();
        currentScope = scopes.back();
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
        return numExpr;  // 数字表达式直接返回
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
    }
    
    return nullptr;
}

// 声明求值
Expression* Interpreter::evaluateDeclaration(VariableDeclaration* decl) {
    if (!decl) return nullptr;
    
    string name = decl->name;
    Expression* value = nullptr;
    
    if (decl->initializer) {
        value = evaluate(decl->initializer);
    }
    
    // 如果变量没有初始值，设置为默认值（数字0）
    if (!value) {
        value = new NumberExpression(0);
    }
    
    defineVariable(name, value);
    return nullptr;
}

// 表达式求值
Expression* Interpreter::evaluateExpression(Expression* expr) {
    if (!expr) return nullptr;
    
    // 根据表达式类型调用相应的求值方法
    if (NumberExpression* numExpr = dynamic_cast<NumberExpression*>(expr)) {
        return numExpr;
    } else if (IdentifierExpression* idExpr = dynamic_cast<IdentifierExpression*>(expr)) {
        return evaluateIdentifierExpression(idExpr);
    } else if (ArithmeticExpression* arith = dynamic_cast<ArithmeticExpression*>(expr)) {
        return evaluateArithmeticExpression(arith);
    } else if (StringLiteral* strLit = dynamic_cast<StringLiteral*>(expr)) {
        return evaluateStringLiteral(strLit);
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
    } else if (StringConcatenationExpression* concat = dynamic_cast<StringConcatenationExpression*>(expr)) {
        return evaluateStringConcatenationExpression(concat);
    }
    
    reportError("Unknown expression type");
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
        }
    }
    
    // 其他类型不匹配的情况
    cout << "Error: Type mismatch in arithmetic expression" << endl;
    return nullptr;
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
        
        cout << "DEBUG: Calling function '" << funcName << "' with " << evaluatedArgs.size() << " arguments" << endl;
        return executeFunction(funcDef, evaluatedArgs);
    }
    
    cout << "Error: Function not found or not callable" << endl;
    return nullptr;
}

Expression* Interpreter::executeReturn(ReturnStatement* returnStmt) {
    if (!returnStmt || !returnStmt->returnValue) return nullptr;
    cout << "DEBUG: Executing return statement" << endl;
    Expression* result = evaluate(returnStmt->returnValue);
    cout << "DEBUG: Return value: " << (result ? result->toString() : "null") << endl;
    return result;
}

// 执行函数
Expression* Interpreter::executeFunction(FunctionDefinition* funcDef, vector<Expression*>& args) {
    if (!funcDef || !funcDef->prototype || !funcDef->body) return nullptr;
    
    cout << "DEBUG: Executing function '" << funcDef->prototype->name << "'" << endl;
    
    // 进入新的作用域
    enterScope();
    
    // 绑定参数到局部变量
    const vector<string>& params = funcDef->prototype->parameters;
    for (size_t i = 0; i < params.size() && i < args.size(); ++i) {
        defineVariable(params[i], args[i]);
        cout << "DEBUG: Bound parameter '" << params[i] << "' = " << args[i]->toString() << endl;
    }
    
    // 执行函数体
    Expression* result = nullptr;
    cout << "DEBUG: Executing function body with " << funcDef->body->statements.size() << " statements" << endl;
    for (size_t i = 0; i < funcDef->body->statements.size(); ++i) {
        Statement* stmt = funcDef->body->statements[i];
        cout << "DEBUG: Executing statement " << i << ": " << (stmt ? typeid(*stmt).name() : "null") << endl;
        
        ReturnResult returnResult = execute(stmt);
        if (returnResult.hasReturn) {
            // 遇到了return语句
            cout << "DEBUG: Function returned: " << (returnResult.value ? returnResult.value->toString() : "null") << endl;
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
    
    cout << "DEBUG: Registered function '" << funcName << "'" << endl;
    
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
    
    string name = decl->name;
    Expression* value = nullptr;
    
    cout << "DEBUG: Declaring variable '" << name << "'" << endl;
    
    if (decl->initializer) {
        cout << "DEBUG: Evaluating initializer for '" << name << "': " << decl->initializer->toString() << endl;
        value = evaluate(decl->initializer);
        cout << "DEBUG: Initializer result: " << (value ? value->toString() : "null") << endl;
    } else {
        // 如果没有初始化器，使用默认值0
        value = new NumberExpression(0);
    }
    
    defineVariable(name, value);
    cout << "DEBUG: Defined variable '" << name << "' with value " << (value ? value->toString() : "null") << endl;
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
        cout << "DEBUG: Condition value: " << numExpr->getIntValue() << " (bool: " << conditionValue << ")" << endl;
    }
    
    if (conditionValue && ifStmt->thenBranch) {
        cout << "DEBUG: Executing then branch" << endl;
        ReturnResult result = execute(ifStmt->thenBranch);
        if (result.hasReturn) {
            return result;  // 传递return值
        }
    } else if (!conditionValue && ifStmt->elseBranch) {
        cout << "DEBUG: Executing else branch" << endl;
        ReturnResult result = execute(ifStmt->elseBranch);
        if (result.hasReturn) {
            return result;  // 传递return值
        }
    }
    
    return ReturnResult();
}

// 循环语句执行
ReturnResult Interpreter::executeWhileStatement(WhileStatement* whileStmt) {
    if (!whileStmt || !whileStmt->condition || !whileStmt->body) return ReturnResult();
    
    // 这里简化处理，实际应该循环执行
    Expression* condition = evaluate(whileStmt->condition);
    if (condition) {
        ReturnResult result = execute(whileStmt->body);
        if (result.hasReturn) {
            return result;  // 传递return值
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
