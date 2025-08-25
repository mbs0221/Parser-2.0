#include "parser.h"
#include "inter.cpp"
#include "inter.h"

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

// 语法分析器 - 匹配Tag预定义一个语法元素
bool Parser::match(int Tag) {
    if (look->Tag == Tag) {
        move();
        return true;
    }
    move();
    if (look->Tag > 255)
        printf("line[%03d]:%d not matched.\n", lex.line, look->Tag);
    else
        printf("line[%03d]:%c not matched.\n", lex.line, (char)look->Tag);
    return false;
}

// 解析程序 - 程序根节点
Program* Parser::parseProgram() {
    Program* program = new Program();
    
    while (look->Tag != '#' && look->Tag != -1) {
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
        case PRINT:
            return parsePrintStatement();
        case ID:
            return parseExpressionStatement();
        case '{':
            return parseBlock();
        default:
            printf("Error[%03d]: unexpected token in statement\n", lex.line);
            match(look->Tag);
            return nullptr;
    }
}

// 解析变量声明语句 (let x = 10;)
VariableDeclaration* Parser::parseVariableDeclaration() {
    match(LET);
    
    // 解析标识符
    Token* idToken = look;
    match(ID);
    
    Word* wordToken = static_cast<Word*>(idToken);
    string name = wordToken ? wordToken->word : "";
    
    // 匹配等号
    match('=');
    
    // 解析表达式
    Expression* value = parseExpression();
    
    // 匹配分号
    match(';');
    
    return new VariableDeclaration(name, value);
}

// 解析表达式语句 (x + y;)
ExpressionStatement* Parser::parseExpressionStatement() {
    Expression* expr = parseExpression();
    match(';');
    return new ExpressionStatement(expr);
}

// 解析print语句 (print("Hello");)
ExpressionStatement* Parser::parsePrintStatement() {
    match(PRINT);
    match('(');
    
    Expression* expr = parseExpression();
    
    match(')');
    match(';');
    
    // 创建print函数调用
    IdentifierExpression* printFunc = new IdentifierExpression("print");
    vector<Expression*> args;
    args.push_back(expr);
    CallExpression* callExpr = new CallExpression(printFunc, args);
    
    return new ExpressionStatement(callExpr);
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
    return new TryStatement(tryBlock, catchBlock);
}

// 解析catch语句
CatchStatement* Parser::parseCatchStatement() {
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
    Statement* catchBlock = parseStatement();
    return new CatchStatement(exceptionName, catchBlock);
}

// 解析finally语句
FinallyStatement* Parser::parseFinallyStatement() {
    match(FINALLY);
    match('{');
    Statement* finallyBlock = parseStatement();
    match('}');
    return new FinallyStatement(finallyBlock);
}

// 解析switch语句
SwitchStatement* Parser::parseSwitchStatement() {
    match(SWITCH);
    Expression* condition = parseExpression();
    match('{');
    vector<CaseStatement*> cases;
    Statement* defaultCase = nullptr;
    
    while (look->Tag == CASE) {
        CaseStatement* caseStmt = parseCaseStatement();
        cases.push_back(caseStmt);
    }
    
    if (look->Tag == DEFAULT) {
        match(DEFAULT);
        defaultCase = parseStatement();
    }
    
    match('}');
    return new SwitchStatement(condition, cases, defaultCase);
}

// 解析case语句
CaseStatement* Parser::parseCaseStatement() {
    match(CASE);
    Expression* condition = parseExpression();
    match(':');
    Statement* body = parseStatement();
    return new CaseStatement(condition, body);
}

// 解析default语句
DefaultStatement* Parser::parseDefaultStatement() {
    match(DEFAULT);
    Statement* body = parseStatement();
    return new DefaultStatement(body);
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

// 解析加减运算
Expression* Parser::parseAdditive() {
    Expression* left = parseTerm();
    
    while (look->Tag == '+' || look->Tag == '-') {
        Token* op = look;
        match(look->Tag);
        Expression* right = parseTerm();
        left = new ArithmeticExpression(left, op, right);
    }
    
    return left;
}

// 解析表达式
Expression* Parser::parseExpression() {
    return parseCompare();
}

// 解析比较运算
Expression* Parser::parseCompare() {
    Expression* left = parseAdditive();
    while (look->Tag == '>' || look->Tag == '<' || look->Tag == GE || look->Tag == BE || 
           look->Tag == EQ || look->Tag == NE || look->Tag == AND || look->Tag == OR) {
        Token* op = look;
        match(look->Tag);
        Expression* right = parseAdditive();
        left = new ArithmeticExpression(left, op, right);
    }
    return left;
}

// 解析项
Expression* Parser::parseTerm() {
    Expression* left = parseFactor();
    
    while (look->Tag == '*' || look->Tag == '/') {
        Token* op = look;
        match(look->Tag);
        Expression* right = parseFactor();
        left = new ArithmeticExpression(left, op, right);
    }
    
    return left;
}

// 解析因子
Expression* Parser::parseFactor() {
    // 处理一元操作符
    if (look->Tag == '!') {
        Token* op = look;
        match('!');
        Expression* operand = parseFactor();
        if (!operand) return nullptr;
        
        // 创建一元表达式
        return new UnaryExpression(op, operand);
    }
    
    if (look->Tag == ID) {
        Token* idToken = look;
        match(ID);
        
        // 检查是否是函数调用
        if (look->Tag == '(') {
            return parseCall(idToken);
        }
        
        // 检查是否是访问操作
        if (look->Tag == '[' || look->Tag == '.') {
            string name = "";
            if (idToken && idToken->Tag == Tag::ID) {
                Word* wordToken = static_cast<Word*>(idToken);
                name = wordToken ? wordToken->word : "";
            }
            return parseAccess(new IdentifierExpression(name));
        }
        
        // 普通标识符
        string name = "";
        if (idToken && idToken->Tag == Tag::ID) {
            Word* wordToken = static_cast<Word*>(idToken);
            name = wordToken ? wordToken->word : "";
        }
        return new IdentifierExpression(name);
    } else if (look->Tag == NUM) {
        Integer* intToken = static_cast<Integer*>(look);
        int value = intToken ? intToken->value : 0;
        match(NUM);
        return new NumberExpression(value);
    } else if (look->Tag == STR) {
        // 字符串字面量
        return parseStringLiteral();
    } else if (look->Tag == '[') {
        // 数组字面量
        return parseArray();
    } else if (look->Tag == '{') {
        // 字典字面量
        return parseDict();
    } else if (look->Tag == '(') {
        match('(');
        Expression* expr = parseExpression();
        match(')');
        return expr;
    } else {
        printf("Error: unexpected token in factor\n");
        match(look->Tag);
        return nullptr;
    }
}

// 解析字符串字面量
StringLiteral* Parser::parseStringLiteral() {
    // 词法分析器已经处理了字符串，直接获取当前token
    Word* wordToken = static_cast<Word*>(look);
    string word = wordToken ? wordToken->word : "";
    match(STR);
    return new StringLiteral(word);
}

// 解析数组字面量
ArrayNode* Parser::parseArray() {
    match('[');  // 匹配开始方括号
    
    ArrayNode* array = new ArrayNode();
    
    if (look->Tag != ']') {
        // 解析第一个元素
        Expression* element = parseExpression();
        array->addElement(element);
        
        // 解析后续元素
        while (look->Tag == ',') {
            match(',');
            element = parseExpression();
            array->addElement(element);
        }
    }
    
    match(']');  // 匹配结束方括号
    
    return array;
}

// 解析字典字面量
DictNode* Parser::parseDict() {
    match('{');  // 匹配开始大括号
    
    DictNode* dict = new DictNode();
    
    if (look->Tag != '}') {
        // 解析第一个键值对
        StringLiteral* key = parseStringLiteral();
        match(':');
        Expression* value = parseExpression();
        dict->setEntry(key->toString(), value);
        
        // 解析后续键值对
        while (look->Tag == ',') {
            match(',');
            StringLiteral* key = parseStringLiteral();
            match(':');
            Expression* value = parseExpression();
            dict->setEntry(key->toString(), value);
        }
    }
    
    match('}');  // 匹配结束大括号
    
    return dict;
}

// 解析函数调用
CallExpression* Parser::parseCall(Token* funcToken) {
    string name = "";
    if (funcToken && funcToken->Tag == Tag::ID) {
        Word* wordToken = static_cast<Word*>(funcToken);
        name = wordToken ? wordToken->word : "";
    }
    IdentifierExpression* calleeExpr = new IdentifierExpression(name);
    
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
    
    return new CallExpression(calleeExpr, arguments);
}

// 解析访问操作 - 统一Access类型，支持所有访问形式
AccessExpression* Parser::parseAccess(IdentifierExpression* id) {
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
    
    return dynamic_cast<AccessExpression*>(target);
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
