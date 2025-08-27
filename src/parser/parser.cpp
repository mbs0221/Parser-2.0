#include "parser/parser.h"
#include "parser/expression.h"
#include "parser/inter.h"

using namespace std;

// Parser构造函数和析构函数
Parser::Parser() {
    look = nullptr;
}

Parser::~Parser() {
    // 清理资源
}

// parse方法实现 - 返回AST
Program* Parser::parse(const string& file) {
    if (lex.open(file)) {
			look = lex.scan();
        return parseProgram();
    } else {
        printf("can't open %s.\n", file.c_str());
        return nullptr;
		}
}

void Parser::move() {
		look = lex.scan();
}

// 解析程序 - 程序根节点
Program* Parser::parseProgram() {
    Program* program = new Program();
    
    while (look->Tag != '#' && look->Tag != -1 && look->Tag != END_OF_FILE) {
        Statement* stmt = parseStatement();
        if (stmt) {
            program->addStatement(stmt);
        }
    }
    
    return program;
}

// 解析语句
Statement* Parser::parseStatement() {
    switch (look->Tag) {
        case IMPORT:
            return parseImportStatement();
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
            return parseThrowStatement();
        case TRY:
            return parseTryStatement();
        case CATCH:
            return parseCatchStatement();
        case FINALLY:
            return parseFinallyStatement();
        case SWITCH:
            return parseSwitchStatement();
        case CASE:
            return parseCaseStatement();
        case DEFAULT:
            return parseDefaultStatement();
        case FUNCTION:
            return parseFunction();
        case STRUCT:
            return parseStruct();
        case CLASS:
            return parseClass();
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
    match(IMPORT);
    
    // 解析字符串字面量作为模块名
    if (look->Tag != STR) {
        printf("SYNTAX ERROR line[%03d]: expected string literal after import\n", lex.line);
        exit(1);
    }
    
    String *moduleName = matchString();
    
    match(';');
    
    return new ImportStatement(moduleName);
}

// 解析变量声明语句 (let x = 10, y = 20, z;)
VariableDeclaration* Parser::parseVariableDeclaration() {
    match(LET);
    
    VariableDeclaration* decl = new VariableDeclaration();
    
    while (true) {
        // 解析标识符
        Token* idToken = look;
		match(ID);
        
        Word* wordToken = static_cast<Word*>(idToken);
        string name = wordToken ? wordToken->word : "";
        
        Expression* value = nullptr;
        
        // 检查是否有初始化表达式
        if (look->Tag == '=') {
            match('=');
            value = parseExpression();
        }
        
        // 添加变量到声明中
        decl->addVariable(name, value);
        
        // 检查是否还有更多变量（用逗号分隔）
        if (look->Tag == ',') {
					match(',');
        } else {
				break;
			}
		}
    
    // 匹配分号
    match(';');
    
    return decl;
}

// 解析表达式语句 (x + y;)
ExpressionStatement* Parser::parseExpressionStatement() {
    Expression* expr = parseExpression();
    match(';');
    return new ExpressionStatement(expr);
}

// 解析条件语句 (if (x > 0) { ... } else { ... })
IfStatement* Parser::parseIfStatement() {
    match(IF);
    match('(');
    
    Expression* condition = parseExpression();
    
    match(')');
    
    Statement* thenStmt = parseStatement();
    
    Statement* elseStmt = nullptr;
    if (look->Tag == ELSE) {
        match(ELSE);
        elseStmt = parseStatement();
    }
    
    return new IfStatement(condition, thenStmt, elseStmt);
}

// 解析循环语句 (while (x > 0) { ... })
WhileStatement* Parser::parseWhileStatement() {
    match(WHILE);
    match('(');
    
    Expression* condition = parseExpression();
    
    match(')');
    
    Statement* body = parseStatement();
    
    return new WhileStatement(condition, body);
}

// 解析For循环语句 (for (let i = 0; i < 10; i++) { ... })
ForStatement* Parser::parseForStatement() {
    match(FOR);
    match('(');
    
    Expression* init = nullptr;
    init = parseExpression();
    
    match(';');
    
    Expression* condition = nullptr;
    if (look->Tag != ';') {
        condition = parseExpression();
    }
    
    match(';');
    
    Expression* increment = nullptr;
    if (look->Tag != ')') {
        increment = parseExpression();
    }
    
    match(')');
    
    Statement* body = parseStatement();
    
    // 暂时返回while语句，实际应该创建for语句
    return new ForStatement(init, condition, increment, body);
}

// 解析break语句
BreakStatement* Parser::parseBreakStatement() {
    match(BREAK);
		match(';');
    return new BreakStatement();
}

// 解析continue语句
ContinueStatement* Parser::parseContinueStatement() {
    match(CONTINUE);
    match(';');
    return new ContinueStatement();
}

// 解析return语句
ReturnStatement* Parser::parseReturnStatement() {
    match(RETURN);
    Expression* returnValue = nullptr;
    if (look->Tag != ';') {
        returnValue = parseExpression();
    }
    match(';');
    return new ReturnStatement(returnValue);
}

// 解析throw语句
ThrowStatement* Parser::parseThrowStatement() {
    match(THROW);
    Expression* exception = parseExpression();
    match(';');
    return new ThrowStatement(exception);
}

// 解析try语句
TryStatement* Parser::parseTryStatement() {
    match(TRY);
    Statement* tryBlock = parseStatement();
    Statement* catchBlock = nullptr;
    Statement* finallyBlock = nullptr;
    if (look->Tag == CATCH) {
        match(CATCH);
			match('(');
        Token* exceptionToken = look;
			match(ID);
        
        string exceptionName = "";
        if (exceptionToken && exceptionToken->Tag == Tag::ID) {
            Word* wordToken = static_cast<Word*>(exceptionToken);
            exceptionName = wordToken ? wordToken->word : "";
        }
        match(')');
        catchBlock = parseStatement();
    }
    if (look->Tag == FINALLY) {
        match(FINALLY);
        finallyBlock = parseStatement();
    }
    return new TryStatement(tryBlock, catchBlock, finallyBlock);
}

// 解析switch语句
SwitchStatement* Parser::parseSwitchStatement() {
    match(SWITCH);
    Expression* condition = parseExpression();
    match('{');
    vector<CaseStatement*> cases;
    Statement* defaultCase = nullptr;
    
    // 解析case语句
    while (look->Tag == CASE) {
        match(CASE);
        Expression* condition = parseExpression();
        match(':');
        Statement* body = parseStatement();
        cases.push_back(new CaseStatement(condition, body));
    }
    
    // 解析default语句
    if (look->Tag == DEFAULT) {
        match(DEFAULT);
        defaultCase = parseStatement();
    }
    
    match('}');
    return new SwitchStatement(condition, cases, defaultCase);
}

// 解析语句块 ({ ... })
BlockStatement* Parser::parseBlock() {
    match('{');
    
    BlockStatement* block = new BlockStatement();
    
    while (look->Tag != '}') {
        Statement* stmt = parseStatement();
        if (stmt) {
            block->addStatement(stmt);
        }
    }
    
    match('}');
    
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
        if (!isBinaryOperator(look->Tag)) {
            break;
        }
        
        Operator* op = static_cast<Operator*>(look);
        int precedence = op->getPrecedence();
        
        // 如果操作符优先级低于最小优先级，停止解析
        if (precedence < minPrecedence) {
            break;
        }
        
        // 处理赋值操作符的特殊情况
        if (op->Tag == '=') {
            // 检查左操作数是否为变量引用
            if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(left)) {
                matchOperator();  // 消费赋值操作符
                // 对于赋值操作符，使用右结合性，所以递归调用时使用相同优先级
                Expression* right = parseExpressionWithPrecedence(precedence);
                left = new BinaryExpression(left, right, op);
            } else {
                printf("Error: Left side of assignment must be a variable\n");
                break;
            }
        } else {
            // 处理其他二元操作符
            matchOperator();  // 消费操作符
            
            // 对于左结合操作符，递归调用时使用更高优先级
            // 对于右结合操作符，递归调用时使用相同优先级
            int nextPrecedence = op->isLeftAssoc() ? precedence + 1 : precedence;
            Expression* right = parseExpressionWithPrecedence(nextPrecedence);
            
            left = new BinaryExpression(left, right, op);
        }
    }
    
    return left;
}

// 解析基本表达式（因子）
Expression* Parser::parsePrimary() {
    switch (look->Tag) {
        case '!': // 逻辑非
        case '-': // 负号
            Operator* op = matchOperator();
            Expression* operand = parsePrimary();
            return new UnaryExpression(op, operand);
        case ID: // 标识符
            return parseVariable();
        case NUM: // 整数
        case REAL: // 浮点数
        case BOOL: // 布尔值
        case STR: // 字符串
        case CHAR: // 字符
            return parseConstant();
        case '(': // 括号表达式
            matchToken('(');
            Expression* expr = parseExpressionWithPrecedence(0);
            matchToken(')');
            return expr;
        case '[': // 数组
            return parseArray();
        case '{': // 字典
            return parseDict();
        default:
            printf("SYNTAX ERROR line[%03d]: unexpected token in expression\n", lex.line);
            match(look->Tag);
            return nullptr;
    }
}

// 解析后缀操作符
Expression* Parser::parsePostfix(Expression* expr) {
    while (true) {
        switch (look->Tag) {
            case '.':
                // 成员访问
                matchToken('.');
                string memberName = matchIdentifier();
                
                if (look->Tag == '(') {
                    // 方法调用
                    matchToken('(');
                    vector<Expression*> arguments;
                    
                    if (look->Tag != ')') {
                        arguments.push_back(parseExpressionWithPrecedence(0));
                        
                        while (look->Tag == ',') {
                            matchToken(',');
                            arguments.push_back(parseExpressionWithPrecedence(0));
                        }
                    }
                    
                    matchToken(')');
                    expr = new MethodCallExpression(expr, memberName, arguments);
                } else {
                    // 成员访问
                    expr = new MemberAccessExpression(expr, memberName);
                }
                break;
                
            case '[':
                // 数组访问
                matchToken('[');
                Expression* index = parseExpressionWithPrecedence(0);
                matchToken(']');
                expr = new AccessExpression(expr, index);
                break;
                
            case '(':
                // 函数调用
                matchToken('(');
                vector<Expression*> arguments;
                
                if (look->Tag != ')') {
                    arguments.push_back(parseExpressionWithPrecedence(0));
                    
                    while (look->Tag == ',') {
                        matchToken(',');
                        arguments.push_back(parseExpressionWithPrecedence(0));
                    }
                }
                
                matchToken(')');
                expr = new CallExpression(expr, arguments);
                break;
                
            default:
                return expr;
        }
    }
}

// 检查是否为二元操作符
bool Parser::isBinaryOperator(int tag) {
    return tag == PLUS || tag == MINUS || tag == MULTIPLY || tag == DIVIDE || tag == MODULO ||
           tag == LT || tag == GT || tag == LE || tag == GE || tag == EQ_EQ || tag == NE_EQ || tag == AND_AND || tag == OR_OR || tag == ASSIGN;
}

// 解析标识符
Expression* Parser::parseVariable() {
    string name = matchIdentifier();
    VariableExpression* idExpr = new VariableExpression(name);
    
    // 向后看一个token，根据token类型决定是函数调用、数组访问、成员访问、结构体实例化还是类实例化
    switch (look->Tag) {
        case LPAREN:
            // 检查是否是类实例化还是函数调用
            // 这里我们需要区分类实例化和函数调用
            // 暂时都当作函数调用处理，在解释器中再区分
            return parseCall(idExpr);
        case LBRACKET:
        case DOT:
            return parseAccess(idExpr);
        case LBRACE:
            return parseStructInstantiation(idExpr);
        default:
            return idExpr;
    }
}

// 旧的parseParentheses方法已被统一的parsePrimary方法替代

// 解析常量
Expression* Parser::parseConstant() {
    switch (look->Tag) {
        case NUM:
            return new ConstantExpression(matchInt());
        case REAL:
            return new ConstantExpression(matchDouble());
        case STR:
            return new ConstantExpression(matchString());
        case CHAR:
            return new ConstantExpression(matchChar());
        case BOOL:
            return new ConstantExpression(matchBool());
        default:
            printf("SYNTAX ERROR line[%03d]: unexpected token in constant\n", lex.line);
            match(look->Tag);
            return nullptr;
    }
}

// 解析数组字面量
Expression* Parser::parseArray() {
    match('[');  // 匹配开始方括号
    
    Array* array = new Array();
    
    if (look->Tag != ']') {
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
        while (look->Tag == ',') {
            match(',');
            element = parseExpression();
            if (ConstantExpression* constExpr = dynamic_cast<ConstantExpression*>(element)) {
                array->addElement(constExpr->value);
            } else {
                array->addElement(nullptr);
            }
        }
    }
    
    match(']');  // 匹配结束方括号
    
    // 将Array包装在ConstantExpression中
    return new ConstantExpression(array);
}

// 解析字典字面量
Expression* Parser::parseDict() {
    match('{');  // 匹配开始大括号
    
    Dict* dict = new Dict();
    
    if (look->Tag != '}') {
        // 解析第一个键值对
        String* key = matchString();
        match(':');
        Expression* valueExpr = parseExpression();
        // 将Expression转换为Value
        if (ConstantExpression* constExpr = dynamic_cast<ConstantExpression*>(valueExpr)) {
            dict->setEntry(key->getValue(), constExpr->value);
        } else {
            // 对于非常量表达式，暂时使用nullptr
            dict->setEntry(key->getValue(), nullptr);
        }
        
        // 解析后续键值对
        while (look->Tag == ',') {
            match(',');
            key = matchString();
            match(':');
            valueExpr = parseExpression();
            if (ConstantExpression* constExpr = dynamic_cast<ConstantExpression*>(valueExpr)) {
                dict->setEntry(key->getValue(), constExpr->value);
            } else {
                dict->setEntry(key->getValue(), nullptr);
            }
        }
    }
    
    match('}');  // 匹配结束大括号
    
    // 将Dict包装在ConstantExpression中
    return new ConstantExpression(dict);
}

// 解析函数调用或类实例化
Expression* Parser::parseCall(VariableExpression* calleeExpr) {
    
		match('(');
    
    vector<Expression*> arguments;
    
    if (look->Tag != ')') {
        arguments.push_back(parseExpression());
        
        while (look->Tag == ',') {
            match(',');
            arguments.push_back(parseExpression());
        }
    }
    
		match(')');
    
    // 检查是否是类实例化
    // 这里我们需要检查calleeExpr是否是已定义的类名
    // 暂时简单地将所有带参数的调用都当作函数调用处理
    // 在实际实现中，我们需要检查类定义表
    
    // 检查是否是类名（首字母大写）
    string className = calleeExpr->name;
    if (!className.empty() && isupper(className[0])) {
        // 可能是类实例化
        return new ClassInstantiationExpression(calleeExpr, arguments);
    } else {
        // 可能是函数调用
        return new CallExpression(calleeExpr    , arguments);
    }
}

// 解析访问操作 - 统一Access类型，支持所有访问形式
Expression* Parser::parseAccess(VariableExpression* id) {
    Expression* target = id;
    
    while (look->Tag == '[' || look->Tag == '.') {
        Token* accessToken = look;
        
        if (look->Tag == '[') {
            // 数组访问：arr[index] 或 jsonArr[index]
            match('[');
            Expression* key = parseExpression();
			match(']');
            target = new AccessExpression(target, key, false);
        } else if (look->Tag == '.') {
            // 静态成员访问：obj.member
            match('.');
            Token* memberToken = look;
			match(ID);
            Word* wordToken = static_cast<Word*>(memberToken);
            StringLiteral* key = new StringLiteral(wordToken ? wordToken->word : "");
            target = new AccessExpression(target, key, true);
        }
    }
    
    return target;
}



// 解析函数声明 - 返回Prototype语句
FunctionPrototype* Parser::parsePrototype() {
    match(FUNCTION);
    
    // 解析函数名
    Token* funcNameToken = look;
		match(ID);
    
    string funcName = "";
    if (funcNameToken && funcNameToken->Tag == Tag::ID) {
        Word* wordToken = static_cast<Word*>(funcNameToken);
        funcName = wordToken ? wordToken->word : "";
    }
    
    // 解析参数列表
    match('(');
    vector<string> parameters;
    
    if (look->Tag != ')') {
        Token* paramToken = look;
        match(ID);
        
        string paramName = "";
        if (paramToken && paramToken->Tag == Tag::ID) {
            Word* wordToken = static_cast<Word*>(paramToken);
            paramName = wordToken ? wordToken->word : "";
        }
        parameters.push_back(paramName);
        
        while (look->Tag == ',') {
				match(',');
            paramToken = look;
            match(ID);
            
            paramName = "";
            if (paramToken && paramToken->Tag == Tag::ID) {
                Word* wordToken = static_cast<Word*>(paramToken);
                paramName = wordToken ? wordToken->word : "";
            }
            parameters.push_back(paramName);
        }
    }
    
    match(')');
    return new FunctionPrototype(funcName, parameters);
}

// 解析函数定义
FunctionDefinition* Parser::parseFunction() {
    FunctionPrototype* proto = parsePrototype();
    BlockStatement* body = parseBlock();
    return new FunctionDefinition(proto, body);
}

// 解析结构体定义
StructDefinition* Parser::parseStruct() {
    match(STRUCT);
    
    // 解析结构体名
    Token* structNameToken = look;
    match(ID);
    
    string structName = "";
    if (structNameToken && structNameToken->Tag == Tag::ID) {
        Word* wordToken = static_cast<Word*>(structNameToken);
        structName = wordToken ? wordToken->word : "";
    }
    
			match('{');
    
    vector<StructMember> members;
    
    while (look->Tag != '}') {
        // 解析成员类型
        Token* typeToken = look;
        
        // 支持基本类型和用户定义类型
        if (look->Tag == ID || look->Tag == STR || look->Tag == INT || look->Tag == DOUBLE) {
			match(look->Tag);
        } else {
            match(ID); // 默认情况
        }
        
        string memberType = "";
        if (typeToken) {
            if (typeToken->Tag == Tag::ID) {
                Word* wordToken = static_cast<Word*>(typeToken);
                memberType = wordToken ? wordToken->word : "";
            } else if (typeToken->Tag == Tag::STR) {
                memberType = "string";
            } else if (typeToken->Tag == Tag::INT) {
                memberType = "int";
            } else if (typeToken->Tag == Tag::DOUBLE) {
                memberType = "double";
            }
        }
        
        // 解析成员名
        Token* memberNameToken = look;
        match(ID);
        
        string memberName = "";
        if (memberNameToken && memberNameToken->Tag == Tag::ID) {
            Word* wordToken = static_cast<Word*>(memberNameToken);
            memberName = wordToken ? wordToken->word : "";
        }
        
        Expression* defaultValue = nullptr;
        
        // 检查是否有默认值
        if (look->Tag == '=') {
            match('=');
            defaultValue = parseExpression();
        }
        
        match(';');
        
        members.push_back(StructMember(memberName, memberType, defaultValue));
    }
    
			match('}');
    
    return new StructDefinition(structName, members);
}

// 解析类定义
ClassDefinition* Parser::parseClass() {
    match(CLASS);
    
    // 解析类名
    Token* classNameToken = look;
				match(ID);
    
    string className = "";
    if (classNameToken && classNameToken->Tag == Tag::ID) {
        Word* wordToken = static_cast<Word*>(classNameToken);
        className = wordToken ? wordToken->word : "";
    }
    
    string baseClass = "";
    
    // 检查是否有继承
    if (look->Tag == ':') {
			match(':');
        Token* baseToken = look;
				match(ID);
        
        if (baseToken && baseToken->Tag == Tag::ID) {
            Word* wordToken = static_cast<Word*>(baseToken);
            baseClass = wordToken ? wordToken->word : "";
        }
    }
    
    match('{');
    
    vector<ClassMember> members;
    vector<ClassMethod> methods;
    
    while (look->Tag != '}') {
        if (look->Tag == PUBLIC || look->Tag == PRIVATE || look->Tag == PROTECTED) {
            // 解析成员变量
            string visibility = "";
            if (look->Tag == PUBLIC) {
                match(PUBLIC);
                visibility = "public";
            } else if (look->Tag == PRIVATE) {
                match(PRIVATE);
                visibility = "private";
            } else if (look->Tag == PROTECTED) {
                match(PROTECTED);
                visibility = "protected";
            }
            
            // 解析类型和名称
            Token* typeToken = look;
            
            // 支持基本类型和用户定义类型
            if (look->Tag == ID || look->Tag == STR || look->Tag == INT || look->Tag == DOUBLE) {
                match(look->Tag);
            } else {
                match(ID); // 默认情况
            }
            
            string memberType = "";
            if (typeToken) {
                if (typeToken->Tag == Tag::ID) {
                    Word* wordToken = static_cast<Word*>(typeToken);
                    memberType = wordToken ? wordToken->word : "";
                } else if (typeToken->Tag == Tag::STR) {
                    memberType = "string";
                } else if (typeToken->Tag == Tag::INT) {
                    memberType = "int";
                } else if (typeToken->Tag == Tag::DOUBLE) {
                    memberType = "double";
                }
            }
            
            Token* memberNameToken = look;
            match(ID);
            
            string memberName = "";
            if (memberNameToken && memberNameToken->Tag == Tag::ID) {
                Word* wordToken = static_cast<Word*>(memberNameToken);
                memberName = wordToken ? wordToken->word : "";
            }
            
            Expression* defaultValue = nullptr;
            
            if (look->Tag == '=') {
                match('=');
                defaultValue = parseExpression();
            }
            
            match(';');
            
            members.push_back(ClassMember(memberName, memberType, visibility, defaultValue));
        } else if (look->Tag == FUNCTION) {
            // 解析方法
            methods.push_back(*parseClassMethod());
        } else {
            // 默认public成员
            Token* typeToken = look;
            
            // 支持基本类型和用户定义类型
            if (look->Tag == ID || look->Tag == STR || look->Tag == INT || look->Tag == DOUBLE) {
                match(look->Tag);
            } else {
                match(ID); // 默认情况
            }
            
            string memberType = "";
            if (typeToken) {
                if (typeToken->Tag == Tag::ID) {
                    Word* wordToken = static_cast<Word*>(typeToken);
                    memberType = wordToken ? wordToken->word : "";
                } else if (typeToken->Tag == Tag::STR) {
                    memberType = "string";
                } else if (typeToken->Tag == Tag::INT) {
                    memberType = "int";
                } else if (typeToken->Tag == Tag::DOUBLE) {
                    memberType = "double";
                }
            }
            
            Token* memberNameToken = look;
		match(ID);
            
            string memberName = "";
            if (memberNameToken && memberNameToken->Tag == Tag::ID) {
                Word* wordToken = static_cast<Word*>(memberNameToken);
                memberName = wordToken ? wordToken->word : "";
            }
            
            Expression* defaultValue = nullptr;
            
            if (look->Tag == '=') {
                match('=');
                defaultValue = parseExpression();
            }
            
            match(';');
            
            members.push_back(ClassMember(memberName, memberType, "public", defaultValue));
        }
    }
    
    match('}');
    
    return new ClassDefinition(className, members, methods, baseClass);
}

// 解析类方法
ClassMethod* Parser::parseClassMethod() {
    match(FUNCTION);
    
    // 解析方法名
    Token* methodNameToken = look;
    match(ID);
    
    string methodName = "";
    if (methodNameToken && methodNameToken->Tag == Tag::ID) {
        Word* wordToken = static_cast<Word*>(methodNameToken);
        methodName = wordToken ? wordToken->word : "";
    }
    
    // 解析参数列表
    match('(');
    vector<string> parameters;
    
    if (look->Tag != ')') {
        Token* paramToken = look;
        match(ID);
        
        string paramName = "";
        if (paramToken && paramToken->Tag == Tag::ID) {
            Word* wordToken = static_cast<Word*>(paramToken);
            paramName = wordToken ? wordToken->word : "";
        }
        parameters.push_back(paramName);
        
        while (look->Tag == ',') {
				match(',');
            paramToken = look;
            match(ID);
            
            paramName = "";
            if (paramToken && paramToken->Tag == Tag::ID) {
                Word* wordToken = static_cast<Word*>(paramToken);
                paramName = wordToken ? wordToken->word : "";
            }
            parameters.push_back(paramName);
        }
    }
    
    match(')');
    
    // 解析方法体
    BlockStatement* body = parseBlock();
    
    return new ClassMethod(methodName, parameters, "void", "public", body);
}

// 解析结构体实例化
Expression* Parser::parseStructInstantiation(VariableExpression* structName) {
			match('{');
    
    map<string, Expression*> fieldValues;
    
    if (look->Tag != '}') {
        // 解析第一个字段
        Token* fieldNameToken = look;
        match(ID);
        
        string fieldName = "";
        if (fieldNameToken && fieldNameToken->Tag == Tag::ID) {
            Word* wordToken = static_cast<Word*>(fieldNameToken);
            fieldName = wordToken ? wordToken->word : "";
        }
        
        match(':');
        Expression* value = parseExpression();
        fieldValues[fieldName] = value;
        
        // 解析更多字段
        while (look->Tag == ',') {
				match(',');
            
            fieldNameToken = look;
            match(ID);
            
            fieldName = "";
            if (fieldNameToken && fieldNameToken->Tag == Tag::ID) {
                Word* wordToken = static_cast<Word*>(fieldNameToken);
                fieldName = wordToken ? wordToken->word : "";
            }
            
            match(':');
            value = parseExpression();
            fieldValues[fieldName] = value;
        }
    }
    
			match('}');
    
    return new StructInstantiationExpression(structName, fieldValues);
}



// 解析成员访问
Expression* Parser::parseMemberAccess() {
    Expression* object = parseExpression();
    
    while (look->Tag == '.') {
        match('.');
        
        Token* memberToken = look;
        match(ID);
        
        string memberName = "";
        if (memberToken && memberToken->Tag == Tag::ID) {
            Word* wordToken = static_cast<Word*>(memberToken);
            memberName = wordToken ? wordToken->word : "";
        }
        
        object = new MemberAccessExpression(object, memberName);
    }
    
    return object;
}

// 解析方法调用
Expression* Parser::parseMethodCall() {
    Expression* object = parseExpression();
    
    while (look->Tag == '.') {
        match('.');
        
        Token* methodToken = look;
        match(ID);
        
        string methodName = "";
        if (methodToken && methodToken->Tag == Tag::ID) {
            Word* wordToken = static_cast<Word*>(methodToken);
            methodName = wordToken ? wordToken->word : "";
        }
        
        match('(');
        
        vector<Expression*> arguments;
        
        if (look->Tag != ')') {
            arguments.push_back(parseExpression());
            
            while (look->Tag == ',') {
                match(',');
                arguments.push_back(parseExpression());
            }
        }
        
        match(')');
        
        object = new MethodCallExpression(object, methodName, arguments);
    }
    
    return object;
}

// ==================== 私有Helper方法实现 ====================

// 匹配操作符
Operator* Parser::matchOperator() {
    if (look->Tag >= 32 && look->Tag <= 126) {
        Operator* op = static_cast<Operator*>(look);
        move();
        return op;
    }
    printf("SYNTAX ERROR line[%03d]: expected operator, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

// 语法分析器 - 匹配Tag预定义一个语法元素
bool Parser::match(int Tag) {
    if (look->Tag == Tag) {
        move();
        return true;
    }
    move();
    if (look->Tag > 255)
        printf("SYNTAX ERROR line[%03d]: expected %d, got %d\n", lex.line, Tag, look->Tag);
    else
        printf("SYNTAX ERROR line[%03d]: expected '%c', got '%c'\n", lex.line, (char)Tag, (char)look->Tag);
    exit(1);  // 强制退出
    return false;
}

// 匹配整数
Integer* Parser::matchInt() {
    if (look->Tag == NUM) {
        Integer* intVal = static_cast<Integer*>(look);
        move();
        return intVal;
    }
    printf("SYNTAX ERROR line[%03d]: expected integer, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

// 匹配浮点数
Double* Parser::matchDouble() {
    if (look->Tag == REAL) {
        Double* doubleVal = static_cast<Double*>(look);
        move();
        return doubleVal;
    }
    printf("SYNTAX ERROR line[%03d]: expected double, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

// 匹配字符
Char* Parser::matchChar() {
    if (look->Tag == CHAR) {
        Char* charVal = static_cast<Char*>(look);
        move();
        return charVal;
    }
    printf("SYNTAX ERROR line[%03d]: expected char, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

// 匹配布尔值
Bool* Parser::matchBool() {
    if (look->Tag == BOOL) {
        Bool* boolVal = static_cast<Bool*>(look);
        move();
        return boolVal;
    }
    printf("SYNTAX ERROR line[%03d]: expected bool, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

// 匹配字符串
String* Parser::matchString() {
    if (look->Tag == STR) {
        String* strVal = static_cast<String*>(look);
        move();
        return strVal;
    }
    printf("SYNTAX ERROR line[%03d]: expected string, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

// 匹配单词（标识符或关键字）
string Parser::matchIdentifier() {
    if (look->Tag == ID) {
        Word* word = static_cast<Word*>(look);
        move();
        return word ? word->word : "";
    }
    printf("SYNTAX ERROR line[%03d]: expected identifier, got %d\n", lex.line, look->Tag);
    exit(1);
}

// 匹配指定类型的token
void Parser::matchToken(int tag) {
    if (look->Tag == tag) {
        move();
        return;
    }
    if (look->Tag > 255) {
        printf("SYNTAX ERROR line[%03d]: expected %d, got %d\n", lex.line, tag, look->Tag);
    } else {
        printf("SYNTAX ERROR line[%03d]: expected '%c', got '%c'\n", lex.line, (char)tag, (char)look->Tag);
    }
    exit(1);
}
