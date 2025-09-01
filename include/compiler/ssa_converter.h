#ifndef SSA_CONVERTER_H
#define SSA_CONVERTER_H

#include "parser/ast_visitor.h"
#include "parser/statement.h"
#include "parser/expression.h"
#include <vector>
#include <string>
#include <map>
#include <memory>

using namespace std;

// ==================== 虚拟机指令类型 ====================
enum class VMInstructionType {
    // 栈操作
    PUSH, POP, DUP, SWAP,
    
    // 算术运算
    ADD, SUB, MUL, DIV, MOD,
    
    // 位运算
    AND, OR, XOR, NOT, SHL, SHR,
    
    // 比较运算
    EQ, NE, LT, LE, GT, GE,
    
    // 跳转
    JMP, JZ, JNZ, JGT, JLT,
    
    // 函数调用
    CALL, RET,
    
    // 变量操作
    LOAD, STORE, LOAD_GLOBAL, STORE_GLOBAL,
    
    // 数组操作
    ARRAY_LOAD, ARRAY_STORE,
    
    // 其他
    NOP, HALT, PRINT, PRINTLN
};

// ==================== 虚拟机指令 ====================
struct VMInstruction {
    VMInstructionType type;
    int operand;  // 立即数或地址
    string label; // 标签（用于跳转）
    
    VMInstruction(VMInstructionType t, int op = 0, const string& lbl = "")
        : type(t), operand(op), label(lbl) {}
    
    string toString() const;
};

// ==================== 虚拟机函数 ====================
struct VMFunction {
    string name;
    vector<VMInstruction> instructions;
    map<string, int> labels;  // 标签到地址的映射
    int localVarCount;
    int paramCount;
    
    VMFunction(const string& n) : name(n), localVarCount(0), paramCount(0) {}
    
    void addInstruction(const VMInstruction& inst);
    void addLabel(const string& label);
    int getLabelAddress(const string& label) const;
    
    string toString() const;
};

// ==================== 虚拟机程序 ====================
struct VMProgram {
    vector<VMFunction> functions;
    map<string, VMFunction*> functionMap;
    vector<string> globals;
    
    VMFunction* addFunction(const string& name);
    VMFunction* getFunction(const string& name);
    void addGlobal(const string& name);
    
    string toString() const;
    vector<uint8_t> generateBinary() const;
};

// ==================== SSA转换器 ====================
class SSAConverter : public ASTVisitor<void> {
private:
    VMProgram* program;
    VMFunction* currentFunction;
    
    // SSA生成相关
    int tempCounter;         // 临时变量计数器
    
    // 变量管理
    map<string, int> localVars;  // 局部变量到栈偏移的映射
    map<string, int> globals;    // 全局变量到地址的映射
    int nextLocalOffset;
    int nextGlobalAddress;
    
    // 标签管理
    int labelCounter;
    string createLabel(const string& base = "L");
    
    // SSA生成辅助方法
    string createTemp(const string& base = "t");
    void generateSSAInstruction(const string& operation, const vector<string>& operands);
    
    // 表达式转换
    void convertExpression(Expression* expr);
    void convertBinaryExpression(BinaryExpression* expr);
    void convertUnaryExpression(UnaryExpression* expr);
    void convertCallExpression(CallExpression* expr);
    void convertAssignExpression(AssignExpression* expr);
    void convertVariableExpression(VariableExpression* expr);
    void convertConstantExpression(ConstantExpression* expr);
    
    // 语句转换
    void convertStatement(Statement* stmt);
    void convertIfStatement(IfStatement* stmt);
    void convertWhileStatement(WhileStatement* stmt);
    void convertForStatement(ForStatement* stmt);
    void convertReturnStatement(ReturnStatement* stmt);
    void convertVariableDefinition(VariableDefinition* stmt);
    void convertExpressionStatement(ExpressionStatement* stmt);
    void convertBlockStatement(BlockStatement* stmt);
    
    // 辅助方法
    void addInstruction(VMInstructionType type, int operand = 0, const string& label = "");
    int getVariableOffset(const string& name);
    void declareVariable(const string& name);

public:
    SSAConverter();
    ~SSAConverter();
    
    // 主要转换接口
    VMProgram* convert(Program* ast);
    
    // SSA生成接口
    string generateSSA(Program* ast);
    
    // AST访问者模式实现
    void visit(Expression* expr) override;
    void visit(ConstantExpression* expr) override;
    void visit(VariableExpression* expr) override;
    void visit(UnaryExpression* expr) override;
    void visit(BinaryExpression* expr) override;
    void visit(AssignExpression* expr) override;
    void visit(AccessExpression* expr) override;
    void visit(CallExpression* expr) override;
    void visit(CastExpression* expr) override;
    
    void visit(Program* program) override;
    void visit(Statement* stmt) override;
    void visit(ImportStatement* stmt) override;
    void visit(ExpressionStatement* stmt) override;
    void visit(VariableDefinition* stmt) override;
    void visit(IfStatement* stmt) override;
    void visit(WhileStatement* stmt) override;
    void visit(ForStatement* stmt) override;
    void visit(DoWhileStatement* stmt) override;
    void visit(BlockStatement* stmt) override;
    void visit(StructDefinition* stmt) override;
    void visit(ClassDefinition* stmt) override;
    void visit(BreakStatement* stmt) override;
    void visit(ContinueStatement* stmt) override;
    void visit(ReturnStatement* stmt) override;
    void visit(TryStatement* stmt) override;
    void visit(SwitchStatement* stmt) override;
    void visit(FunctionPrototype* stmt) override;
    void visit(Identifier* id) override;
    void visit(Variable* var) override;
};

// ==================== 编译器主类 ====================
class Compiler {
private:
    SSAConverter converter;
    
public:
    Compiler() = default;
    ~Compiler() = default;
    
    // 编译AST到虚拟机程序
    VMProgram* compile(Program* ast);
    
    // 编译并生成二进制文件
    bool compileToFile(Program* ast, const string& outputFile);
    
    // 生成汇编代码（用于调试）
    string generateAssembly(Program* ast);
};

// ==================== 类型别名和默认类型 ====================

// 默认的编译器类型
using DefaultCompiler = Compiler;

#endif // SSA_CONVERTER_H
