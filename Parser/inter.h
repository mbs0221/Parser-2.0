#ifndef INTER_H
#define INTER_H
#include "lexer.h"
#include <string>
#include <list>
#include <map>
#include <vector>
#include <memory>
#include <stdexcept>
#include <sstream>

using namespace std;

// ==================== AST基类 ====================
// 抽象语法树节点基类
struct AST {
    virtual ~AST() = default;
    
    // 节点类型枚举
    enum class Type {
        EXPRESSION, // 表达式节点
        STATEMENT,  // 语句节点
        DECLARATION, // 声明节点
        FUNCTION,   // 函数类型
        ARRAY,      // 数组类型
        STRING,     // 字符串类型
        DICT,       // 字典类型
        STRUCT,     // 结构体类型
        CLASS       // 类类型
    };
    
    // 获取节点类型
    virtual Type getType() const = 0;
    
    // 获取节点位置信息（用于错误报告）
    virtual string getLocation() const { return "unknown"; }
    
    // 获取节点的字符串表示
    virtual string toString() const = 0;
};

// ==================== 语法单元 ====================
// 表达式基类
struct Expression : public AST {
    Type getType() const override { return Type::EXPRESSION; }
    
    // 表达式求值（在解释器中实现）
    virtual string toString() const override { return "expression"; }
};



// 数字表达式节点 - 现在是语法树节点，不是词法单元
struct NumberExpression : public Expression {
    int value;
    
    NumberExpression(int val) : value(val) {}
    
    // 获取数值
    int getIntValue() const {
        return value;
    }
    
    string toString() const override {
        return to_string(value);
    }
    
    // 运算符重载 - 算术运算
    NumberExpression operator+(const NumberExpression& other) const {
        return NumberExpression(value + other.value);
    }
    
    NumberExpression operator-(const NumberExpression& other) const {
        return NumberExpression(value - other.value);
    }
    
    NumberExpression operator*(const NumberExpression& other) const {
        return NumberExpression(value * other.value);
    }
    
    NumberExpression operator/(const NumberExpression& other) const {
        if (other.value == 0) {
            throw runtime_error("Division by zero");
        }
        return NumberExpression(value / other.value);
    }
    
    NumberExpression operator%(const NumberExpression& other) const {
        if (other.value == 0) {
            throw runtime_error("Modulo by zero");
        }
        return NumberExpression(value % other.value);
    }
    
    // 运算符重载 - 比较运算
    NumberExpression operator==(const NumberExpression& other) const {
        return NumberExpression(value == other.value ? 1 : 0);
    }
    
    NumberExpression operator!=(const NumberExpression& other) const {
        return NumberExpression(value != other.value ? 1 : 0);
    }
    
    NumberExpression operator<(const NumberExpression& other) const {
        return NumberExpression(value < other.value ? 1 : 0);
    }
    
    NumberExpression operator>(const NumberExpression& other) const {
        return NumberExpression(value > other.value ? 1 : 0);
    }
    
    NumberExpression operator<=(const NumberExpression& other) const {
        return NumberExpression(value <= other.value ? 1 : 0);
    }
    
    NumberExpression operator>=(const NumberExpression& other) const {
        return NumberExpression(value >= other.value ? 1 : 0);
    }
    
    // 运算符重载 - 逻辑运算
    NumberExpression operator&&(const NumberExpression& other) const {
        return NumberExpression((value != 0 && other.value != 0) ? 1 : 0);
    }
    
    NumberExpression operator||(const NumberExpression& other) const {
        return NumberExpression((value != 0 || other.value != 0) ? 1 : 0);
    }
    
    // 一元逻辑非操作符
    NumberExpression operator!() const {
        return NumberExpression(value == 0 ? 1 : 0);
    }
    
    // 一元负号操作符
    NumberExpression operator-() const {
        return NumberExpression(-value);
    }
    

};

// 标识符表达式
struct IdentifierExpression : public Expression {
    string name;
    
    IdentifierExpression(const string& n) : name(n) {}
    
    string getName() const {
        return name;
    }
    
    string toString() const override {
        return name;
    }
};

// 一元表达式
struct UnaryExpression : public Expression {
    Token* operator_;  // 操作符Token
    Expression* operand;
    
    UnaryExpression(Token* op, Expression* operand) 
        : operator_(op), operand(operand) {}
    
    string toString() const override {
        string opStr = "";
        if (operator_) {
            if (operator_->Tag == '!') {
                opStr = "!";
            } else if (operator_->Tag == '-') {
                opStr = "-";
            } else {
                Word* wordToken = static_cast<Word*>(operator_);
                opStr = wordToken ? wordToken->word : "";
            }
        }
        return "(" + opStr + (operand ? operand->toString() : "") + ")";
    }
};

// 算术表达式
struct ArithmeticExpression : public Expression {
    Expression* left;
    Token* operator_;  // 操作符Token
    Expression* right;
    
    ArithmeticExpression(Expression* l, Token* op, Expression* r) 
        : left(l), operator_(op), right(r) {}
    
    string toString() const override {
        string opStr = "";
        if (operator_) {
            // 处理单字符操作符
            if (operator_->Tag == '+') opStr = "+";
            else if (operator_->Tag == '-') opStr = "-";
            else if (operator_->Tag == '*') opStr = "*";
            else if (operator_->Tag == '/') opStr = "/";
            else if (operator_->Tag == '%') opStr = "%";
            else if (operator_->Tag == EQ) opStr = "==";
            else if (operator_->Tag == NE) opStr = "!=";
            else if (operator_->Tag == '<') opStr = "<";
            else if (operator_->Tag == '>') opStr = ">";
            else if (operator_->Tag == BE) opStr = "<=";
            else if (operator_->Tag == GE) opStr = ">=";
            else if (operator_->Tag == AND) opStr = "&&";
            else if (operator_->Tag == OR) opStr = "||";
            else {
                // 尝试作为Word处理（用于其他操作符）
                Word* wordToken = static_cast<Word*>(operator_);
                opStr = wordToken ? wordToken->word : "";
            }
        }
        return "(" + (left ? left->toString() : "") + " " + opStr + " " + (right ? right->toString() : "") + ")";
    }
};

// 赋值表达式
struct AssignmentExpression : public Expression {
    string variableName;  // 变量名
    Expression* value;    // 要赋的值
    
    AssignmentExpression(const string& varName, Expression* val) 
        : variableName(varName), value(val) {}
    
    string toString() const override {
        return variableName + " = " + (value ? value->toString() : "");
    }
};

// 字符串拼接表达式
struct StringConcatenationExpression : public Expression {
    Expression* left;
    Expression* right;
    
    StringConcatenationExpression(Expression* l, Expression* r) 
        : left(l), right(r) {}
    
    string toString() const override {
        return "(" + (left ? left->toString() : "") + " + " + 
               (right ? right->toString() : "") + ")";
    }
};

// 字符表达式
struct CharExpression : public Expression {
    char value;
    
    CharExpression(char val) : value(val) {}
    
    string toString() const override {
        return string(1, value);
    }
};

// 数组节点 - 支持元素访问的语法树节点
struct ArrayNode : public Expression {
    vector<Expression*> elements;
    
    ArrayNode() {}
    
    // 添加元素
    void addElement(Expression* element) {
        elements.push_back(element);
    }
    
    // 获取元素
    Expression* getElement(size_t index) const {
        return index < elements.size() ? elements[index] : nullptr;
    }
    
    // 设置元素
    void setElement(size_t index, Expression* element) {
        if (index < elements.size()) {
            elements[index] = element;
        } else if (index == elements.size()) {
            elements.push_back(element);
        }
    }
    
    // 获取元素数量
    size_t getElementCount() const {
        return elements.size();
    }
    
    // 获取数组大小
    size_t size() const {
        return elements.size();
    }
    
    string toString() const override {
        string result = "[";
        for (size_t i = 0; i < elements.size(); ++i) {
            if (i > 0) result += ", ";
            result += elements[i]->toString();
        }
        result += "]";
        return result;
    }
    
    // 访问操作 - 将key作为数组下标
    virtual Expression* access(Expression* key) {
        // 将key转换为数字索引
        string keyStr = key->toString();
        try {
            int index = stoi(keyStr);
            return index >= 0 && index < (int)elements.size() ? elements[index] : nullptr;
        } catch (const exception&) {
            // 如果转换失败，返回nullptr
            return nullptr;
        }
    }
};

// 字符串节点 - 继承自数组，元素是字符
struct StringNode : public ArrayNode {
    StringNode(const string& str) {
        // 将字符串的每个字符作为数组元素
        for (char c : str) {
            // 使用NumberExpression表示字符的ASCII值
            addElement(new CharExpression(c));
        }
    }
    
    // 设置字符串值
    void setValue(const string& str) {
        // 清空现有元素并重新添加
        elements.clear();
        for (char c : str) {
            // 使用NumberExpression表示字符的ASCII值
            addElement(new CharExpression(c));
        }
    }
    
    // 获取字符串长度
    size_t length() const {
        return elements.size();
    }
    
    // 字符串拼接
    void append(const string& str) {
        // 添加新的字符元素
        for (char c : str) {
            addElement(new CharExpression(c));
        }
    }
    
    // 字符串拼接
    void append(char c) {
        addElement(new CharExpression(c));
    }
    
    // 子字符串
    string substring(size_t start, size_t length) const {
        if (start >= elements.size()) return "";
        string result = "";
        for (size_t i = start; i < start + length; ++i) {
            result += elements[i]->toString();
        }
        return result;
    }
    
    string toString() const override {
        string result = "";
        for (size_t i = 0; i < elements.size(); ++i) {
            result += elements[i]->toString();
		}
		return result;
	}
    
    // 访问操作 - 继承自ArrayNode，返回字符节点
    Expression* access(Expression* key) override {
        // 调用父类的access方法，返回字符节点
        return ArrayNode::access(key);
    }
    
    // 运算符重载 - 字符串拼接
    StringNode operator+(const StringNode& other) const {
        StringNode result = *this;
        result.append(other.toString());
        return result;
    }
    
    // 运算符重载 - 字符串比较
    NumberExpression operator==(const StringNode& other) const {
        return NumberExpression(toString() == other.toString() ? 1 : 0);
    }
    
    NumberExpression operator!=(const StringNode& other) const {
        return NumberExpression(toString() != other.toString() ? 1 : 0);
    }
    
    NumberExpression operator<(const StringNode& other) const {
        return NumberExpression(toString() < other.toString() ? 1 : 0);
    }
    
    NumberExpression operator>(const StringNode& other) const {
        return NumberExpression(toString() > other.toString() ? 1 : 0);
    }
    
    NumberExpression operator<=(const StringNode& other) const {
        return NumberExpression(toString() <= other.toString() ? 1 : 0);
    }
    
    NumberExpression operator>=(const StringNode& other) const {
        return NumberExpression(toString() >= other.toString() ? 1 : 0);
    }
    

};

// 字符串字面量节点 - 使用StringNode实现，支持数组访问
using StringLiteral = StringNode;

// 字典节点 - 支持键值访问的语法树节点
struct DictNode : public Expression {
    map<string, Expression*> entries;
    
    DictNode() {}
    
    // 设置键值对
    void setEntry(const string& key, Expression* value) {
        entries[key] = value;
    }
    
    // 获取值
    Expression* getEntry(const string& key) const {
        auto it = entries.find(key);
        return it != entries.end() ? it->second : nullptr;
    }
    
    // 检查键是否存在
    bool hasKey(const string& key) const {
        return entries.find(key) != entries.end();
    }
    
    // 删除键值对
    void removeEntry(const string& key) {
        entries.erase(key);
    }
    
    // 获取键值对数量
    size_t getEntryCount() const {
        return entries.size();
    }
    
    // 获取所有键
    vector<string> getKeys() const {
        vector<string> keys;
        for (const auto& pair : entries) {
            keys.push_back(pair.first);
        }
        return keys;
    }
    
    // 获取所有值
    vector<Expression*> getValues() const {
        vector<Expression*> values;
        for (const auto& pair : entries) {
            values.push_back(pair.second);
        }
        return values;
    }
    
    string toString() const override {
        string result = "{";
        bool first = true;
        for (const auto& pair : entries) {
            if (!first) result += ", ";
            result += "\"" + pair.first + "\": " + pair.second->toString();
            first = false;
        }
        result += "}";
        return result;
    }
    
    // 访问操作 - 将key作为字符串去查询
    Expression* access(Expression* key) {
        // 将key转换为字符串
        string keyStr = key->toString();
        auto it = entries.find(keyStr);
        return it != entries.end() ? it->second : nullptr;
    }
};

// 访问表达式
struct AccessExpression : public Expression {
    Expression* target;  // 被访问的表达式
    Expression* key;     // 访问键
    bool isDotNotation;  // 是否为点号访问
    
    AccessExpression(Expression* t, Expression* k, bool dotNotation = false) 
        : target(t), key(k), isDotNotation(dotNotation) {}
    
    string toString() const override {
        if (isDotNotation) {
            return (target ? target->toString() : "") + "." + (key ? key->toString() : "");
        } else {
            return (target ? target->toString() : "") + "[" + (key ? key->toString() : "") + "]";
        }
    }
};

// 函数调用表达式
struct CallExpression : public Expression {
    Expression* callee;             // 被调用的表达式
    vector<Expression*> arguments;  // 参数列表
    
    CallExpression(Expression* func, vector<Expression*> args) : callee(func), arguments(args) {}
    
    string toString() const override {
        string result = (callee ? callee->toString() : "") + "(";
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (i > 0) result += ", ";
            result += arguments[i]->toString();
        }
        result += ")";
        return result;
    }
};

// 语句基类
struct Statement : public AST {
    Type getType() const override { return Type::STATEMENT; }
    
    virtual string toString() const override { return "statement"; }
};

// 表达式语句
struct ExpressionStatement : public Statement {
    Expression* expression;
    
    ExpressionStatement(Expression* expr) : expression(expr) {}
    
    string toString() const override {
        return (expression ? expression->toString() : "") + ";";
    }
};

// 变量声明语句
struct VariableDeclaration : public Statement {
    struct Variable {
        string name;
        Expression* initializer;  // 初始化表达式，可为nullptr
        
        Variable(const string& n, Expression* init = nullptr) 
            : name(n), initializer(init) {}
    };
    
    vector<Variable> variables;
    
    VariableDeclaration() {}
    
    // 添加单个变量
    void addVariable(const string& name, Expression* initializer = nullptr) {
        variables.emplace_back(name, initializer);
    }
    
    // 构造函数，用于单个变量（向后兼容）
    VariableDeclaration(const string& name, Expression* initializer = nullptr) {
        addVariable(name, initializer);
    }
    
    string toString() const override {
        string result = "let ";
        for (size_t i = 0; i < variables.size(); ++i) {
            if (i > 0) result += ", ";
            result += variables[i].name;
            if (variables[i].initializer) {
                result += " = " + variables[i].initializer->toString();
            }
        }
        return result + ";";
    }
};

// 条件语句
struct IfStatement : public Statement {
    Expression* condition;
    Statement* thenBranch;
    Statement* elseBranch;  // 可为空
    
    IfStatement(Expression* cond, Statement* thenStmt, Statement* elseStmt = nullptr) 
        : condition(cond), thenBranch(thenStmt), elseBranch(elseStmt) {}
    
    string toString() const override {
        string result = "if (" + (condition ? condition->toString() : "") + ") " + 
                       (thenBranch ? thenBranch->toString() : "");
        if (elseBranch) {
            result += " else " + elseBranch->toString();
        }
        return result;
    }
};

// 循环语句
struct WhileStatement : public Statement {
    Expression* condition;
    Statement* body;
    
    WhileStatement(Expression* cond, Statement* bodyStmt) 
        : condition(cond), body(bodyStmt) {}
    
    string toString() const override {
        return "while (" + (condition ? condition->toString() : "") + ") " + 
               (body ? body->toString() : "");
    }
};

struct ForStatement : public Statement {
    Expression* initializer;
    Expression* condition;
    Expression* increment;
    Statement* body;
    
    ForStatement(Expression* init, Expression* cond, Expression* inc, Statement* bodyStmt) 
        : initializer(init), condition(cond), increment(inc), body(bodyStmt) {}
    
    string toString() const override {
        string result = "for (" + (initializer ? initializer->toString() : "") + "; " + 
                        (condition ? condition->toString() : "") + "; " +
                        (increment ? increment->toString() : "") + ") " +
                        (body ? body->toString() : "");
        return result;
    }
};

struct BreakStatement : public Statement {
    string toString() const override {
        return "break;";
    }
};

struct ContinueStatement : public Statement {
    string toString() const override {
        return "continue;";
    }
};

struct ReturnStatement : public Statement {
    Expression* returnValue;
    
    ReturnStatement(Expression* retVal = nullptr) 
        : returnValue(retVal) {}
    
    string toString() const override {
        string result = "return";
        if (returnValue) {
            result += " " + returnValue->toString();
        }
        return result + ";";
    }
};

struct ThrowStatement : public Statement {
    Expression* exception;
    
    ThrowStatement(Expression* exc) : exception(exc) {}
    
    string toString() const override {
        return "throw " + (exception ? exception->toString() : "");
    }
};

struct TryStatement : public Statement {
    Statement* tryBlock;
    Statement* catchBlock;
    
    TryStatement(Statement* tryStmt, Statement* catchStmt = nullptr) 
        : tryBlock(tryStmt), catchBlock(catchStmt) {}
    
    string toString() const override {
        string result = "try {\n";
        result += (tryBlock ? tryBlock->toString() : "") + "\n";
        if (catchBlock) {
            result += "} catch (error) {\n";
            result += catchBlock->toString() + "\n";
            result += "} }";
        }
        return result;
    }
};

struct CatchStatement : public Statement {
    string exception;
    Statement* catchBlock;
    
    CatchStatement(const string& exc, Statement* catchStmt) 
        : exception(exc), catchBlock(catchStmt) {}
    

    string toString() const override {
        return "catch (" + exception + ") " + 
               (catchBlock ? catchBlock->toString() : "");
    }
};

struct FinallyStatement : public Statement {
    Statement* finallyBlock;
    
    FinallyStatement(Statement* finallyStmt) 
        : finallyBlock(finallyStmt) {}
    
    string toString() const override {
        return "finally " + (finallyBlock ? finallyBlock->toString() : "");
    }
};

struct CaseStatement : public Statement {
    Expression* condition;
    Statement* body;
    
    CaseStatement(Expression* cond, Statement* bodyStmt) 
        : condition(cond), body(bodyStmt) {}
    
    string toString() const override {
        return "case " + (condition ? condition->toString() : "") + ": " + 
               (body ? body->toString() : "");
    }
};

struct DefaultStatement : public Statement {
    Statement* body;
    
    DefaultStatement(Statement* bodyStmt) 
        : body(bodyStmt) {}
    
    string toString() const override {
        return "default: " + (body ? body->toString() : "");
    }
};

struct SwitchStatement : public Statement {
    Expression* condition;
    vector<CaseStatement*> cases;
    Statement* defaultCase;
    
    SwitchStatement(Expression* cond, vector<CaseStatement*> caseStmts, Statement* defaultStmt = nullptr) 
        : condition(cond), cases(caseStmts), defaultCase(defaultStmt) {}

    string toString() const override {
        string result = "switch (" + (condition ? condition->toString() : "") + ") {\n";
        for (size_t i = 0; i < cases.size(); ++i) {
            result += "  case " + cases[i]->toString() + ":\n";
            result += (cases[i]->body ? cases[i]->body->toString() : "") + "\n";
            result += "  break;\n";
        }
        if (defaultCase) {
            result += "  default:\n";
            result += defaultCase->toString() + "\n";
            result += "  break;\n";
        }
        result += "}";
        return result;
    }
};

// 语句块
struct BlockStatement : public Statement {
    vector<Statement*> statements;
    
    // 添加语句
    void addStatement(Statement* stmt) {
        statements.push_back(stmt);
    }
    
    string toString() const override {
        string result = "{\n";
        for (auto stmt : statements) {
            result += "  " + stmt->toString() + "\n";
        }
        result += "}";
        return result;
    }
};

struct FunctionPrototype : public Statement {
    string name;
    vector<string> parameters;
    
    FunctionPrototype(const string& n, vector<string> params) 
        : name(n), parameters(params) {}
    
    string toString() const override {
        string result = "function " + name;
        result += "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) result += ", ";
            result += parameters[i];
        }
        result += ")";
        return result;
    }
};

// 函数定义语句
struct FunctionDefinition : public Statement {
    FunctionPrototype* prototype;
    BlockStatement* body;
    
    FunctionDefinition(FunctionPrototype* proto, BlockStatement* body) 
        : prototype(proto), body(body) {}
    
    string toString() const override {
        return prototype->toString() + " {\n" + (body ? body->toString() : "") + "\n}";
    }
};

// 程序根节点
struct Program : public AST {
    vector<Statement*> statements;
    
    // 添加语句
    void addStatement(Statement* stmt) {
        statements.push_back(stmt);
    }
    
    Type getType() const override { return Type::STATEMENT; }
    
    string toString() const override {
        string result;
        for (auto stmt : statements) {
            result += stmt->toString() + "\n";
        }
        return result;
    }
};

// ==================== 类型别名（向后兼容） ====================
// 为了保持向后兼容性，提供类型别名
using Id = IdentifierExpression;
using Constant = NumberExpression;
using Arith = ArithmeticExpression;
using Stmt = Statement;
using Stmts = BlockStatement;
using Bind = VariableDeclaration;
using If = IfStatement;
using While = WhileStatement;
using Call = CallExpression;
using Access = AccessExpression;
using Nodes = BlockStatement;

// ==================== 内置函数表达式 ====================
// 内置函数表达式
struct BuiltinFunctionExpression : public Expression {
    string functionName;
    
    BuiltinFunctionExpression(const string& name) : functionName(name) {}
    
    Type getType() const override { return Type::FUNCTION; }
    
    string toString() const override {
        return "builtin_function(" + functionName + ")";
    }
    
    string getName() const { return functionName; }
};

// ==================== 结构体和类定义 ====================

// 结构体成员定义
struct StructMember {
    string name;
    string type;
    Expression* defaultValue;
    
    StructMember(const string& n, const string& t, Expression* def = nullptr) 
        : name(n), type(t), defaultValue(def) {}
};

// 结构体定义
struct StructDefinition : public Statement {
    string name;
    vector<StructMember> members;
    
    StructDefinition(const string& n, vector<StructMember> mems) 
        : name(n), members(mems) {}
    
    string toString() const override {
        string result = "struct " + name + " {\n";
        for (const auto& member : members) {
            result += "  " + member.type + " " + member.name;
            if (member.defaultValue) {
                result += " = " + member.defaultValue->toString();
            }
            result += ";\n";
        }
        result += "}";
        return result;
    }
};

// 类成员定义
struct ClassMember {
    string name;
    string type;
    string visibility; // "public", "private", "protected"
    Expression* defaultValue;
    
    ClassMember(const string& n, const string& t, const string& vis = "public", Expression* def = nullptr) 
        : name(n), type(t), visibility(vis), defaultValue(def) {}
};

// 类方法定义
struct ClassMethod {
    string name;
    vector<string> parameters;
    string returnType;
    string visibility; // "public", "private", "protected"
    BlockStatement* body;
    
    ClassMethod(const string& n, vector<string> params, const string& ret = "void", 
                const string& vis = "public", BlockStatement* b = nullptr) 
        : name(n), parameters(params), returnType(ret), visibility(vis), body(b) {}
    
    string toString() const {
        string result = visibility + " " + returnType + " " + name + "(";
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) result += ", ";
            result += parameters[i];
        }
        result += ")";
        if (body) {
            result += " {\n" + body->toString() + "\n}";
        }
        return result;
    }
};

// 类定义
struct ClassDefinition : public Statement {
    string name;
    vector<ClassMember> members;
    vector<ClassMethod> methods;
    string baseClass; // 继承的基类
    
    ClassDefinition(const string& n, vector<ClassMember> mems = {}, 
                   vector<ClassMethod> meths = {}, const string& base = "") 
        : name(n), members(mems), methods(meths), baseClass(base) {}
    
    string toString() const override {
        string result = "class " + name;
        if (!baseClass.empty()) {
            result += " : " + baseClass;
        }
        result += " {\n";
        
        // 成员变量
        for (const auto& member : members) {
            result += "  " + member.visibility + " " + member.type + " " + member.name;
            if (member.defaultValue) {
                result += " = " + member.defaultValue->toString();
            }
            result += ";\n";
        }
        
        // 方法
        for (const auto& method : methods) {
            result += "  " + method.toString() + "\n";
        }
        
        result += "}";
        return result;
    }
};

// 结构体实例化表达式
struct StructInstantiationExpression : public Expression {
    string structName;
    map<string, Expression*> fieldValues;
    
    StructInstantiationExpression(const string& name, map<string, Expression*> values) 
        : structName(name), fieldValues(values) {}
    
    string toString() const override {
        string result = structName + " {";
        for (const auto& pair : fieldValues) {
            if (result.back() != '{') result += ", ";
            result += pair.first + ": " + pair.second->toString();
        }
        result += "}";
        return result;
    }
};

// 类实例化表达式
struct ClassInstantiationExpression : public Expression {
    string className;
    vector<Expression*> arguments;
    
    ClassInstantiationExpression(const string& name, vector<Expression*> args) 
        : className(name), arguments(args) {}
    
    string toString() const override {
        string result = className + "(";
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (i > 0) result += ", ";
            result += arguments[i]->toString();
        }
        result += ")";
        return result;
    }
};

// 成员访问表达式
struct MemberAccessExpression : public Expression {
    Expression* object;
    string memberName;
    
    MemberAccessExpression(Expression* obj, const string& member) 
        : object(obj), memberName(member) {}
    
    string toString() const override {
        return object->toString() + "." + memberName;
    }
};

// 方法调用表达式
struct MethodCallExpression : public Expression {
    Expression* object;
    string methodName;
    vector<Expression*> arguments;
    
    MethodCallExpression(Expression* obj, const string& method, vector<Expression*> args) 
        : object(obj), methodName(method), arguments(args) {}
    
    string toString() const override {
        string result = object->toString() + "." + methodName + "(";
        for (size_t i = 0; i < arguments.size(); ++i) {
            if (i > 0) result += ", ";
            result += arguments[i]->toString();
        }
        result += ")";
        return result;
    }
};

#endif // INTER_H