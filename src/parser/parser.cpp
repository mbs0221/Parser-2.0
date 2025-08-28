#include "parser/parser.h"
#include "parser/expression.h"
#include "parser/inter.h"

using namespace std;

// Parser构造函数和析构函数
Parser::Parser() {
    // 初始化解析器
}

Parser::~Parser() {
    // 清理资源
}

// parse方法实现 - 返回AST
Program* Parser::parse(const string& file) {
    if (lex.open(file)) {
        lex.move();
        return parseProgram();
    } else {
        printf("can't open %s.\n", file.c_str());
        return nullptr;
	}
}

// 解析程序 - 程序根节点
Program* Parser::parseProgram() {
    Program* program = new Program();
    
    while (lex.token()->Tag != -1 && lex.token()->Tag != END_OF_FILE) {
        Statement* stmt = parseDeclaration();
        if (stmt) {
            program->addStatement(stmt);
        }
    }
    
    return program;
}

// 解析声明（全局）
Statement* Parser::parseDeclaration() {
    switch (lex.token()->Tag) {
        case IMPORT:
            return parseImportStatement();
        case LET:
            return parseVariableDeclaration();
        case FUNCTION:
            return parseFunction();
        case STRUCT:
            return parseStruct();
        case CLASS:
            return parseClass();
        case ID:
            return parseExpressionStatement();
        default:
            printf("SYNTAX ERROR line[%03d]: unexpected token in global declaration\n", lex.line);
            exit(1);  // 强制退出
            return nullptr;
    }
}

// 解析语句（函数内部）
Statement* Parser::parseStatement() {
    switch (lex.token()->Tag) {
        case LET:
            return parseVariableDeclaration();
        case IF:
            return parseIfStatement();
        case WHILE:
            return parseWhileStatement();
        case FOR:
            return parseForStatement();
        case BREAK:
            return parseBreakStatement();
        case CONTINUE:
            return parseContinueStatement();
        case RETURN:
            return parseReturnStatement();
        case THROW:
            // throw语句暂未实现
            printf("SYNTAX ERROR line[%03d]: throw statement not implemented\n", lex.line);
            exit(1);
        case TRY:
            return parseTryStatement();
        case SWITCH:
            return parseSwitchStatement();
        case ID:
            return parseExpressionStatement();
        case '{':
            return parseBlock();
        default:
            printf("SYNTAX ERROR line[%03d]: unexpected token in statement\n", lex.line);
            exit(1);  // 强制退出
            return nullptr;
    }
}

// 解析导入语句 (import "module.txt";)
ImportStatement* Parser::parseImportStatement() {
    lex.match(IMPORT);
    
    // 解析字符串字面量作为模块名
    if (lex.token()->Tag != STR) {
        printf("SYNTAX ERROR line[%03d]: expected string literal after import\n", lex.line);
        exit(1);
    }
    
    String *moduleName = dynamic_cast<String*>(lex.matchValue());
    lex.match(';');

    return new ImportStatement(moduleName);
}

// 解析变量声明语句 (let x = 10, y = 20, z;)
VariableDeclaration* Parser::parseVariableDeclaration() {
    lex.match(LET);
    VariableDeclaration* decl = new VariableDeclaration();
    
    while (true) {
        // 解析标识符
        string name = lex.matchIdentifier();
        
        Expression* value = nullptr;
        
        // 检查是否有初始化表达式
        if (lex.token()->Tag == '=') {
            lex.match('=');
            value = parseExpression();
        }
        
        // 添加变量到声明中
        decl->addVariable(name, value);
        
        // 检查是否还有更多变量（用逗号分隔）
        if (lex.token()->Tag == ',') {
            lex.match(',');
        } else {
            break;
        }
    }

    // 匹配分号
    lex.match(';');
    
    return decl;
}

// 解析表达式语句 (x + y;)
ExpressionStatement* Parser::parseExpressionStatement() {
    Expression* expr = parseExpression();
    lex.match(';');
    return new ExpressionStatement(expr);
}

// 解析条件语句 (if (x > 0) { ... } else { ... })
IfStatement* Parser::parseIfStatement() {
    lex.match(IF);
    lex.match('(');
    Expression* condition = parseExpression();
    lex.match(')');
    
    Statement* thenStmt = parseStatement();
    Statement* elseStmt = nullptr;
    if (lex.token()->Tag == ELSE) {
        lex.match(ELSE);
        elseStmt = parseStatement();
    }
    
    return new IfStatement(condition, thenStmt, elseStmt);
}

// 解析循环语句 (while (x > 0) { ... })
WhileStatement* Parser::parseWhileStatement() {
    lex.match(WHILE);
    lex.match('(');
    
    Expression* condition = parseExpression();
    
    lex.match(')');
    
    Statement* body = parseStatement();
    
    return new WhileStatement(condition, body);
}

// 解析For循环语句 (for (let i = 0; i < 10; i++) { ... })
ForStatement* Parser::parseForStatement() {
    lex.match(FOR);
    lex.match('(');
    
    Statement* init = nullptr;
    if (lex.token()->Tag != ';') {
        // 将初始化表达式包装成表达式语句
        Expression* initExpr = parseExpression();
        init = new ExpressionStatement(initExpr);
    }
    
    lex.match(';');
    
    Expression* condition = nullptr;
    if (lex.token()->Tag != ';') {
        condition = parseExpression();
    }
    
    lex.match(';');
    
    Expression* increment = nullptr;
    if (lex.token()->Tag != ')') {
        increment = parseExpression();
    }
    
    lex.match(')');
    
    Statement* body = parseStatement();
    
    return new ForStatement(init, condition, increment, body);
}

// 解析break语句
BreakStatement* Parser::parseBreakStatement() {
    lex.match(BREAK);
	lex.match(';');
    return new BreakStatement();
}

// 解析continue语句
ContinueStatement* Parser::parseContinueStatement() {
    lex.match(CONTINUE);
    lex.match(';');
    return new ContinueStatement();
}

// 解析return语句
ReturnStatement* Parser::parseReturnStatement() {
    lex.match(RETURN);
    Expression* returnValue = nullptr;
    if (lex.token()->Tag != ';') {
        returnValue = parseExpression();
    }
    lex.match(';');
    return new ReturnStatement(returnValue);
}



// 解析try语句 - 合并了catch和finally
TryStatement* Parser::parseTryStatement() {
    lex.match(TRY);
    Statement* tryBlock = parseStatement();
    vector<TryStatement::CatchBlock> catchBlocks;
    Statement* finallyBlock = nullptr;
    
    while (lex.token()->Tag == CATCH) {
        lex.match(CATCH);
        lex.match('(');
        string exceptionType = "Exception"; // 默认异常类型
        string exceptionName = lex.matchIdentifier();
        lex.match(')');
        Statement* catchBody = parseStatement();
        catchBlocks.push_back(TryStatement::CatchBlock(exceptionType, exceptionName, catchBody));
    }
    
    if (lex.token()->Tag == FINALLY) {
        lex.match(FINALLY);
        finallyBlock = parseStatement();
    }
    
    return new TryStatement(tryBlock, catchBlocks, finallyBlock);
}

// 解析switch语句 - 合并了case和default
SwitchStatement* Parser::parseSwitchStatement() {
    lex.match(SWITCH);
    lex.match('(');
    Expression* condition = parseExpression();
    lex.match(')');
    lex.match('{');
    vector<SwitchStatement::SwitchCase> cases;
    
    // 解析case语句
    while (lex.token()->Tag == CASE) {
        lex.match(CASE);
        Expression* caseValue = parseExpression();
        lex.match(':');
        
        // 解析case主体 - 可以是单个语句或多个语句
        vector<Statement*> caseStatements;
        while (lex.token()->Tag != CASE && lex.token()->Tag != DEFAULT && lex.token()->Tag != '}') {
            Statement* stmt = parseStatement();
            if (stmt) {
                caseStatements.push_back(stmt);
            }
        }
        
        cases.push_back(SwitchStatement::SwitchCase(caseValue, caseStatements));
    }
    
    // 解析default语句
    if (lex.token()->Tag == DEFAULT) {
        lex.match(DEFAULT);
        lex.match(':');
        
        // 解析default主体 - 可以是单个语句或多个语句
        vector<Statement*> defaultStatements;
        while (lex.token()->Tag != '}') {
            Statement* stmt = parseStatement();
            if (stmt) {
                defaultStatements.push_back(stmt);
            }
        }
        
        cases.push_back(SwitchStatement::SwitchCase(nullptr, defaultStatements));
    }
    
    lex.match('}');
    return new SwitchStatement(condition, cases);
}

// 解析语句块 ({ ... })
BlockStatement* Parser::parseBlock() {
    lex.match('{');
    
    BlockStatement* block = new BlockStatement();
    
    while (lex.token()->Tag != '}') {
        Statement* stmt = parseStatement();
        if (stmt) {
            block->addStatement(stmt);
        }
    }
    
    lex.match('}');
    
    return block;
}

// 统一的表达式解析函数 - 使用运算符优先文法
Expression* Parser::parseExpression() {
    return parseExpressionWithPrecedence(0);
}

// 根据优先级解析表达式
Expression* Parser::parseExpressionWithPrecedence(int minPrecedence) {

    // 解析左操作数
    Expression* left = parsePrimary();
    
    // 处理后缀操作符（成员访问、方法调用等）
    left = parsePostfix(left);
    
    // 处理二元操作符
    while (true) {
        // 检查当前token是否为二元操作符
        if (!isBinaryOperator(lex.token()->Tag)) {
            break;
        }
        
        // 获取操作符
        Operator* op = lex.matchOperator();
        int precedence = op->getPrecedence();
        bool isLeftAssoc = op->isLeftAssoc();
        
        // 如果操作符优先级低于最小优先级，停止解析
        if (precedence < minPrecedence) {
            break;
        }
        
        // 处理赋值操作符的特殊情况
        if (op->Tag == '=') {
            // 检查左操作数是否为变量引用
            if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(left)) {
                // 对于赋值操作符，使用右结合性，所以递归调用时使用相同优先级
                Expression* right = parseExpressionWithPrecedence(precedence);
                left = new AssignExpression(left, right);
            } else {
                printf("Error: Left side of assignment must be a variable\n");
                break;
            }
        } else {
            // 处理其他二元操作符
            // 对于左结合操作符，递归调用时使用更高优先级
            // 对于右结合操作符，递归调用时使用相同优先级
            int nextPrecedence = isLeftAssoc ? precedence + 1 : precedence;
            Expression* right = parseExpressionWithPrecedence(nextPrecedence);
            
            left = new BinaryExpression(left, right, op);
        }
    }
    
    return left;
}

// 解析基本表达式（因子）
Expression* Parser::parsePrimary() {
    switch (lex.token()->Tag) {
        case '!': // 逻辑非 (ASCII 33)
        case '-': // 负号 (ASCII 45)
        case '~': // 位运算取反 (ASCII 126)
            {
                Operator* op = lex.matchOperator();
                Expression* operand = parsePrimary();
                return new UnaryExpression(operand, op);
            }
        case '+': // 正号 (ASCII 43) - 只在表达式开头时作为一元操作符
            {
                // 检查是否在表达式开头，如果是，则作为一元正号处理
                Operator* op = lex.matchOperator();
                Expression* operand = parsePrimary();
                return new UnaryExpression(operand, op);
            }
        case ID: // 标识符
            return parseVariable();
        case NUM: // 整数
        case REAL: // 浮点数
        case BOOL: // 布尔值
        case STR: // 字符串
        case CHAR: // 字符
            return parseConstant();
        case '(': // 括号表达式
            {
                lex.matchToken('(');
                Expression* expr = parseExpressionWithPrecedence(0);
                lex.matchToken(')');
                return expr;
            }
        case '[': // 数组
            return parseArray();
        case '{': // 字典字面量
            return parseDict();
        default:
            printf("SYNTAX ERROR line[%03d]: unexpected token in expression\n", lex.line);
            lex.match(lex.token()->Tag);
            return nullptr;
    }
}

// 解析后缀操作符
Expression* Parser::parsePostfix(Expression* expr) {
    while (true) {
        switch (lex.token()->Tag) {
            case '.':
                {
                    // 成员访问
                    lex.match('.');
                    string memberName = lex.matchIdentifier();
                    
                    if (lex.token()->Tag == '(') {
                        // 方法调用：将对象作为第一个参数
                        lex.matchToken('(');
                        vector<Expression*> arguments;
                        arguments.push_back(expr); // 将对象作为第一个参数
                        
                        if (lex.token()->Tag != ')') {
                            arguments.push_back(parseExpressionWithPrecedence(0));
                            while (lex.token()->Tag == ',') {
                                lex.matchToken(',');
                                arguments.push_back(parseExpressionWithPrecedence(0));
                            }
                        }
                        
                        lex.matchToken(')');
                        expr = new CallExpression(memberName, arguments);
                    } else {
                        // 成员访问
                        expr = new AccessExpression(expr, new ConstantExpression(new String(memberName)));
                    }
                }
                break;
                
            case '[':
                // 数组访问
                {
                    lex.matchToken('[');
                    Expression* index = parseExpressionWithPrecedence(0);
                    lex.matchToken(']');
                    expr = new AccessExpression(expr, index);
                }
                break;
                
            case '(':
                // 函数调用
                {
                    expr = parseCallExpression(expr);
                }
                break;
                
            case '{':
                // 结构体实例化：ID {member: value, ...}
                {
                    // 检查前面的表达式是否是标识符
                    if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(expr)) {
                        // 解析结构体实例化
                        expr = parseStructInstantiation(varExpr->name);
                    } else {
                        // 如果不是标识符，报错
                        printf("SYNTAX ERROR line[%03d]: expected identifier before '{' for struct instantiation\n", lex.line);
                        return nullptr;
                    }
                }
                break;
                
            default:
                return expr;
        }
    }
}

// 检查是否为二元操作符
bool Parser::isBinaryOperator(int tag) {
    // 检查单字符操作符（ASCII码）
    if (tag == '+' || tag == '-' || tag == '*' || tag == '/' || tag == '%' ||
        tag == '<' || tag == '>' || tag == '=' || tag == '&' || 
        tag == '|' || tag == '^') {
        return true;
    }
    
    // 检查多字符操作符（枚举类型）
    return tag == LE || tag == GE || tag == EQ_EQ || tag == NE_EQ || 
           tag == AND_AND || tag == OR_OR || tag == BIT_AND || tag == BIT_OR || 
           tag == BIT_XOR || tag == LEFT_SHIFT || tag == RIGHT_SHIFT;
}

// 解析标识符
Expression* Parser::parseVariable() {
    string name = lex.matchIdentifier();
    VariableExpression* idExpr = new VariableExpression(name);
    
    // 只返回变量表达式，函数调用、数组访问等由parsePostfix处理
    return idExpr;
}

// 解析常量
Expression* Parser::parseConstant() {
    // 根据当前token类型直接调用相应的matchValue函数
    // matchValue内部会处理类型检查和错误
    switch (lex.token()->Tag) {
        case NUM:
            return new ConstantExpression(dynamic_cast<Integer*>(lex.matchValue()));
        case REAL:
            return new ConstantExpression(dynamic_cast<Double*>(lex.matchValue()));
        case STR:
            return new ConstantExpression(dynamic_cast<String*>(lex.matchValue()));
        case CHAR:
            return new ConstantExpression(dynamic_cast<Char*>(lex.matchValue()));
        case BOOL:
            return new ConstantExpression(dynamic_cast<Bool*>(lex.matchValue()));
        default:
            printf("SYNTAX ERROR line[%03d]: unexpected token in constant\n", lex.line);
            lex.match(lex.token()->Tag);
            return nullptr;
    }
}

// 解析数组字面量
Expression* Parser::parseArray() {
    lex.match('[');  // 匹配开始方括号
    
    Array* array = new Array();
    
    if (lex.token()->Tag != ']') {
        // 解析第一个元素
        Expression* element = parseExpression();
        // 将Expression转换为Value
        if (ConstantExpression* constExpr = dynamic_cast<ConstantExpression*>(element)) {
            array->addElement(constExpr->value);
        } else {
            // 对于非常量表达式，暂时使用nullptr
            array->addElement(nullptr);
        }
        
        // 解析后续元素
        while (lex.token()->Tag == ',') {
            lex.match(',');
            element = parseExpression();
            if (ConstantExpression* constExpr = dynamic_cast<ConstantExpression*>(element)) {
                array->addElement(constExpr->value);
            } else {
                array->addElement(nullptr);
            }
        }
    }
    
    lex.match(']');  // 匹配结束方括号
    
    // 将Array包装在ConstantExpression中
    return new ConstantExpression(array);
}

// 解析字典字面量
Expression* Parser::parseDict() {
    lex.match('{');  // 匹配开始大括号
    
    Dict* dict = new Dict();
    
    if (lex.token()->Tag != '}') {
        // 解析第一个键值对
        String* key = dynamic_cast<String*>(lex.matchValue());
        lex.match(':');
        Expression* valueExpr = parseExpression();
        // 将Expression转换为Value
        if (ConstantExpression* constExpr = dynamic_cast<ConstantExpression*>(valueExpr)) {
            dict->setEntry(key->getValue(), constExpr->value);
        } else {
            // 对于非常量表达式，暂时使用nullptr
            dict->setEntry(key->getValue(), nullptr);
        }
        
        // 解析后续键值对
        while (lex.token()->Tag == ',') {  
            lex.match(',');
            key = dynamic_cast<String*>(lex.matchValue());
            lex.match(':');
            valueExpr = parseExpression();
            if (ConstantExpression* constExpr = dynamic_cast<ConstantExpression*>(valueExpr)) {
                dict->setEntry(key->getValue(), constExpr->value);
            } else {
                dict->setEntry(key->getValue(), nullptr);
            }
        }
    }
    
    lex.match('}');  // 匹配结束大括号
    
    // 将Dict包装在ConstantExpression中
    return new ConstantExpression(dict);
}

// 解析结构体实例化
Expression* Parser::parseStructInstantiation(const string& structName) {
    lex.match('{');  // 匹配开始大括号
    
    vector<pair<string, Expression*>> members;
    
    if (lex.token()->Tag != '}') {
        // 解析第一个成员 - key是标识符，不是字符串
        string key = lex.matchIdentifier();
        lex.match(':');
        Expression* valueExpr = parseExpression();
        members.push_back(make_pair(key, valueExpr));
        
        // 解析后续成员
        while (lex.token()->Tag == ',') {  
            lex.match(',');
            key = lex.matchIdentifier();
            lex.match(':');
            valueExpr = parseExpression();
            members.push_back(make_pair(key, valueExpr));
        }
    }
    
    lex.match('}');  // 匹配结束大括号
    
    // 创建结构体实例化表达式
    // 这里我们使用CallExpression来表示结构体实例化
    // 按照结构体定义的成员顺序传递参数
    vector<Expression*> arguments;
    
    // 将成员按照结构体定义的顺序转换为参数列表
    // 注意：这里我们暂时保持字典形式，但在instantiateStruct中会按照定义顺序处理
    Dict* dict = new Dict();
    for (const auto& member : members) {
        if (ConstantExpression* constExpr = dynamic_cast<ConstantExpression*>(member.second)) {
            dict->setEntry(member.first, constExpr->value);
        } else {
            // 对于非常量表达式，暂时使用nullptr
            dict->setEntry(member.first, nullptr);
        }
    }
    arguments.push_back(new ConstantExpression(dict));
    
    // 返回结构体实例化调用表达式
    return new CallExpression(structName, arguments);
}



// 解析通用函数调用表达式
Expression* Parser::parseCallExpression(Expression* calleeExpr) {
    // 需要跳过'('token，进入函数参数列表
    lex.match('(');
    
    vector<Expression*> arguments;
    
    if (lex.token()->Tag != ')') {
        arguments.push_back(parseExpression());
        
        while (lex.token()->Tag == ',') {
            lex.match(',');
            arguments.push_back(parseExpression());
        }
    }
    
    lex.match(')');
    
    // 根据被调用表达式的类型决定如何处理
    if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(calleeExpr)) {
        // 变量表达式调用 - 可能是函数调用或类实例化
        return new CallExpression(varExpr->name, arguments);
    } else if (AccessExpression* memberExpr = dynamic_cast<AccessExpression*>(calleeExpr)) {
        // 成员访问表达式调用 - 方法调用
        // 从AccessExpression中提取对象和成员名
        Expression* object = memberExpr->target;
        // 成员名现在是字符串常量，需要提取其值
        if (ConstantExpression* memberNameExpr = dynamic_cast<ConstantExpression*>(memberExpr->key)) {
            if (String* strVal = dynamic_cast<String*>(memberNameExpr->value)) {
                string memberName = strVal->getValue();
                return new MethodCallExpression(object, memberName, arguments);
            }
        }
        // 如果无法提取成员名，返回错误
        return nullptr;
    } else {
        // 其他类型的表达式调用 - 使用通用调用表达式
        return new CallExpression("", arguments);
    }
}

// 解析函数声明 - 返回Prototype语句
FunctionPrototype* Parser::parsePrototype() {
    lex.match(FUNCTION);
    
    // 解析函数名
    string funcName = lex.matchIdentifier();
    
    // 解析参数列表
    lex.match('(');
    vector<pair<string, Type*>> parameters = parseParameterList();
    lex.match(')');
    
    return new FunctionPrototype(funcName, parameters, nullptr);
}

// 解析函数定义
FunctionDefinition* Parser::parseFunction() {
    FunctionPrototype* proto = parsePrototype();
    BlockStatement* body = parseBlock();
    return new UserFunction(proto, body);
}

// 解析结构体定义
StructDefinition* Parser::parseStruct() {
    lex.match(STRUCT);
    string structName = lex.matchIdentifier();
	lex.match('{');
    
    vector<StructMember> members;
    
    while (lex.token()->Tag != '}') {
        // 结构体成员默认为public
        members.push_back(parseClassMember("public"));
    }
    
	lex.match('}');
    
    return new StructDefinition(structName, members);
}

// 解析类定义
ClassDefinition* Parser::parseClass() {
    lex.match(CLASS);
    
    // 解析类名
    string className = lex.matchIdentifier();
    
    string baseClass = "";
    if (lex.token()->Tag == ':') {
		lex.match(':');
        baseClass = lex.matchIdentifier();
    } else {
        baseClass = "Object";
    }
    
    lex.match('{');
    
    vector<StructMember> members;
    vector<ClassMethod*> methods;
    
    // 当前访问修饰符，默认为public
    string currentVisibility = "public";
    
    while (lex.token()->Tag != '}') {
        if (lex.token()->Tag == PUBLIC || lex.token()->Tag == PRIVATE || lex.token()->Tag == PROTECTED) {
            // 更新当前访问修饰符
            Visibility* visibilityToken = static_cast<Visibility*>(lex.token());
            currentVisibility = visibilityToken->toString();
            lex.move(); // 消费访问修饰符
            
            // 继续解析后续的成员，使用新的访问修饰符
            continue;
        } else if (lex.token()->Tag == FUNCTION) {
            // 解析方法，使用当前访问修饰符
            ClassMethod* method = parseClassMethod(currentVisibility);
            methods.push_back(method);
        } else {
            // 解析成员变量，使用当前访问修饰符
            members.push_back(parseClassMember(currentVisibility));
        }
    }
    
    lex.match('}');
    
    return new ClassDefinition(className, baseClass, members, methods);
}

// 解析类成员变量
StructMember Parser::parseClassMember(const string& visibility) {
    // 解析类型和名称
    Type* memberType = lex.matchType();
    string memberName = lex.matchIdentifier();
    Expression* defaultValue = nullptr;
    
    if (lex.token()->Tag == '=') {
        lex.match('=');
        defaultValue = parseExpression();
    }
    
    lex.match(';');
    
    return StructMember(memberName, memberType, visibility, defaultValue);
}

// 解析类方法
ClassMethod* Parser::parseClassMethod(const string& visibility) {
    lex.match(FUNCTION);
    
    // 解析方法名
    string methodName = lex.matchIdentifier();
    
    // 解析参数列表
    lex.match('('); 
    vector<pair<string, Type*>> parameters = parseParameterList();
    lex.match(')');
    
    // 创建函数原型
    FunctionPrototype* prototype = new FunctionPrototype(methodName, parameters, nullptr);
    
    // 解析方法体
    BlockStatement* body = parseBlock();
    
    // 返回带有可见性的ClassMethod
    return new ClassMethod(prototype, body, visibility, false);
}

// 解析参数列表
vector<pair<string, Type*>> Parser::parseParameterList() {
    vector<pair<string, Type*>> parameters;
    
    if (lex.token()->Tag != ')') {
        Token* paramToken = lex.token();
        lex.match(ID);
        
        string paramName = "";
        if (paramToken && paramToken->Tag == Tag::ID) {
            Word* wordToken = static_cast<Word*>(paramToken);
            paramName = wordToken ? wordToken->word : "";
        }
        parameters.push_back(make_pair(paramName, nullptr)); // 暂时使用nullptr，后续可以解析类型
        
        while (lex.token()->Tag == ',') {
            lex.match(',');
            paramToken = lex.token();
            lex.match(ID);
            
            paramName = "";
            if (paramToken && paramToken->Tag == Tag::ID) {
                Word* wordToken = static_cast<Word*>(paramToken);
                paramName = wordToken ? wordToken->word : "";
            }
            parameters.push_back(make_pair(paramName, nullptr)); // 暂时使用nullptr
        }
    }
    
    return parameters;
}




