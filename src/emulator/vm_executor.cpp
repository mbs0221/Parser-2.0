#include "emulator/emulator.h"
#include "emulator/peripherals.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>

// 虚拟机执行器实现
// 负责执行虚拟机指令，包括外设支持

VMExecutor::VMExecutor(VMProgram* prog) : program(prog), currentFunction(nullptr) {
    if (program) {
        context.globalVariables.resize(program->globals.size());
    }
    
    // 初始化外设管理器
    peripheralManager = std::make_unique<PeripheralManager>();
    peripheralManager->initializeAll();
}

VMExecutor::~VMExecutor() {
    if (peripheralManager) {
        peripheralManager->shutdownAll();
    }
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
    // 使用外设系统进行输出
    ConsolePeripheral* console = peripheralManager->getConsole();
    if (!console) {
        std::cerr << "Error: Console peripheral not available" << std::endl;
        return;
    }
    
    switch (inst.type) {
        case VMInstructionType::PRINT: {
            if (!context.operandStack.empty()) {
                VMValue val = context.operandStack.top();
                console->write(val.toString());
            }
            break;
        }
        
        case VMInstructionType::PRINTLN: {
            if (!context.operandStack.empty()) {
                VMValue val = context.operandStack.top();
                console->writeLine(val.toString());
            } else {
                console->writeLine("");
            }
            break;
        }
        
        default:
            break;
    }
}

// 外设指令执行
void VMExecutor::executePeripheralInstruction(const PeripheralInstruction& inst) {
    switch (inst.type) {
        case PeripheralInstructionType::CONSOLE_WRITE: {
            if (inst.parameters.size() >= 1) {
                ConsolePeripheral* console = peripheralManager->getConsole();
                if (console) {
                    console->write(inst.parameters[0]);
                }
            }
            break;
        }
        
        case PeripheralInstructionType::CONSOLE_WRITELN: {
            if (inst.parameters.size() >= 1) {
                ConsolePeripheral* console = peripheralManager->getConsole();
                if (console) {
                    console->writeLine(inst.parameters[0]);
                }
            } else {
                ConsolePeripheral* console = peripheralManager->getConsole();
                if (console) {
                    console->writeLine("");
                }
            }
            break;
        }
        
        case PeripheralInstructionType::CONSOLE_READ: {
            ConsolePeripheral* console = peripheralManager->getConsole();
            if (console) {
                std::string input = console->readLine();
                pushValue(VMValue(input));
            }
            break;
        }
        
        case PeripheralInstructionType::FILE_OPEN: {
            if (inst.parameters.size() >= 2) {
                FileSystemPeripheral* fs = peripheralManager->getFileSystem();
                if (fs) {
                    int handle = fs->openFile(inst.parameters[0], inst.parameters[1]);
                    pushValue(VMValue(handle));
                }
            }
            break;
        }
        
        case PeripheralInstructionType::FILE_CLOSE: {
            if (inst.parameters.size() >= 1) {
                FileSystemPeripheral* fs = peripheralManager->getFileSystem();
                if (fs) {
                    int handle = std::stoi(inst.parameters[0]);
                    bool result = fs->closeFile(handle);
                    pushValue(VMValue(result));
                }
            }
            break;
        }
        
        case PeripheralInstructionType::FILE_READ: {
            if (inst.parameters.size() >= 2) {
                FileSystemPeripheral* fs = peripheralManager->getFileSystem();
                if (fs) {
                    int handle = std::stoi(inst.parameters[0]);
                    size_t size = std::stoul(inst.parameters[1]);
                    std::string data = fs->readFile(handle, size);
                    pushValue(VMValue(data));
                }
            }
            break;
        }
        
        case PeripheralInstructionType::FILE_WRITE: {
            if (inst.parameters.size() >= 2) {
                FileSystemPeripheral* fs = peripheralManager->getFileSystem();
                if (fs) {
                    int handle = std::stoi(inst.parameters[0]);
                    bool result = fs->writeFile(handle, inst.parameters[1]);
                    pushValue(VMValue(result));
                }
            }
            break;
        }
        
        case PeripheralInstructionType::TIMER_SLEEP: {
            if (inst.parameters.size() >= 1) {
                TimerPeripheral* timer = peripheralManager->getTimer();
                if (timer) {
                    int milliseconds = std::stoi(inst.parameters[0]);
                    timer->sleep(milliseconds);
                }
            }
            break;
        }
        
        case PeripheralInstructionType::SYS_EXIT: {
            if (inst.parameters.size() >= 1) {
                int exitCode = std::stoi(inst.parameters[0]);
                exit(exitCode);
            } else {
                exit(0);
            }
            break;
        }
        
        default:
            std::cerr << "Error: Unsupported peripheral instruction" << std::endl;
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
