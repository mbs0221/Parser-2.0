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
        case SWITCH:
            return parseSwitchStatement();
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
    
    String *moduleName = matchValue<String>();
    
    match(';');
    
    return new ImportStatement(moduleName);
}

// 解析变量声明语句 (let x = 10, y = 20, z;)
VariableDeclaration* Parser::parseVariableDeclaration() {
    match(LET);
    VariableDeclaration* decl = new VariableDeclaration();
    
    while (true) {
        // 解析标识符
        string name = matchIdentifier();
        
        Expression* value = nullptr;
        
        // 检查是否有初始化表达式
        if (look->Tag == ASSIGN) {
            match(ASSIGN);
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
    
    Statement* init = nullptr;
    if (look->Tag != ';') {
        // 将初始化表达式包装成表达式语句
        Expression* initExpr = parseExpression();
        init = new ExpressionStatement(initExpr);
    }
    
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
    printf("DEBUG: parseReturnStatement() - starting\n");
    match(RETURN);
    Expression* returnValue = nullptr;
    if (look->Tag != ';') {
        printf("DEBUG: parseReturnStatement() - parsing return value\n");
        returnValue = parseExpression();
    }
    printf("DEBUG: parseReturnStatement() - expecting ';' token\n");
    match(';');
    printf("DEBUG: parseReturnStatement() - ';' matched\n");
    return new ReturnStatement(returnValue);
}

// 解析throw语句
ThrowStatement* Parser::parseThrowStatement() {
    match(THROW);
    Expression* exception = parseExpression();
    match(';'); 
    return new ThrowStatement(exception);
}

// 解析try语句 - 合并了catch和finally
TryStatement* Parser::parseTryStatement() {
    match(TRY);
    Statement* tryBlock = parseStatement();
    vector<TryStatement::CatchBlock> catchBlocks;
    Statement* finallyBlock = nullptr;
    
    while (look->Tag == CATCH) {
        match(CATCH);
        match(LPAREN);
        string exceptionType = "Exception"; // 默认异常类型
        string exceptionName = matchIdentifier();
        match(RPAREN);
        Statement* catchBody = parseStatement();
        catchBlocks.push_back(TryStatement::CatchBlock(exceptionType, exceptionName, catchBody));
    }
    
    if (look->Tag == FINALLY) {
        match(FINALLY);
        finallyBlock = parseStatement();
    }
    
    return new TryStatement(tryBlock, catchBlocks, finallyBlock);
}

// 解析switch语句 - 合并了case和default
SwitchStatement* Parser::parseSwitchStatement() {
    match(SWITCH);
    Expression* condition = parseExpression();
    match('{');
    vector<SwitchStatement::SwitchCase> cases;
    
    // 解析case语句
    while (look->Tag == CASE) {
        match(CASE);
        Expression* caseValue = parseExpression();
        match(':');
        Statement* caseBody = parseStatement();
        vector<Statement*> caseStatements = {caseBody};
        cases.push_back(SwitchStatement::SwitchCase(caseValue, caseStatements));
    }
    
    // 解析default语句
    if (look->Tag == DEFAULT) {
        match(DEFAULT);
        Statement* defaultBody = parseStatement();
        vector<Statement*> defaultStatements = {defaultBody};
        cases.push_back(SwitchStatement::SwitchCase(nullptr, defaultStatements));
    }
    
    match('}');
    return new SwitchStatement(condition, cases);
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
        
        // 获取操作符
        Operator* op = matchOperator();
        int precedence = op->getPrecedence();
        bool isLeftAssoc = op->isLeftAssoc();
        
        // 如果操作符优先级低于最小优先级，停止解析
        if (precedence < minPrecedence) {
            break;
        }
        
        // 处理赋值操作符的特殊情况
        if (op->Tag == ASSIGN) {
            // 检查左操作数是否为变量引用
            if (VariableExpression* varExpr = dynamic_cast<VariableExpression*>(left)) {
                // 对于赋值操作符，使用右结合性，所以递归调用时使用相同优先级
                Expression* right = parseExpressionWithPrecedence(precedence);
                left = new BinaryExpression(left, right, op);
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
    switch (look->Tag) {
        case NOT: // 逻辑非
        case '-': // 负号 (ASCII 45)
        case MINUS: // 负号 (枚举 301)
            {
                Operator* op = matchOperator();
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
                matchToken('(');
                Expression* expr = parseExpressionWithPrecedence(0);
                matchToken(')');
                return expr;
            }
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
                {
                    // 成员访问
                    match('.');
                    string memberName = matchIdentifier();
                    
                    if (look->Tag == '(') {
                        // 方法调用：先访问方法，然后调用
                        // 创建访问表达式获取方法
                        ConstantExpression* memberNameExpr = new ConstantExpression(new String(memberName));
                        Expression* methodAccess = new AccessExpression(expr, memberNameExpr);
                        
                        // 然后作为函数调用，将对象作为第一个参数
                        matchToken('(');
                        vector<Expression*> arguments;
                        
                        // 将对象作为第一个参数（类似this指针）
                        arguments.push_back(expr);
                        
                        if (look->Tag != ')') {
                            arguments.push_back(parseExpressionWithPrecedence(0));
                            
                            while (look->Tag == ',') {
                                matchToken(',');
                                arguments.push_back(parseExpressionWithPrecedence(0));
                            }
                        }
                        
                        matchToken(')');
                        expr = new CallExpression(memberName, arguments);
                    } else {
                        // 成员访问
                        // 创建字符串常量作为成员名
                        ConstantExpression* memberNameExpr = new ConstantExpression(new String(memberName));
                        expr = new AccessExpression(expr, memberNameExpr);
                    }
                }
                break;
                
            case '[':
                // 数组访问
                {
                    matchToken('[');
                    Expression* index = parseExpressionWithPrecedence(0);
                    matchToken(']');
                    expr = new AccessExpression(expr, index);
                }
                break;
                
            case '(':
                // 函数调用
                {
                    expr = parseCallExpression(expr);
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
        tag == '<' || tag == '>' || tag == '=' || tag == '!' || tag == '&' || 
        tag == '|' || tag == '^' || tag == '~') {
        return true;
    }
    
    // 检查多字符操作符（枚举类型）
    return tag == PLUS || tag == MINUS || tag == MULTIPLY || tag == DIVIDE || tag == MODULO ||
           tag == LT || tag == GT || tag == LE || tag == GE || tag == EQ_EQ || tag == NE_EQ || 
           tag == AND_AND || tag == OR_OR || tag == ASSIGN || tag == BIT_AND || tag == BIT_OR || 
           tag == BIT_XOR || tag == LEFT_SHIFT || tag == RIGHT_SHIFT;
}

// 解析标识符
Expression* Parser::parseVariable() {
    string name = matchIdentifier();
    VariableExpression* idExpr = new VariableExpression(name);
    
    // 只返回变量表达式，函数调用、数组访问等由parsePostfix处理
    return idExpr;
}

// 解析常量
Expression* Parser::parseConstant() {
    // 根据当前token类型直接调用相应的matchValue函数
    // matchValue内部会处理类型检查和错误
    switch (look->Tag) {
        case NUM:
            return new ConstantExpression(matchValue<Integer>());
        case REAL:
            return new ConstantExpression(matchValue<Double>());
        case STR:
            return new ConstantExpression(matchValue<String>());
        case CHAR:
            return new ConstantExpression(matchValue<Char>());
        case BOOL:
            return new ConstantExpression(matchValue<Bool>());
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
        String* key = matchValue<String>();
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
            key = matchValue<String>();
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



// 解析通用函数调用表达式
Expression* Parser::parseCallExpression(Expression* calleeExpr) {
    // 需要跳过'('token，进入函数参数列表
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
    match(FUNCTION);
    
    // 解析函数名
    string funcName = matchIdentifier();
    
    // 解析参数列表
    match('(');
    vector<pair<string, Type*>> parameters = parseParameterList();
    match(')');
    
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
    match(STRUCT);
    string structName = matchIdentifier();
	match('{');
    
    vector<StructMember> members;
    
    while (look->Tag != '}') {
        // 结构体成员默认为public
        members.push_back(parseClassMember("public"));
    }
    
	match('}');
    
    return new StructDefinition(structName, members);
}

// 解析类定义
ClassDefinition* Parser::parseClass() {
    match(CLASS);
    
    // 解析类名
    string className = matchIdentifier();
    
    string baseClass = "";
    if (look->Tag == ':') {
		match(':');
        baseClass = matchIdentifier();
    } else {
        baseClass = "Object";
    }
    
    match('{');
    
    vector<StructMember> members;
    vector<ClassMethod*> methods;
    
    // 当前访问修饰符，默认为public
    string currentVisibility = "public";
    
    while (look->Tag != '}') {
        if (look->Tag == PUBLIC || look->Tag == PRIVATE || look->Tag == PROTECTED) {
            // 更新当前访问修饰符
            Visibility* visibilityToken = static_cast<Visibility*>(look);
            currentVisibility = visibilityToken->toString();
            move(); // 消费访问修饰符
            
            // 继续解析后续的成员，使用新的访问修饰符
            continue;
        } else if (look->Tag == FUNCTION) {
            // 解析方法，使用当前访问修饰符
            ClassMethod* method = parseClassMethod(currentVisibility);
            methods.push_back(method);
        } else {
            // 解析成员变量，使用当前访问修饰符
            members.push_back(parseClassMember(currentVisibility));
        }
    }
    
    match('}');
    
    return new ClassDefinition(className, baseClass, members, methods);
}

// 解析类成员变量
StructMember Parser::parseClassMember(const string& visibility) {
    // 解析类型和名称
    Type* memberType = matchType();
    
    string memberName = matchIdentifier();
    
    Expression* defaultValue = nullptr;
    
    if (look->Tag == '=') {
        match('=');
        defaultValue = parseExpression();
    }
    
    match(';');
    
    return StructMember(memberName, memberType, visibility, defaultValue);
}

// 解析类方法
ClassMethod* Parser::parseClassMethod(const string& visibility) {
    match(FUNCTION);
    
    // 解析方法名
    string methodName = matchIdentifier();
    
    // 解析参数列表
    match('('); 
    vector<pair<string, Type*>> parameters = parseParameterList();
    match(')');
    
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
    
    if (look->Tag != ')') {
        Token* paramToken = look;
        match(ID);
        
        string paramName = "";
        if (paramToken && paramToken->Tag == Tag::ID) {
            Word* wordToken = static_cast<Word*>(paramToken);
            paramName = wordToken ? wordToken->word : "";
        }
        parameters.push_back(make_pair(paramName, nullptr)); // 暂时使用nullptr，后续可以解析类型
        
        while (look->Tag == ',') {
            match(',');
            paramToken = look;
            match(ID);
            
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

// 解析结构体实例化
Expression* Parser::parseStructInstantiation(VariableExpression* structName) {
	match('{');
    
    map<string, Expression*> fieldValues;
    
    if (look->Tag != '}') {
        // 解析第一个字段
        string fieldName = matchIdentifier();
        
        match(':');
        Expression* value = parseExpression();
        fieldValues[fieldName] = value;
        
        // 解析更多字段
        while (look->Tag == ',') {
			match(',');
            
            fieldName = matchIdentifier();
            
            match(':');
            value = parseExpression();
            fieldValues[fieldName] = value;
        }
    }
    
	match('}');
    
    	// 结构体实例化现在使用CallExpression
	return new CallExpression(structName->name, vector<Expression*>());
}



// 解析成员访问
Expression* Parser::parseMemberAccess() {
    Expression* object = parseExpression();
    
    while (look->Tag == '.') {
        match('.');
        
        string memberName = matchIdentifier();
        
        // 创建字符串常量作为成员名
        ConstantExpression* memberNameExpr = new ConstantExpression(new String(memberName));
        object = new AccessExpression(object, memberNameExpr);
    }
    
    return object;
}

// 解析方法调用
Expression* Parser::parseMethodCall() {
    Expression* object = parseExpression();
    
    while (look->Tag == '.') {
        match('.');
        
        string methodName = matchIdentifier();
        
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
    // 检查是否为运算符（ASCII 字符或 enum 类型）
    if (isOperator(look->Tag)) {
        Operator* op = static_cast<Operator*>(look);
        move();
        return op;
    }
    printf("SYNTAX ERROR line[%03d]: expected operator, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
}

// 判断是否为运算符
bool Parser::isOperator(int tag) {
    // ASCII 字符运算符
    if (tag >= 32 && tag <= 126) {
        switch (tag) {
            case '+': case '-': case '*': case '/': case '%':
            case '<': case '>': case '=': case '!':
            case '&': case '|': case '^': case '~':
            case '?': case ':': case '.':
                return true;
        }
    }
    
    // enum 类型运算符
    if (tag >= 300 && tag <= 399) {
        return true;
    }
    
    return false;
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

// 匹配类型
Type* Parser::matchType() {
    if (look->Tag == ID) {
        // 用户定义类型，暂时返回Type::Int作为默认值
        // 这里可以根据需要扩展为查找用户定义的类型
        move();
        return Type::Int;
    } else if (look->Tag == STR) {
        move();
        return Type::String;
    } else if (look->Tag == NUM) {
        move();
        return Type::Int;
    } else if (look->Tag == DOUBLE) {
        move();
        return Type::Double;
    } else if (look->Tag == CHAR) {
        move();
        return Type::Char;
    } else if (look->Tag == BOOL) {
        move();
        return Type::Bool;
    }
    printf("SYNTAX ERROR line[%03d]: expected type, got %d\n", lex.line, look->Tag);
    exit(1);
    return nullptr;
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


