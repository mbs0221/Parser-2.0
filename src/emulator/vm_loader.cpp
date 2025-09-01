#include "emulator/emulator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

// 虚拟机加载器实现
// 负责从二进制文件加载虚拟机程序

VMLoader::VMLoader() {
}

VMLoader::~VMLoader() {
}

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
    
    // 检查魔数 "VMBC"
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
    
    // 解析全局变量表
    size_t offset = 16;
    for (uint32_t i = 0; i < globalCount; ++i) {
        if (offset >= binary.size()) {
            std::cerr << "Error: Unexpected end of file while reading globals" << std::endl;
            delete program;
            return nullptr;
        }
        
        // 读取全局变量名长度
        uint8_t nameLen = binary[offset++];
        if (offset + nameLen > binary.size()) {
            std::cerr << "Error: Unexpected end of file while reading global name" << std::endl;
            delete program;
            return nullptr;
        }
        
        // 读取全局变量名
        std::string globalName(reinterpret_cast<const char*>(&binary[offset]), nameLen);
        offset += nameLen;
        
        program->addGlobal(globalName);
        std::cout << "  Global: " << globalName << std::endl;
    }
    
    // 解析函数表
    for (uint32_t i = 0; i < funcCount; ++i) {
        if (offset >= binary.size()) {
            std::cerr << "Error: Unexpected end of file while reading functions" << std::endl;
            delete program;
            return nullptr;
        }
        
        // 读取函数名长度
        uint8_t nameLen = binary[offset++];
        if (offset + nameLen > binary.size()) {
            std::cerr << "Error: Unexpected end of file while reading function name" << std::endl;
            delete program;
            return nullptr;
        }
        
        // 读取函数名
        std::string funcName(reinterpret_cast<const char*>(&binary[offset]), nameLen);
        offset += nameLen;
        
        // 读取函数参数数量
        if (offset >= binary.size()) {
            std::cerr << "Error: Unexpected end of file while reading function params" << std::endl;
            delete program;
            return nullptr;
        }
        uint32_t paramCount = binary[offset++];
        
        // 读取局部变量数量
        if (offset >= binary.size()) {
            std::cerr << "Error: Unexpected end of file while reading function locals" << std::endl;
            delete program;
            return nullptr;
        }
        uint32_t localCount = binary[offset++];
        
        // 创建函数
        VMFunction* func = program->addFunction(funcName);
        func->paramCount = paramCount;
        func->localVarCount = localCount;
        
        // 读取指令数量
        if (offset + 4 > binary.size()) {
            std::cerr << "Error: Unexpected end of file while reading instruction count" << std::endl;
            delete program;
            return nullptr;
        }
        uint32_t instCount = (binary[offset] << 24) | (binary[offset+1] << 16) | 
                           (binary[offset+2] << 8) | binary[offset+3];
        offset += 4;
        
        std::cout << "  Function: " << funcName 
                  << " (params: " << paramCount 
                  << ", locals: " << localCount 
                  << ", instructions: " << instCount << ")" << std::endl;
        
        // 读取指令
        for (uint32_t j = 0; j < instCount; ++j) {
            if (offset >= binary.size()) {
                std::cerr << "Error: Unexpected end of file while reading instructions" << std::endl;
                delete program;
                return nullptr;
            }
            
            VMInstruction inst = readInstruction(binary, offset);
            func->addInstruction(inst);
        }
    }
    
    std::cout << "Program loaded successfully" << std::endl;
    return program;
}

VMInstruction VMLoader::readInstruction(const std::vector<uint8_t>& binary, size_t& offset) {
    if (offset >= binary.size()) {
        std::cerr << "Error: Unexpected end of file while reading instruction" << std::endl;
        return VMInstruction(VMInstructionType::NOP);
    }
    
    // 读取指令类型
    uint8_t instType = binary[offset++];
    VMInstructionType type = static_cast<VMInstructionType>(instType);
    
    int operand = 0;
    std::string label = "";
    
    // 根据指令类型读取操作数
    switch (type) {
        case VMInstructionType::PUSH:
        case VMInstructionType::LOAD:
        case VMInstructionType::STORE:
        case VMInstructionType::LOAD_GLOBAL:
        case VMInstructionType::STORE_GLOBAL:
            // 这些指令有立即数操作数
            if (offset + 4 <= binary.size()) {
                operand = (binary[offset] << 24) | (binary[offset+1] << 16) | 
                         (binary[offset+2] << 8) | binary[offset+3];
                offset += 4;
            }
            break;
            
        case VMInstructionType::JMP:
        case VMInstructionType::JZ:
        case VMInstructionType::JNZ:
        case VMInstructionType::JGT:
        case VMInstructionType::JLT:
        case VMInstructionType::CALL:
            // 这些指令有标签
            if (offset < binary.size()) {
                uint8_t labelLen = binary[offset++];
                if (offset + labelLen <= binary.size()) {
                    label = std::string(reinterpret_cast<const char*>(&binary[offset]), labelLen);
                    offset += labelLen;
                }
            }
            break;
            
        default:
            // 其他指令没有操作数
            break;
    }
    
    return VMInstruction(type, operand, label);
}

bool VMLoader::readHeader(std::ifstream& file, uint32_t& version, uint32_t& globalCount, uint32_t& funcCount) {
    // 读取魔数
    char magic[4];
    file.read(magic, 4);
    if (strncmp(magic, "VMBC", 4) != 0) {
        return false;
    }
    
    // 读取版本号
    file.read(reinterpret_cast<char*>(&version), 4);
    
    // 读取全局变量数量
    file.read(reinterpret_cast<char*>(&globalCount), 4);
    
    // 读取函数数量
    file.read(reinterpret_cast<char*>(&funcCount), 4);
    
    return true;
}

VMFunction* VMLoader::readFunction(std::ifstream& file) {
    // 读取函数名长度
    uint8_t nameLen;
    file.read(reinterpret_cast<char*>(&nameLen), 1);
    
    // 读取函数名
    std::string funcName(nameLen, '\0');
    file.read(&funcName[0], nameLen);
    
    // 读取参数数量和局部变量数量
    uint8_t paramCount, localCount;
    file.read(reinterpret_cast<char*>(&paramCount), 1);
    file.read(reinterpret_cast<char*>(&localCount), 1);
    
    // 创建函数
    VMFunction* func = new VMFunction(funcName);
    func->paramCount = paramCount;
    func->localVarCount = localCount;
    
    // 读取指令数量
    uint32_t instCount;
    file.read(reinterpret_cast<char*>(&instCount), 4);
    
    // 读取指令
    for (uint32_t i = 0; i < instCount; ++i) {
        VMInstruction inst = readInstruction(file);
        func->addInstruction(inst);
    }
    
    return func;
}

VMInstruction VMLoader::readInstruction(std::ifstream& file) {
    // 读取指令类型
    uint8_t instType;
    file.read(reinterpret_cast<char*>(&instType), 1);
    VMInstructionType type = static_cast<VMInstructionType>(instType);
    
    int operand = 0;
    std::string label = "";
    
    // 根据指令类型读取操作数
    switch (type) {
        case VMInstructionType::PUSH:
        case VMInstructionType::LOAD:
        case VMInstructionType::STORE:
        case VMInstructionType::LOAD_GLOBAL:
        case VMInstructionType::STORE_GLOBAL:
            file.read(reinterpret_cast<char*>(&operand), 4);
            break;
            
        case VMInstructionType::JMP:
        case VMInstructionType::JZ:
        case VMInstructionType::JNZ:
        case VMInstructionType::JGT:
        case VMInstructionType::JLT:
        case VMInstructionType::CALL:
            {
                uint8_t labelLen;
                file.read(reinterpret_cast<char*>(&labelLen), 1);
                label.resize(labelLen);
                file.read(&label[0], labelLen);
            }
            break;
            
        default:
            break;
    }
    
    return VMInstruction(type, operand, label);
}
