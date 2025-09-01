#include "emulator/emulator.h"
#include <iostream>
#include <sstream>
#include <iomanip>

// VMValue实现
bool VMValue::toBool() const {
    switch (type) {
        case INT: return intVal != 0;
        case DOUBLE: return doubleVal != 0.0;
        case BOOL: return boolVal;
        case STRING: return !stringVal.empty();
        case VOID: return false;
    }
    return false;
}

std::string VMValue::toString() const {
    switch (type) {
        case INT: return std::to_string(intVal);
        case DOUBLE: return std::to_string(doubleVal);
        case BOOL: return boolVal ? "true" : "false";
        case STRING: return stringVal;
        case VOID: return "void";
    }
    return "unknown";
}

// VMProgram实现
VMFunction* VMProgram::getFunction(const std::string& name) {
    auto it = functionMap.find(name);
    return (it != functionMap.end()) ? it->second : nullptr;
}

// VMLoader实现
VMProgram* VMLoader::loadFromFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return nullptr;
    }
    
    // 读取文件内容
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> binary(fileSize);
    file.read(reinterpret_cast<char*>(binary.data()), fileSize);
    file.close();
    
    return loadFromBinary(binary);
}

VMProgram* VMLoader::loadFromBinary(const std::vector<uint8_t>& binary) {
    if (binary.size() < 16) {
        std::cerr << "Error: Invalid binary format (too small)" << std::endl;
        return nullptr;
    }
    
    // 检查魔数
    if (binary[0] != 'V' || binary[1] != 'M' || binary[2] != 'B' || binary[3] != 'C') {
        std::cerr << "Error: Invalid magic number" << std::endl;
        return nullptr;
    }
    
    VMProgram* program = new VMProgram();
    
    // 读取版本号
    uint32_t version = (binary[4] << 24) | (binary[5] << 16) | (binary[6] << 8) | binary[7];
    if (version != 1) {
        std::cerr << "Error: Unsupported version " << version << std::endl;
        delete program;
        return nullptr;
    }
    
    // 读取全局变量数量
    uint32_t globalCount = (binary[8] << 24) | (binary[9] << 16) | (binary[10] << 8) | binary[11];
    
    // 读取函数数量
    uint32_t funcCount = (binary[12] << 24) | (binary[13] << 16) | (binary[14] << 8) | binary[15];
    
    std::cout << "Loading VM program: version=" << version 
              << ", globals=" << globalCount 
              << ", functions=" << funcCount << std::endl;
    
    // 这里简化处理，实际应该解析完整的二进制格式
    // 目前只创建基本的程序结构
    
    return program;
}

// VMExecutor实现
VMExecutor::VMExecutor(VMProgram* prog) : program(prog), currentFunction(nullptr) {
    if (program) {
        context.globalVariables.resize(program->globals.size());
    }
}

VMExecutor::~VMExecutor() {
    // 程序由外部管理，这里不删除
}

bool VMExecutor::execute() {
    if (!program || program->functions.empty()) {
        std::cerr << "Error: No program to execute" << std::endl;
        return false;
    }
    
    // 执行main函数
    return executeFunction("main");
}

bool VMExecutor::executeFunction(const std::string& functionName) {
    VMFunction* func = program->getFunction(functionName);
    if (!func) {
        std::cerr << "Error: Function '" << functionName << "' not found" << std::endl;
        return false;
    }
    
    currentFunction = func;
    context.programCounter = 0;
    context.halted = false;
    context.localVariables.resize(func->localVarCount);
    
    std::cout << "Executing function: " << functionName << std::endl;
    
    while (context.programCounter < func->instructions.size() && !context.halted) {
        const VMInstruction& inst = func->instructions[context.programCounter];
        executeInstruction(inst);
        context.programCounter++;
    }
    
    return !context.halted;
}

void VMExecutor::reset() {
    context.operandStack = std::stack<VMValue>();
    context.localVariables.clear();
    context.callStack.clear();
    context.programCounter = 0;
    context.halted = false;
    currentFunction = nullptr;
}

void VMExecutor::executeInstruction(const VMInstruction& inst) {
    switch (inst.type) {
        case VMInstructionType::PUSH:
        case VMInstructionType::POP:
        case VMInstructionType::DUP:
        case VMInstructionType::SWAP:
            executeStack(inst);
            break;
            
        case VMInstructionType::ADD:
        case VMInstructionType::SUB:
        case VMInstructionType::MUL:
        case VMInstructionType::DIV:
        case VMInstructionType::MOD:
        case VMInstructionType::AND:
        case VMInstructionType::OR:
        case VMInstructionType::XOR:
        case VMInstructionType::NOT:
        case VMInstructionType::SHL:
        case VMInstructionType::SHR:
            executeArithmetic(inst);
            break;
            
        case VMInstructionType::EQ:
        case VMInstructionType::NE:
        case VMInstructionType::LT:
        case VMInstructionType::LE:
        case VMInstructionType::GT:
        case VMInstructionType::GE:
            executeComparison(inst);
            break;
            
        case VMInstructionType::JMP:
        case VMInstructionType::JZ:
        case VMInstructionType::JNZ:
        case VMInstructionType::JGT:
        case VMInstructionType::JLT:
            executeJump(inst);
            break;
            
        case VMInstructionType::LOAD:
        case VMInstructionType::STORE:
        case VMInstructionType::LOAD_GLOBAL:
        case VMInstructionType::STORE_GLOBAL:
            executeVariable(inst);
            break;
            
        case VMInstructionType::CALL:
        case VMInstructionType::RET:
            executeCall(inst);
            break;
            
        case VMInstructionType::PRINT:
        case VMInstructionType::PRINTLN:
            executePrint(inst);
            break;
            
        case VMInstructionType::NOP:
            // 什么都不做
            break;
            
        case VMInstructionType::HALT:
            context.halted = true;
            break;
            
        default:
            std::cerr << "Error: Unknown instruction type" << std::endl;
            break;
    }
}

void VMExecutor::executeStack(const VMInstruction& inst) {
    switch (inst.type) {
        case VMInstructionType::PUSH:
            pushValue(VMValue(inst.operand));
            break;
            
        case VMInstructionType::POP:
            popValue();
            break;
            
        case VMInstructionType::DUP: {
            VMValue val = popValue();
            pushValue(val);
            pushValue(val);
            break;
        }
        
        case VMInstructionType::SWAP: {
            VMValue val1 = popValue();
            VMValue val2 = popValue();
            pushValue(val1);
            pushValue(val2);
            break;
        }
        
        default:
            break;
    }
}

void VMExecutor::executeArithmetic(const VMInstruction& inst) {
    VMValue result;
    
    switch (inst.type) {
        case VMInstructionType::ADD: {
            VMValue b = popValue();
            VMValue a = popValue();
            if (a.type == VMValue::INT && b.type == VMValue::INT) {
                result = VMValue(a.intVal + b.intVal);
            } else if (a.type == VMValue::DOUBLE || b.type == VMValue::DOUBLE) {
                double aVal = (a.type == VMValue::INT) ? a.intVal : a.doubleVal;
                double bVal = (b.type == VMValue::INT) ? b.intVal : b.doubleVal;
                result = VMValue(aVal + bVal);
            }
            break;
        }
        
        case VMInstructionType::SUB: {
            VMValue b = popValue();
            VMValue a = popValue();
            if (a.type == VMValue::INT && b.type == VMValue::INT) {
                result = VMValue(a.intVal - b.intVal);
            } else if (a.type == VMValue::DOUBLE || b.type == VMValue::DOUBLE) {
                double aVal = (a.type == VMValue::INT) ? a.intVal : a.doubleVal;
                double bVal = (b.type == VMValue::INT) ? b.intVal : b.doubleVal;
                result = VMValue(aVal - bVal);
            }
            break;
        }
        
        case VMInstructionType::MUL: {
            VMValue b = popValue();
            VMValue a = popValue();
            if (a.type == VMValue::INT && b.type == VMValue::INT) {
                result = VMValue(a.intVal * b.intVal);
            } else if (a.type == VMValue::DOUBLE || b.type == VMValue::DOUBLE) {
                double aVal = (a.type == VMValue::INT) ? a.intVal : a.doubleVal;
                double bVal = (b.type == VMValue::INT) ? b.intVal : b.doubleVal;
                result = VMValue(aVal * bVal);
            }
            break;
        }
        
        case VMInstructionType::DIV: {
            VMValue b = popValue();
            VMValue a = popValue();
            if (a.type == VMValue::INT && b.type == VMValue::INT) {
                if (b.intVal == 0) {
                    std::cerr << "Error: Division by zero" << std::endl;
                    return;
                }
                result = VMValue(a.intVal / b.intVal);
            } else if (a.type == VMValue::DOUBLE || b.type == VMValue::DOUBLE) {
                double aVal = (a.type == VMValue::INT) ? a.intVal : a.doubleVal;
                double bVal = (b.type == VMValue::INT) ? b.intVal : b.doubleVal;
                if (bVal == 0.0) {
                    std::cerr << "Error: Division by zero" << std::endl;
                    return;
                }
                result = VMValue(aVal / bVal);
            }
            break;
        }
        
        case VMInstructionType::MOD: {
            VMValue b = popValue();
            VMValue a = popValue();
            if (a.type == VMValue::INT && b.type == VMValue::INT) {
                if (b.intVal == 0) {
                    std::cerr << "Error: Modulo by zero" << std::endl;
                    return;
                }
                result = VMValue(a.intVal % b.intVal);
            }
            break;
        }
        
        case VMInstructionType::AND: {
            VMValue b = popValue();
            VMValue a = popValue();
            result = VMValue(a.toBool() && b.toBool());
            break;
        }
        
        case VMInstructionType::OR: {
            VMValue b = popValue();
            VMValue a = popValue();
            result = VMValue(a.toBool() || b.toBool());
            break;
        }
        
        case VMInstructionType::NOT: {
            VMValue a = popValue();
            result = VMValue(!a.toBool());
            break;
        }
        
        default:
            std::cerr << "Error: Unsupported arithmetic operation" << std::endl;
            return;
    }
    
    pushValue(result);
}

void VMExecutor::executeComparison(const VMInstruction& inst) {
    VMValue b = popValue();
    VMValue a = popValue();
    bool result = false;
    
    switch (inst.type) {
        case VMInstructionType::EQ:
            if (a.type == b.type) {
                switch (a.type) {
                    case VMValue::INT: result = (a.intVal == b.intVal); break;
                    case VMValue::DOUBLE: result = (a.doubleVal == b.doubleVal); break;
                    case VMValue::BOOL: result = (a.boolVal == b.boolVal); break;
                    case VMValue::STRING: result = (a.stringVal == b.stringVal); break;
                    default: break;
                }
            }
            break;
            
        case VMInstructionType::NE:
            if (a.type == b.type) {
                switch (a.type) {
                    case VMValue::INT: result = (a.intVal != b.intVal); break;
                    case VMValue::DOUBLE: result = (a.doubleVal != b.doubleVal); break;
                    case VMValue::BOOL: result = (a.boolVal != b.boolVal); break;
                    case VMValue::STRING: result = (a.stringVal != b.stringVal); break;
                    default: break;
                }
            }
            break;
            
        case VMInstructionType::LT:
            if (a.type == VMValue::INT && b.type == VMValue::INT) {
                result = (a.intVal < b.intVal);
            } else if (a.type == VMValue::DOUBLE || b.type == VMValue::DOUBLE) {
                double aVal = (a.type == VMValue::INT) ? a.intVal : a.doubleVal;
                double bVal = (b.type == VMValue::INT) ? b.intVal : b.doubleVal;
                result = (aVal < bVal);
            }
            break;
            
        case VMInstructionType::LE:
            if (a.type == VMValue::INT && b.type == VMValue::INT) {
                result = (a.intVal <= b.intVal);
            } else if (a.type == VMValue::DOUBLE || b.type == VMValue::DOUBLE) {
                double aVal = (a.type == VMValue::INT) ? a.intVal : a.doubleVal;
                double bVal = (b.type == VMValue::INT) ? b.intVal : b.doubleVal;
                result = (aVal <= bVal);
            }
            break;
            
        case VMInstructionType::GT:
            if (a.type == VMValue::INT && b.type == VMValue::INT) {
                result = (a.intVal > b.intVal);
            } else if (a.type == VMValue::DOUBLE || b.type == VMValue::DOUBLE) {
                double aVal = (a.type == VMValue::INT) ? a.intVal : a.doubleVal;
                double bVal = (b.type == VMValue::INT) ? b.intVal : b.doubleVal;
                result = (aVal > bVal);
            }
            break;
            
        case VMInstructionType::GE:
            if (a.type == VMValue::INT && b.type == VMValue::INT) {
                result = (a.intVal >= b.intVal);
            } else if (a.type == VMValue::DOUBLE || b.type == VMValue::DOUBLE) {
                double aVal = (a.type == VMValue::INT) ? a.intVal : a.doubleVal;
                double bVal = (b.type == VMValue::INT) ? b.intVal : b.doubleVal;
                result = (aVal >= bVal);
            }
            break;
            
        default:
            std::cerr << "Error: Unsupported comparison operation" << std::endl;
            return;
    }
    
    pushValue(VMValue(result));
}

void VMExecutor::executeJump(const VMInstruction& inst) {
    bool shouldJump = false;
    
    switch (inst.type) {
        case VMInstructionType::JMP:
            shouldJump = true;
            break;
            
        case VMInstructionType::JZ: {
            VMValue val = popValue();
            shouldJump = !val.toBool();
            break;
        }
        
        case VMInstructionType::JNZ: {
            VMValue val = popValue();
            shouldJump = val.toBool();
            break;
        }
        
        case VMInstructionType::JGT: {
            VMValue val = popValue();
            shouldJump = val.toBool();
            break;
        }
        
        case VMInstructionType::JLT: {
            VMValue val = popValue();
            shouldJump = val.toBool();
            break;
        }
        
        default:
            std::cerr << "Error: Unsupported jump operation" << std::endl;
            return;
    }
    
    if (shouldJump) {
        // 查找标签地址
        auto it = currentFunction->labels.find(inst.label);
        if (it != currentFunction->labels.end()) {
            context.programCounter = it->second - 1; // -1因为循环会+1
        } else {
            std::cerr << "Error: Label '" << inst.label << "' not found" << std::endl;
        }
    }
}

void VMExecutor::executeVariable(const VMInstruction& inst) {
    switch (inst.type) {
        case VMInstructionType::LOAD: {
            VMValue& var = getLocalVariable(inst.operand);
            pushValue(var);
            break;
        }
        
        case VMInstructionType::STORE: {
            VMValue val = popValue();
            VMValue& var = getLocalVariable(inst.operand);
            var = val;
            break;
        }
        
        case VMInstructionType::LOAD_GLOBAL: {
            VMValue& var = getGlobalVariable(inst.operand);
            pushValue(var);
            break;
        }
        
        case VMInstructionType::STORE_GLOBAL: {
            VMValue val = popValue();
            VMValue& var = getGlobalVariable(inst.operand);
            var = val;
            break;
        }
        
        default:
            std::cerr << "Error: Unsupported variable operation" << std::endl;
            break;
    }
}

void VMExecutor::executeCall(const VMInstruction& inst) {
    switch (inst.type) {
        case VMInstructionType::CALL: {
            // 保存返回地址
            context.callStack.push_back(VMValue(context.programCounter));
            
            // 调用函数
            executeFunction(inst.label);
            break;
        }
        
        case VMInstructionType::RET: {
            if (!context.callStack.empty()) {
                VMValue returnAddr = context.callStack.back();
                context.callStack.pop_back();
                context.programCounter = returnAddr.intVal;
            } else {
                context.halted = true;
            }
            break;
        }
        
        default:
            std::cerr << "Error: Unsupported call operation" << std::endl;
            break;
    }
}

void VMExecutor::executePrint(const VMInstruction& inst) {
    switch (inst.type) {
        case VMInstructionType::PRINT: {
            if (!context.operandStack.empty()) {
                VMValue val = context.operandStack.top();
                std::cout << val.toString();
            }
            break;
        }
        
        case VMInstructionType::PRINTLN: {
            if (!context.operandStack.empty()) {
                VMValue val = context.operandStack.top();
                std::cout << val.toString() << std::endl;
            } else {
                std::cout << std::endl;
            }
            break;
        }
        
        default:
            break;
    }
}

VMValue VMExecutor::popValue() {
    if (context.operandStack.empty()) {
        std::cerr << "Error: Stack underflow" << std::endl;
        return VMValue();
    }
    
    VMValue val = context.operandStack.top();
    context.operandStack.pop();
    return val;
}

void VMExecutor::pushValue(const VMValue& value) {
    context.operandStack.push(value);
}

VMValue& VMExecutor::getLocalVariable(int offset) {
    if (offset < 0 || offset >= context.localVariables.size()) {
        std::cerr << "Error: Invalid local variable offset " << offset << std::endl;
        static VMValue dummy;
        return dummy;
    }
    return context.localVariables[offset];
}

VMValue& VMExecutor::getGlobalVariable(int address) {
    if (address < 0 || address >= context.globalVariables.size()) {
        std::cerr << "Error: Invalid global variable address " << address << std::endl;
        static VMValue dummy;
        return dummy;
    }
    return context.globalVariables[address];
}

// Emulator主类实现
Emulator::Emulator() : program(nullptr), executor(nullptr), debugMode(false) {
}

Emulator::~Emulator() {
    if (executor) delete executor;
    if (program) delete program;
}

bool Emulator::loadProgram(const std::string& filename) {
    if (program) {
        delete program;
        program = nullptr;
    }
    
    program = VMLoader::loadFromFile(filename);
    if (!program) {
        return false;
    }
    
    if (executor) {
        delete executor;
    }
    executor = new VMExecutor(program);
    
    if (debugMode) {
        printProgramInfo();
    }
    
    return true;
}

bool Emulator::loadProgramFromBinary(const std::vector<uint8_t>& binary) {
    if (program) {
        delete program;
        program = nullptr;
    }
    
    program = VMLoader::loadFromBinary(binary);
    if (!program) {
        return false;
    }
    
    if (executor) {
        delete executor;
    }
    executor = new VMExecutor(program);
    
    if (debugMode) {
        printProgramInfo();
    }
    
    return true;
}

bool Emulator::execute() {
    if (!executor) {
        std::cerr << "Error: No program loaded" << std::endl;
        return false;
    }
    
    return executor->execute();
}

bool Emulator::executeFunction(const std::string& functionName) {
    if (!executor) {
        std::cerr << "Error: No program loaded" << std::endl;
        return false;
    }
    
    return executor->executeFunction(functionName);
}

void Emulator::reset() {
    if (executor) {
        executor->reset();
    }
}

void Emulator::setDebugMode(bool debug) {
    debugMode = debug;
}

void Emulator::printProgramInfo() {
    if (!program) {
        std::cout << "No program loaded" << std::endl;
        return;
    }
    
    std::cout << "=== Program Information ===" << std::endl;
    std::cout << "Global variables: " << program->globals.size() << std::endl;
    std::cout << "Functions: " << program->functions.size() << std::endl;
    
    for (const auto& func : program->functions) {
        std::cout << "  Function: " << func.name 
                  << " (params: " << func.paramCount 
                  << ", locals: " << func.localVarCount 
                  << ", instructions: " << func.instructions.size() << ")" << std::endl;
    }
    std::cout << "==========================" << std::endl;
}

void Emulator::printExecutionTrace() {
    // 实现执行跟踪功能
    std::cout << "Execution trace not implemented yet" << std::endl;
}
