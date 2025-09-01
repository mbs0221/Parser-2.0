#include "compiler/ssa_converter.h"
#include "parser/statement.h"
#include "parser/expression.h"
#include "parser/definition.h"
#include "lexer/token.h"
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

// ==================== VMInstruction实现 ====================
string VMInstruction::toString() const {
    stringstream ss;
    
    switch (type) {
        case VMInstructionType::PUSH:
            ss << "PUSH " << operand;
            break;
        case VMInstructionType::POP:
            ss << "POP";
            break;
        case VMInstructionType::DUP:
            ss << "DUP";
            break;
        case VMInstructionType::SWAP:
            ss << "SWAP";
            break;
        case VMInstructionType::ADD:
            ss << "ADD";
            break;
        case VMInstructionType::SUB:
            ss << "SUB";
            break;
        case VMInstructionType::MUL:
            ss << "MUL";
            break;
        case VMInstructionType::DIV:
            ss << "DIV";
            break;
        case VMInstructionType::MOD:
            ss << "MOD";
            break;
        case VMInstructionType::AND:
            ss << "AND";
            break;
        case VMInstructionType::OR:
            ss << "OR";
            break;
        case VMInstructionType::XOR:
            ss << "XOR";
            break;
        case VMInstructionType::NOT:
            ss << "NOT";
            break;
        case VMInstructionType::SHL:
            ss << "SHL";
            break;
        case VMInstructionType::SHR:
            ss << "SHR";
            break;
        case VMInstructionType::EQ:
            ss << "EQ";
            break;
        case VMInstructionType::NE:
            ss << "NE";
            break;
        case VMInstructionType::LT:
            ss << "LT";
            break;
        case VMInstructionType::LE:
            ss << "LE";
            break;
        case VMInstructionType::GT:
            ss << "GT";
            break;
        case VMInstructionType::GE:
            ss << "GE";
            break;
        case VMInstructionType::JMP:
            ss << "JMP " << label;
            break;
        case VMInstructionType::JZ:
            ss << "JZ " << label;
            break;
        case VMInstructionType::JNZ:
            ss << "JNZ " << label;
            break;
        case VMInstructionType::JGT:
            ss << "JGT " << label;
            break;
        case VMInstructionType::JLT:
            ss << "JLT " << label;
            break;
        case VMInstructionType::CALL:
            ss << "CALL " << label;
            break;
        case VMInstructionType::RET:
            ss << "RET";
            break;
        case VMInstructionType::LOAD:
            ss << "LOAD " << operand;
            break;
        case VMInstructionType::STORE:
            ss << "STORE " << operand;
            break;
        case VMInstructionType::LOAD_GLOBAL:
            ss << "LOAD_GLOBAL " << operand;
            break;
        case VMInstructionType::STORE_GLOBAL:
            ss << "STORE_GLOBAL " << operand;
            break;
        case VMInstructionType::ARRAY_LOAD:
            ss << "ARRAY_LOAD";
            break;
        case VMInstructionType::ARRAY_STORE:
            ss << "ARRAY_STORE";
            break;
        case VMInstructionType::NOP:
            ss << "NOP";
            break;
        case VMInstructionType::HALT:
            ss << "HALT";
            break;
        case VMInstructionType::PRINT:
            ss << "PRINT";
            break;
        case VMInstructionType::PRINTLN:
            ss << "PRINTLN";
            break;
    }
    
    return ss.str();
}

// ==================== VMFunction实现 ====================
void VMFunction::addInstruction(const VMInstruction& inst) {
    instructions.push_back(inst);
}

void VMFunction::addLabel(const string& label) {
    labels[label] = instructions.size();
}

int VMFunction::getLabelAddress(const string& label) const {
    auto it = labels.find(label);
    return (it != labels.end()) ? it->second : -1;
}

string VMFunction::toString() const {
    stringstream ss;
    ss << "Function: " << name << " (params: " << paramCount << ", locals: " << localVarCount << ")\n";
    
    for (size_t i = 0; i < instructions.size(); ++i) {
        // 检查是否有标签指向这个位置
        for (const auto& label : labels) {
            if (label.second == (int)i) {
                ss << label.first << ":\n";
            }
        }
        ss << "  " << i << ": " << instructions[i].toString() << "\n";
    }
    
    return ss.str();
}

// ==================== VMProgram实现 ====================
VMFunction* VMProgram::addFunction(const string& name) {
    functions.emplace_back(name);
    VMFunction* func = &functions.back();
    functionMap[name] = func;
    return func;
}

VMFunction* VMProgram::getFunction(const string& name) {
    auto it = functionMap.find(name);
    return (it != functionMap.end()) ? it->second : nullptr;
}

void VMProgram::addGlobal(const string& name) {
    globals.push_back(name);
}

string VMProgram::toString() const {
    stringstream ss;
    ss << "VM Program:\n";
    ss << "Globals: ";
    for (const auto& global : globals) {
        ss << global << " ";
    }
    ss << "\n\n";
    
    for (const auto& func : functions) {
        ss << func.toString() << "\n";
    }
    
    return ss.str();
}

vector<uint8_t> VMProgram::generateBinary() const {
    vector<uint8_t> binary;
    
    // 简单的二进制格式：
    // 4字节: 魔数 "VMBC"
    // 4字节: 版本号
    // 4字节: 全局变量数量
    // 4字节: 函数数量
    // 然后是全局变量表和函数表
    
    // 魔数
    binary.push_back('V'); binary.push_back('M'); binary.push_back('B'); binary.push_back('C');
    
    // 版本号 (1)
    binary.push_back(0); binary.push_back(0); binary.push_back(0); binary.push_back(1);
    
    // 全局变量数量
    uint32_t globalCount = globals.size();
    binary.push_back((globalCount >> 24) & 0xFF);
    binary.push_back((globalCount >> 16) & 0xFF);
    binary.push_back((globalCount >> 8) & 0xFF);
    binary.push_back(globalCount & 0xFF);
    
    // 函数数量
    uint32_t funcCount = functions.size();
    binary.push_back((funcCount >> 24) & 0xFF);
    binary.push_back((funcCount >> 16) & 0xFF);
    binary.push_back((funcCount >> 8) & 0xFF);
    binary.push_back(funcCount & 0xFF);
    
    return binary;
}

// ==================== SSAConverter实现 ====================
SSAConverter::SSAConverter() 
    : program(nullptr), currentFunction(nullptr), 
      nextLocalOffset(0), nextGlobalAddress(0), labelCounter(0) {
}

SSAConverter::~SSAConverter() {
    if (program) {
        delete program;
    }
}

VMProgram* SSAConverter::convert(Program* ast) {
    program = new VMProgram();
    
    // 转换程序
    ast->accept(this);
    
    return program;
}

string SSAConverter::createLabel(const string& base) {
    return base + to_string(labelCounter++);
}

void SSAConverter::addInstruction(VMInstructionType type, int operand, const string& label) {
    if (currentFunction) {
        currentFunction->addInstruction(VMInstruction(type, operand, label));
    }
}

int SSAConverter::getVariableOffset(const string& name) {
    auto it = localVars.find(name);
    if (it != localVars.end()) {
        return it->second;
    }
    return -1; // 未找到
}

void SSAConverter::declareVariable(const string& name) {
    if (localVars.find(name) == localVars.end()) {
        localVars[name] = nextLocalOffset++;
        currentFunction->localVarCount = nextLocalOffset;
    }
}

// 表达式和语句转换的实现已分离到单独的文件中
// expression_converter.cpp - 处理所有表达式转换
// statement_converter.cpp - 处理所有语句转换

// ==================== AST访问者模式实现 ====================
void SSAConverter::visit(Expression* expr) {
    convertExpression(expr);
}

void SSAConverter::visit(ConstantExpression* expr) {
    convertConstantExpression(expr);
}

void SSAConverter::visit(VariableExpression* expr) {
    convertVariableExpression(expr);
}

void SSAConverter::visit(UnaryExpression* expr) {
    convertUnaryExpression(expr);
}

void SSAConverter::visit(BinaryExpression* expr) {
    convertBinaryExpression(expr);
}

void SSAConverter::visit(AssignExpression* expr) {
    convertAssignExpression(expr);
}

void SSAConverter::visit(AccessExpression* expr) {
    // 数组访问，简化处理
    if (expr->target) {
        convertExpression(expr->target);
    }
    if (expr->key) {
        convertExpression(expr->key);
    }
    addInstruction(VMInstructionType::ARRAY_LOAD);
}

void SSAConverter::visit(CallExpression* expr) {
    convertCallExpression(expr);
}



void SSAConverter::visit(CastExpression* expr) {
    convertExpression(expr->operand);
}

void SSAConverter::visit(Program* program) {
    if (!program) return;
    
    for (auto stmt : program->statements) {
        convertStatement(stmt);
    }
}

void SSAConverter::visit(Statement* stmt) {
    convertStatement(stmt);
}

void SSAConverter::visit(ImportStatement* stmt) {
    // 导入语句在编译时处理，不需要生成代码
}

void SSAConverter::visit(ExpressionStatement* stmt) {
    convertExpressionStatement(stmt);
}

void SSAConverter::visit(VariableDefinition* stmt) {
    convertVariableDefinition(stmt);
}

void SSAConverter::visit(IfStatement* stmt) {
    convertIfStatement(stmt);
}

void SSAConverter::visit(WhileStatement* stmt) {
    convertWhileStatement(stmt);
}

void SSAConverter::visit(ForStatement* stmt) {
    convertForStatement(stmt);
}

void SSAConverter::visit(DoWhileStatement* stmt) {
    // do-while循环转换
    if (!stmt) return;
    
    string loopLabel = createLabel("doloop");
    string endLabel = createLabel("enddoloop");
    
    // 循环开始标签
    currentFunction->addLabel(loopLabel);
    
    // 转换循环体
    convertStatement(stmt->body);
    
    // 转换条件表达式
    convertExpression(stmt->condition);
    
    // 条件为真时跳回循环开始
    addInstruction(VMInstructionType::JNZ, 0, loopLabel);
    
    // 循环结束标签
    currentFunction->addLabel(endLabel);
}

void SSAConverter::visit(BlockStatement* stmt) {
    convertBlockStatement(stmt);
}

void SSAConverter::visit(StructDefinition* stmt) {
    // 结构体定义在编译时处理，不需要生成代码
}

void SSAConverter::visit(ClassDefinition* stmt) {
    // 类定义在编译时处理，不需要生成代码
}

void SSAConverter::visit(BreakStatement* stmt) {
    // break语句需要跳转到循环结束，这里简化处理
    addInstruction(VMInstructionType::JMP, 0, "break_target");
}

void SSAConverter::visit(ContinueStatement* stmt) {
    // continue语句需要跳转到循环开始，这里简化处理
    addInstruction(VMInstructionType::JMP, 0, "continue_target");
}

void SSAConverter::visit(ReturnStatement* stmt) {
    convertReturnStatement(stmt);
}

void SSAConverter::visit(TryStatement* stmt) {
    // try-catch语句，简化处理
    if (stmt->tryBlock) {
        convertStatement(stmt->tryBlock);
    }
    if (stmt->catchBlocks[0]) {
        convertStatement(stmt->catchBlocks[0]);
    }
}

void SSAConverter::visit(SwitchStatement* stmt) {
    // switch语句，简化处理为if-else链
    if (stmt->expression) {
        convertExpression(stmt->expression);
    }
}

void SSAConverter::visit(FunctionPrototype* stmt) {
    // 函数原型，在编译时处理
}

void SSAConverter::visit(Identifier* id) {
    // 标识符访问
}

void SSAConverter::visit(Variable* var) {
    // 变量访问
}



// ==================== Compiler实现 ====================
VMProgram* Compiler::compile(Program* ast) {
    return converter.convert(ast);
}

bool Compiler::compileToFile(Program* ast, const string& outputFile) {
    VMProgram* vmProgram = compile(ast);
    if (!vmProgram) return false;
    
    vector<uint8_t> binary = vmProgram->generateBinary();
    
    ofstream file(outputFile, ios::binary);
    if (!file.is_open()) return false;
    
    file.write(reinterpret_cast<const char*>(binary.data()), binary.size());
    file.close();
    
    return true;
}

string Compiler::generateAssembly(Program* ast) {
    VMProgram* vmProgram = compile(ast);
    if (!vmProgram) return "";
    
    return vmProgram->toString();
}
