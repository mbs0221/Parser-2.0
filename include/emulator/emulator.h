#ifndef EMULATOR_H
#define EMULATOR_H

#include <vector>
#include <stack>
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <sstream>
#include <chrono>
#include <thread>
#include <iomanip>
#include "emulator/mmio.h"

using namespace std;

// 虚拟机指令类型枚举
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

// 虚拟机指令结构
struct VMInstruction {
    VMInstructionType type;
    int operand;
    std::string label;
    
    VMInstruction(VMInstructionType t, int op = 0, const std::string& lbl = "")
        : type(t), operand(op), label(lbl) {}
};

// 虚拟机函数结构
struct VMFunction {
    std::string name;
    std::vector<VMInstruction> instructions;
    std::map<std::string, int> labels;
    int localVarCount;
    int paramCount;
    
    VMFunction(const std::string& n) : name(n), localVarCount(0), paramCount(0) {}
};

// 虚拟机程序结构
struct VMProgram {
    std::vector<VMFunction> functions;
    std::map<std::string, VMFunction*> functionMap;
    std::vector<std::string> globals;
    
    VMFunction* getFunction(const std::string& name);
};

// 虚拟机值类型
struct VMValue {
    enum Type { INT, DOUBLE, BOOL, STRING, VOID } type;
    union {
        int intVal;
        double doubleVal;
        bool boolVal;
    };
    std::string stringVal;
    
    VMValue() : type(VOID), intVal(0) {}
    VMValue(int val) : type(INT), intVal(val) {}
    VMValue(double val) : type(DOUBLE), doubleVal(val) {}
    VMValue(bool val) : type(BOOL), boolVal(val) {}
    VMValue(const std::string& val) : type(STRING), stringVal(val) {}
    
    // 转换为布尔值
    bool toBool() const;
    
    // 转换为字符串
    std::string toString() const;
};

// 虚拟机执行上下文
struct VMContext {
    std::stack<VMValue> operandStack;
    std::vector<VMValue> localVariables;
    std::vector<VMValue> globalVariables;
    std::vector<VMValue> callStack;
    int programCounter;
    bool halted;
    
    VMContext() : programCounter(0), halted(false) {}
};

// 虚拟机加载器
class VMLoader {
public:
    static VMProgram* loadFromFile(const std::string& filename);
    static VMProgram* loadFromBinary(const std::vector<uint8_t>& binary);
    
private:
    static bool readHeader(std::ifstream& file, uint32_t& version, uint32_t& globalCount, uint32_t& funcCount);
    static VMFunction* readFunction(std::ifstream& file);
    static VMInstruction readInstruction(std::ifstream& file);
};

// 虚拟机执行器
class VMExecutor {
private:
    VMProgram* program;
    VMContext context;
    VMFunction* currentFunction;
    std::unique_ptr<MMIOManager> mmioManager;
    
public:
    VMExecutor(VMProgram* prog);
    ~VMExecutor();
    
    bool execute();
    bool executeFunction(const std::string& functionName);
    void reset();
    
    // MMIO访问方法
    uint32_t readMMIO(uint32_t address);
    void writeMMIO(uint32_t address, uint32_t value);
    uint8_t readMMIOByte(uint32_t address);
    void writeMMIOByte(uint32_t address, uint8_t value);
    uint16_t readMMIOWord(uint32_t address);
    void writeMMIOWord(uint32_t address, uint16_t value);
    
    // 外设访问
    MMIOManager* getMMIOManager() const { return mmioManager.get(); }
    
private:
    void executeInstruction(const VMInstruction& inst);
    void executeArithmetic(const VMInstruction& inst);
    void executeComparison(const VMInstruction& inst);
    void executeJump(const VMInstruction& inst);
    void executeStack(const VMInstruction& inst);
    void executeVariable(const VMInstruction& inst);
    void executeCall(const VMInstruction& inst);
    void executePrint(const VMInstruction& inst);
    void executePeripheralInstruction(const PeripheralInstruction& inst);
    
    VMValue popValue();
    void pushValue(const VMValue& value);
    VMValue& getLocalVariable(int offset);
    VMValue& getGlobalVariable(int address);
};

// 虚拟机主类
class Emulator {
private:
    VMProgram* program;
    VMExecutor* executor;
    
public:
    Emulator();
    ~Emulator();
    
    bool loadProgram(const std::string& filename);
    bool loadProgramFromBinary(const std::vector<uint8_t>& binary);
    bool execute();
    bool executeFunction(const std::string& functionName);
    void reset();
    
    // 调试功能
    void setDebugMode(bool debug);
    void printProgramInfo();
    void printExecutionTrace();
    
private:
    bool debugMode;
};

#endif // EMULATOR_H
