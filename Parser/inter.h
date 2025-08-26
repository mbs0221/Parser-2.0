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
#include <typeinfo>
#include "ast_visitor.h"

// 前向声明
struct ReturnResult;
struct IntExpression;
struct DoubleExpression;
struct StringLiteral;
struct CharExpression;
struct BoolExpression;

using namespace std;

// ==================== AST基类 ====================
// 抽象语法树节点基类
struct AST {
    virtual ~AST() = default;
    
    // 获取节点位置信息（用于错误报告）
    virtual string getLocation() const { return "unknown"; }

    // 接受访问者 - 标准访问者模式，void返回类型
    virtual void accept(ASTVisitor* visitor) = 0;
};

// ==================== 语法单元 ====================
// 前向声明
class ASTVisitor;

// 表达式基类
struct Expression : public AST {
    // 访问者模式：接受AST访问者
    virtual void accept(ASTVisitor* visitor) override = 0;
};

// 叶子节点基类 - 使用泛型类型转换
struct LeafExpression : public Expression {
    // 泛型类型转换函数
    template<typename TargetType>
    TargetType* convert() {
        return nullptr; // 默认实现返回nullptr
    }

    virtual string toString() const {
        return "LeafExpression";
    }
};

// 整数表达式节点
struct IntExpression : public LeafExpression {
    int value;
    
    IntExpression(int val) : value(val) {}
    
    // 获取数值
    int getIntValue() const {
		return value;
	}
    
    double getDoubleValue() const {
        return (double)value;
    }
    
    string toString() const {
        return to_string(value);
    }
    
    void accept(ASTVisitor* visitor) override;
    
    // 泛型类型转换方法 - 声明
    template<typename TargetType>
    TargetType* convert();
    
    // 运算符重载 - 算术运算
    IntExpression operator+(const IntExpression& other) const {
        return IntExpression(value + other.value);
    }
    
    IntExpression operator-(const IntExpression& other) const {
        return IntExpression(value - other.value);
    }
    
    IntExpression operator*(const IntExpression& other) const {
        return IntExpression(value * other.value);
    }
    
    IntExpression operator/(const IntExpression& other) const {
        if (other.value == 0) {
            throw runtime_error("Division by zero");
        }
        return IntExpression(value / other.value);
    }
    
    IntExpression operator%(const IntExpression& other) const {
        if (other.value == 0) {
            throw runtime_error("Modulo by zero");
        }
        return IntExpression(value % other.value);
    }
    
    // 运算符重载 - 比较运算
    IntExpression operator==(const IntExpression& other) const {
        return IntExpression(value == other.value ? 1 : 0);
    }
    
    IntExpression operator!=(const IntExpression& other) const {
        return IntExpression(value != other.value ? 1 : 0);
    }
    
    IntExpression operator<(const IntExpression& other) const {
        return IntExpression(value < other.value ? 1 : 0);
    }
    
    IntExpression operator>(const IntExpression& other) const {
        return IntExpression(value > other.value ? 1 : 0);
    }
    
    IntExpression operator<=(const IntExpression& other) const {
        return IntExpression(value <= other.value ? 1 : 0);
    }
    
    IntExpression operator>=(const IntExpression& other) const {
        return IntExpression(value >= other.value ? 1 : 0);
    }
    
    // 运算符重载 - 逻辑运算
    IntExpression operator&&(const IntExpression& other) const {
        return IntExpression((value != 0 && other.value != 0) ? 1 : 0);
    }
    
    IntExpression operator||(const IntExpression& other) const {
        return IntExpression((value != 0 || other.value != 0) ? 1 : 0);
    }
    
    // 一元逻辑非操作符
    IntExpression operator!() const {
        return IntExpression(value == 0 ? 1 : 0);
    }
    
    // 一元负号操作符
    IntExpression operator-() const {
        return IntExpression(-value);
    }
};

// 浮点数表达式节点
struct DoubleExpression : public LeafExpression {
    double value;
    
    DoubleExpression(double val) : value(val) {}
    DoubleExpression(int val) : value((double)val) {}  // 从整数构造
    
    // 获取数值
    int getIntValue() const {
        return (int)value;
    }
    
    double getDoubleValue() const {
        return value;
    }
    
    string toString() const {
        return to_string(value);
    }
    
    void accept(ASTVisitor* visitor) override;
    
    // 泛型类型转换方法 - 声明
    template<typename TargetType>
    TargetType* convert();
    
    // 运算符重载 - 算术运算
    DoubleExpression operator+(const DoubleExpression& other) const {
        return DoubleExpression(value + other.value);
    }
    
    DoubleExpression operator-(const DoubleExpression& other) const {
        return DoubleExpression(value - other.value);
    }
    
    DoubleExpression operator*(const DoubleExpression& other) const {
        return DoubleExpression(value * other.value);
    }
    
    DoubleExpression operator/(const DoubleExpression& other) const {
        if (other.value == 0) {
            throw runtime_error("Division by zero");
        }
        return DoubleExpression(value / other.value);
    }
    
    // 浮点数不支持模运算，返回0
    DoubleExpression operator%(const DoubleExpression& other) const {
        return DoubleExpression(0.0);
    }
    
    // 运算符重载 - 比较运算
    IntExpression operator==(const DoubleExpression& other) const {
        return IntExpression(value == other.value ? 1 : 0);
    }
    
    IntExpression operator!=(const DoubleExpression& other) const {
        return IntExpression(value != other.value ? 1 : 0);
    }
    
    IntExpression operator<(const DoubleExpression& other) const {
        return IntExpression(value < other.value ? 1 : 0);
    }
    
    IntExpression operator>(const DoubleExpression& other) const {
        return IntExpression(value > other.value ? 1 : 0);
    }
    
    IntExpression operator<=(const DoubleExpression& other) const {
        return IntExpression(value <= other.value ? 1 : 0);
    }
    
    IntExpression operator>=(const DoubleExpression& other) const {
        return IntExpression(value >= other.value ? 1 : 0);
    }
    
    // 运算符重载 - 逻辑运算
    IntExpression operator&&(const DoubleExpression& other) const {
        return IntExpression((value != 0 && other.value != 0) ? 1 : 0);
    }
    
    IntExpression operator||(const DoubleExpression& other) const {
        return IntExpression((value != 0 || other.value != 0) ? 1 : 0);
    }
    
    // 一元逻辑非操作符
    IntExpression operator!() const {
        return IntExpression(value == 0 ? 1 : 0);
    }
    
    // 一元负号操作符
    DoubleExpression operator-() const {
        return DoubleExpression(-value);
    }
};

// 布尔表达式
struct BoolExpression : public LeafExpression {
    bool value;
    
    BoolExpression(bool val) : value(val) {}
    
    bool getBoolValue() const {
        return value;
    }
    
    string getLocation() const override {
        return value ? "true" : "false";
    }
    
    string toString() const {
        return value ? "true" : "false";
    }
    
    void accept(ASTVisitor* visitor) override;
    
    // 泛型类型转换方法 - 声明
    template<typename TargetType>
    TargetType* convert();
    
    // 运算符重载 - 逻辑运算
    BoolExpression operator&&(const BoolExpression& other) const {
        return BoolExpression(value && other.value);
    }
    
    BoolExpression operator||(const BoolExpression& other) const {
        return BoolExpression(value || other.value);
    }
    
    BoolExpression operator!() const {
        return BoolExpression(!value);
    }
    
    // 运算符重载 - 比较运算
    BoolExpression operator==(const BoolExpression& other) const {
        return BoolExpression(value == other.value);
    }
    
    BoolExpression operator!=(const BoolExpression& other) const {
        return BoolExpression(value != other.value);
    }
};

// 标识符表达式
struct IdentifierExpression : public Expression {
    string name;
    
    IdentifierExpression(const string& n) : name(n) {}
    
    string getName() const {
        return name;
    }
    
    void accept(ASTVisitor* visitor) override;
};

// 一元表达式
struct UnaryExpression : public Expression {
    Token* operator_;  // 操作符Token
    Expression* operand;
    
    UnaryExpression(Token* op, Expression* operand) 
        : operator_(op), operand(operand) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 二元运算表达式 - 统一处理算术、比较、逻辑运算
struct BinaryExpression : public Expression {
    Expression* left;
    Token* operator_;  // 操作符Token
    Expression* right;
    
    BinaryExpression(Expression* l, Token* op, Expression* r) 
        : left(l), operator_(op), right(r) {}
    
    void accept(ASTVisitor* visitor) override;
    
    // 获取操作符类型
    enum class OperatorType {
        ARITHMETIC,  // 算术运算: +, -, *, /, %
        COMPARISON,  // 比较运算: ==, !=, <, >, <=, >=
        LOGICAL      // 逻辑运算: &&, ||
    };
    
    OperatorType getOperatorType() const {
        if (!operator_) return OperatorType::ARITHMETIC;
        
        switch (operator_->Tag) {
            case '+': case '-': case '*': case '/': case '%':
                return OperatorType::ARITHMETIC;
            case EQ: case NE: case '<': case '>': case BE: case GE:
                return OperatorType::COMPARISON;
            case AND: case OR:
                return OperatorType::LOGICAL;
            default:
                return OperatorType::ARITHMETIC;
        }
    }
};

// 为了向后兼容，保留ArithmeticExpression别名
using ArithmeticExpression = BinaryExpression;

// 赋值表达式
struct AssignmentExpression : public Expression {
    string variableName;  // 变量名
    Expression* value;    // 要赋的值
    
    AssignmentExpression(const string& varName, Expression* val) 
        : variableName(varName), value(val) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 类型转换表达式
// 类型转换表达式 - 使用模板实现
template<typename TargetType>
struct CastExpression : public Expression {
    Expression* operand;     // 被转换的表达式
    
    CastExpression(Expression* expr) 
        : operand(expr) {}
    
    void accept(ASTVisitor* visitor) override;
    
    // 获取目标类型信息
    const std::type_info& getTargetTypeInfo() const {
        return typeid(TargetType);
    }
    
    // 获取目标类型名称
    string getTargetTypeName() const {
        return typeid(TargetType).name();
    }
};

// 常用的类型转换表达式别名
using IntCastExpression = CastExpression<IntExpression>;
using DoubleCastExpression = CastExpression<DoubleExpression>;
using StringCastExpression = CastExpression<StringLiteral>;
using CharCastExpression = CastExpression<CharExpression>;

// 字符表达式
struct CharExpression : public LeafExpression {
    char value;
    
    CharExpression(char val) : value(val) {}
    
    void accept(ASTVisitor* visitor) override;
    
    // 泛型类型转换方法 - 声明
    template<typename TargetType>
    TargetType* convert();

    string toString() const {
        return string(1, value);
    }
};

// 支持访问操作的表达式基类
struct AccessibleExpression : public Expression {
    // 虚函数：执行访问操作
    virtual Expression* access(Expression* key) = 0;
};

// 数组节点 - 支持访问操作
struct ArrayNode : public AccessibleExpression {
    vector<Expression*> elements;
    
    ArrayNode() {}
    
    // 添加元素
    void addElement(Expression* element) {
        elements.push_back(element);
    }

    // 获取元素
    Expression* getElement(size_t index) const {
        return elements[index];
    }

    // 设置元素
    void setElement(size_t index, Expression* element) {
        elements[index] = element;
    }
    
    // 获取元素数量
    size_t getElementCount() const {
        return elements.size();
    }
    
    // 获取数组大小
    size_t size() const {
        return elements.size();
    }
    
    void accept(ASTVisitor* visitor) override;
    
    // 访问操作 - 将key作为数组下标
    Expression* access(Expression* key) override {
        // 将key转换为数字索引
        IntExpression* intKey = dynamic_cast<IntExpression*>(key);
        if (intKey) {
            int index = intKey->getIntValue();
            return index >= 0 && index < (int)elements.size() ? elements[index] : nullptr;
        }
        return nullptr;
    }
};

// 字符串节点 - 继承自数组，元素是字符
struct StringLiteral : public ArrayNode {
    StringLiteral(const string& str) {
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

    string toString() const {
        string result;
        for (const auto& element : elements) {
            CharExpression* charExpr = dynamic_cast<CharExpression*>(element);
            if (charExpr) {
                result += charExpr->toString();
            }
        }
        return result;
    }
    
    // 运算符重载 - 字符串拼接
    StringLiteral operator+(const StringLiteral& other) const {
        StringLiteral result = *this;
        result.elements.insert(result.elements.end(), other.elements.begin(), other.elements.end());
        return result;
    }

    // 运算符重载 - 字符串比较
    BoolExpression operator==(const StringLiteral& other) const {
        return BoolExpression(toString() == other.toString());
    }

    BoolExpression operator!=(const StringLiteral& other) const {
        return BoolExpression(toString() != other.toString());
    }

    BoolExpression operator<(const StringLiteral& other) const {
        return BoolExpression(toString() < other.toString());
    }

    BoolExpression operator>(const StringLiteral& other) const {
        return BoolExpression(toString() > other.toString());
    }

    BoolExpression operator<=(const StringLiteral& other) const {
        return BoolExpression(toString() <= other.toString());
    }

    BoolExpression operator>=(const StringLiteral& other) const {
        return BoolExpression(toString() >= other.toString());
    }
};

// 字典节点 - 支持访问操作
struct DictNode : public AccessibleExpression {
    map<string, Expression*> entries;
    
    DictNode() {}
    
    // 设置条目
    void setEntry(const string& key, Expression* value) {
        entries[key] = value;
    }
    
    // 获取条目
    Expression* getEntry(const string& key) const {
        auto it = entries.find(key);
        return it != entries.end() ? it->second : nullptr;
    }
    
    // 获取条目数量
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
    void accept(ASTVisitor* visitor) override;
    
    // 访问操作 - 将key作为字符串去查询
    Expression* access(Expression* key) override {
        // 将key转换为字符串
        string keyStr = key->getLocation();
        auto it = entries.find(keyStr);
        return it != entries.end() ? it->second : nullptr;
    }
};

// 访问表达式 - 支持访问操作的表达式
struct AccessExpression : public Expression {
    Expression* target;  // 被访问的表达式（在运行时检查是否为AccessibleExpression）
    Expression* key;     // 访问键
    bool isDotNotation;  // 是否为点号访问
    
    AccessExpression(Expression* t, Expression* k, bool dotNotation = false) 
        : target(t), key(k), isDotNotation(dotNotation) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 函数调用表达式
struct CallExpression : public Expression {
    Expression* callee;             // 被调用的表达式
    vector<Expression*> arguments;  // 参数列表
    
    CallExpression(Expression* func, vector<Expression*> args) : callee(func), arguments(args) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 前向声明
class StatementVisitor;

// 语句基类
struct Statement : public AST {
    // 访问者模式：接受访问者
    virtual void accept(ASTVisitor* visitor) override = 0;
};

// 导入语句
struct ImportStatement : public Statement {
    string moduleName;  // 要导入的模块名
    
    ImportStatement(const string& module) : moduleName(module) {}void accept(ASTVisitor* visitor) override;
};

// 表达式语句
struct ExpressionStatement : public Statement {
    Expression* expression;
    
    ExpressionStatement(Expression* expr) : expression(expr) {}void accept(ASTVisitor* visitor) override;
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

    void accept(ASTVisitor* visitor) override;
};

// 条件语句
struct IfStatement : public Statement {
    Expression* condition;
    Statement* thenBranch;
    Statement* elseBranch;  // 可为空
    
    IfStatement(Expression* cond, Statement* thenStmt, Statement* elseStmt = nullptr) 
        : condition(cond), thenBranch(thenStmt), elseBranch(elseStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 循环语句
struct WhileStatement : public Statement {
    Expression* condition;
    Statement* body;
    
    WhileStatement(Expression* cond, Statement* bodyStmt) 
        : condition(cond), body(bodyStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct ForStatement : public Statement {
    Expression* initializer;
    Expression* condition;
    Expression* increment;
    Statement* body;
    
    ForStatement(Expression* init, Expression* cond, Expression* inc, Statement* bodyStmt) 
        : initializer(init), condition(cond), increment(inc), body(bodyStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct DoWhileStatement : public Statement {
    Statement* body;
    Expression* condition;
    
    DoWhileStatement(Statement* bodyStmt, Expression* cond) 
        : body(bodyStmt), condition(cond) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct BreakStatement : public Statement {
    void accept(ASTVisitor* visitor) override;
};

struct ContinueStatement : public Statement {
    void accept(ASTVisitor* visitor) override;
};

struct ReturnStatement : public Statement {
    Expression* returnValue;
    
    ReturnStatement(Expression* retVal = nullptr) 
        : returnValue(retVal) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct ThrowStatement : public Statement {
    Expression* exception;
    
    ThrowStatement(Expression* exc) : exception(exc) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct TryStatement : public Statement {
    Statement* tryBlock;
    Statement* catchBlock;
    
    TryStatement(Statement* tryStmt, Statement* catchStmt = nullptr) 
        : tryBlock(tryStmt), catchBlock(catchStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct CatchStatement : public Statement {
    string exception;
    Statement* catchBlock;
    
    CatchStatement(const string& exc, Statement* catchStmt) 
        : exception(exc), catchBlock(catchStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct FinallyStatement : public Statement {
    Statement* finallyBlock;
    
    FinallyStatement(Statement* finallyStmt) 
        : finallyBlock(finallyStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct CaseStatement : public Statement {
    Expression* condition;
    Statement* body;
    
    CaseStatement(Expression* cond, Statement* bodyStmt) 
        : condition(cond), body(bodyStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct DefaultStatement : public Statement {
    Statement* body;
    
    DefaultStatement(Statement* bodyStmt) 
        : body(bodyStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

struct SwitchStatement : public Statement {
    Expression* condition;
    vector<CaseStatement*> cases;
    Statement* defaultCase;
    
    SwitchStatement(Expression* cond, vector<CaseStatement*> caseStmts, Statement* defaultStmt = nullptr) 
        : condition(cond), cases(caseStmts), defaultCase(defaultStmt) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 语句块
struct BlockStatement : public Statement {
    vector<Statement*> statements;
    
    // 添加语句
    void addStatement(Statement* stmt) {
        statements.push_back(stmt);
    }
    
    void accept(ASTVisitor* visitor) override;
};

struct FunctionPrototype : public Statement {
    string name;
    vector<string> parameters;
    
    FunctionPrototype(const string& n, vector<string> params) 
        : name(n), parameters(params) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 函数定义语句
struct FunctionDefinition : public Statement {
    FunctionPrototype* prototype;
    BlockStatement* body;
    
    FunctionDefinition(FunctionPrototype* proto, BlockStatement* body) 
        : prototype(proto), body(body) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 程序根节点
struct Program : public AST {
    vector<Statement*> statements;

    void accept(ASTVisitor* visitor) override;
    
    // 添加语句
    void addStatement(Statement* stmt) {
        statements.push_back(stmt);
    }
    
    
};

// ==================== 类型别名（向后兼容） ====================
// 为了保持向后兼容性，提供类型别名
using Id = IdentifierExpression;
using Constant = IntExpression;
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
    
    
    void accept(ASTVisitor* visitor) override;
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
    
    void accept(ASTVisitor* visitor) override;
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
    
    // ClassMethod不需要accept方法，因为它不是AST节点
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
    
    void accept(ASTVisitor* visitor) override;
};

// 结构体实例化表达式
struct StructInstantiationExpression : public Expression {
    IdentifierExpression* structName;
    map<string, Expression*> fieldValues;
    
    StructInstantiationExpression(IdentifierExpression* name, map<string, Expression*> values) 
        : structName(name), fieldValues(values) {}
    
    void accept(ASTVisitor* visitor) override;
};



// 类实例化表达式
struct ClassInstantiationExpression : public Expression {
    IdentifierExpression* className;
    vector<Expression*> arguments;
    
    ClassInstantiationExpression(IdentifierExpression* name, vector<Expression*> args) 
        : className(name), arguments(args) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 成员访问表达式
struct MemberAccessExpression : public Expression {
    Expression* object;
    string memberName;
    
    MemberAccessExpression(Expression* obj, const string& member) 
        : object(obj), memberName(member) {}
    
    void accept(ASTVisitor* visitor) override;
};

// 方法调用表达式
struct MethodCallExpression : public Expression {
    Expression* object;
    string methodName;
    vector<Expression*> arguments;
    
    MethodCallExpression(Expression* obj, const string& method, vector<Expression*> args) 
        : object(obj), methodName(method), arguments(args) {}
    
    void accept(ASTVisitor* visitor) override;
};

// ==================== 模板特化实现 ====================
// 模板特化实现 - 使用inline避免重复定义
// IntExpression的模板特化
template<>
inline IntExpression* IntExpression::convert<IntExpression>() {
    return new IntExpression(value);
}

template<>
inline DoubleExpression* IntExpression::convert<DoubleExpression>() {
    return new DoubleExpression((double)value);
}

template<>
inline StringLiteral* IntExpression::convert<StringLiteral>() {
    return new StringLiteral(std::to_string(value));
}

template<>
inline CharExpression* IntExpression::convert<CharExpression>() {
    return new CharExpression((char)value);
}

template<>
inline BoolExpression* IntExpression::convert<BoolExpression>() {
    return new BoolExpression(value != 0);
}

// DoubleExpression的模板特化
template<>
inline IntExpression* DoubleExpression::convert<IntExpression>() {
    return new IntExpression((int)value);
}

template<>
inline DoubleExpression* DoubleExpression::convert<DoubleExpression>() {
    return new DoubleExpression(value);
}

template<>
inline StringLiteral* DoubleExpression::convert<StringLiteral>() {
    return new StringLiteral(std::to_string(value));
}

template<>
inline CharExpression* DoubleExpression::convert<CharExpression>() {
    return new CharExpression((char)value);
}

template<>
inline BoolExpression* DoubleExpression::convert<BoolExpression>() {
    return new BoolExpression(value != 0.0);
}

// BoolExpression的模板特化
template<>
inline IntExpression* BoolExpression::convert<IntExpression>() {
    return new IntExpression(value ? 1 : 0);
}

template<>
inline DoubleExpression* BoolExpression::convert<DoubleExpression>() {
    return new DoubleExpression(value ? 1.0 : 0.0);
}

template<>
inline StringLiteral* BoolExpression::convert<StringLiteral>() {
    return new StringLiteral(value ? "true" : "false");
}

template<>
inline CharExpression* BoolExpression::convert<CharExpression>() {
    return new CharExpression(value ? '1' : '0');
}

template<>
inline BoolExpression* BoolExpression::convert<BoolExpression>() {
    return new BoolExpression(value);
}

// CharExpression的模板特化
template<>
inline IntExpression* CharExpression::convert<IntExpression>() {
    return new IntExpression((int)value);
}

template<>
inline DoubleExpression* CharExpression::convert<DoubleExpression>() {
    return new DoubleExpression((double)value);
}

template<>
inline StringLiteral* CharExpression::convert<StringLiteral>() {
    return new StringLiteral(std::string(1, value));
}

template<>
inline CharExpression* CharExpression::convert<CharExpression>() {
    return new CharExpression(value);
}

template<>
inline BoolExpression* CharExpression::convert<BoolExpression>() {
    return new BoolExpression(value != '\0');
}

#endif // INTER_H