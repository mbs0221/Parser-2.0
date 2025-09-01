#include "emulator/emulator.h"
#include <iostream>
#include <sstream>

// 虚拟机机器类实现
// 这个文件包含虚拟机的高级抽象和状态管理

class VMMachine {
private:
    Emulator* emulator;
    bool running;
    std::string currentProgram;
    
public:
    VMMachine() : emulator(new Emulator()), running(false) {}
    
    ~VMMachine() {
        if (emulator) {
            delete emulator;
        }
    }
    
    bool loadProgram(const std::string& filename) {
        currentProgram = filename;
        bool success = emulator->loadProgram(filename);
        if (success) {
            std::cout << "Program loaded successfully: " << filename << std::endl;
        } else {
            std::cerr << "Failed to load program: " << filename << std::endl;
        }
        return success;
    }
    
    bool run() {
        if (!emulator) {
            std::cerr << "No emulator available" << std::endl;
            return false;
        }
        
        running = true;
        std::cout << "Starting VM execution..." << std::endl;
        
        bool success = emulator->execute();
        
        running = false;
        if (success) {
            std::cout << "VM execution completed successfully" << std::endl;
        } else {
            std::cout << "VM execution failed" << std::endl;
        }
        
        return success;
    }
    
    bool runFunction(const std::string& functionName) {
        if (!emulator) {
            std::cerr << "No emulator available" << std::endl;
            return false;
        }
        
        std::cout << "Executing function: " << functionName << std::endl;
        return emulator->executeFunction(functionName);
    }
    
    void reset() {
        if (emulator) {
            emulator->reset();
        }
        running = false;
    }
    
    void setDebugMode(bool debug) {
        if (emulator) {
            emulator->setDebugMode(debug);
        }
    }
    
    void printStatus() {
        std::cout << "=== VM Machine Status ===" << std::endl;
        std::cout << "Current program: " << currentProgram << std::endl;
        std::cout << "Running: " << (running ? "Yes" : "No") << std::endl;
        std::cout << "=========================" << std::endl;
    }
    
    Emulator* getEmulator() const {
        return emulator;
    }
};

// 导出一些基本功能
extern "C" {
    VMMachine* create_vm_machine() {
        return new VMMachine();
    }
    
    void destroy_vm_machine(VMMachine* machine) {
        if (machine) {
            delete machine;
        }
    }
    
    bool vm_load_program(VMMachine* machine, const char* filename) {
        if (!machine || !filename) return false;
        return machine->loadProgram(filename);
    }
    
    bool vm_run(VMMachine* machine) {
        if (!machine) return false;
        return machine->run();
    }
    
    bool vm_run_function(VMMachine* machine, const char* function_name) {
        if (!machine || !function_name) return false;
        return machine->runFunction(function_name);
    }
    
    void vm_reset(VMMachine* machine) {
        if (machine) {
            machine->reset();
        }
    }
    
    void vm_set_debug(VMMachine* machine, bool debug) {
        if (machine) {
            machine->setDebugMode(debug);
        }
    }
}
