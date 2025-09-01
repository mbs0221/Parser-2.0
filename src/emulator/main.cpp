#include "emulator/emulator.h"
#include "emulator/mmio.h"
#include <iostream>
#include <iomanip>

using namespace std;

// 演示MMIO外设使用的示例程序
void demonstrateMMIO() {
    cout << "=== MMIO外设演示 ===" << endl;
    
    // 创建MMIO管理器
    MMIOManager mmioManager;
    if (!mmioManager.initialize()) {
        cerr << "Failed to initialize MMIO manager" << endl;
        return;
    }
    
    cout << "MMIO管理器初始化成功" << endl;
    cout << "已注册的设备: ";
    auto devices = mmioManager.getDeviceNames();
    for (const auto& device : devices) {
        cout << device << " ";
    }
    cout << endl;
    
    // 演示控制台外设
    cout << "\n--- 控制台外设演示 ---" << endl;
    
    // 写入控制台数据寄存器
    mmioManager.writeMemory(MMIO::CONSOLE_BASE + MMIO::Console::DATA_REG, 'H');
    mmioManager.writeMemory(MMIO::CONSOLE_BASE + MMIO::Console::DATA_REG, 'e');
    mmioManager.writeMemory(MMIO::CONSOLE_BASE + MMIO::Console::DATA_REG, 'l');
    mmioManager.writeMemory(MMIO::CONSOLE_BASE + MMIO::Console::DATA_REG, 'l');
    mmioManager.writeMemory(MMIO::CONSOLE_BASE + MMIO::Console::DATA_REG, 'o');
    mmioManager.writeMemory(MMIO::CONSOLE_BASE + MMIO::Console::DATA_REG, '\n');
    
    // 读取控制台状态
    uint32_t status = mmioManager.readMemory(MMIO::CONSOLE_BASE + MMIO::Console::STATUS_REG);
    cout << "控制台状态寄存器: 0x" << hex << status << endl;
    
    // 演示文件系统外设
    cout << "\n--- 文件系统外设演示 ---" << endl;
    
    // 打开文件命令
    mmioManager.writeMemory(MMIO::FILE_SYSTEM_BASE + MMIO::FileSystem::COMMAND_REG, 
                           MMIO::FileSystem::CMD_OPEN);
    
    // 读取文件系统状态
    status = mmioManager.readMemory(MMIO::FILE_SYSTEM_BASE + MMIO::FileSystem::STATUS_REG);
    cout << "文件系统状态寄存器: 0x" << hex << status << endl;
    
    // 演示定时器外设
    cout << "\n--- 定时器外设演示 ---" << endl;
    
    // 启用定时器
    mmioManager.writeMemory(MMIO::TIMER_BASE + MMIO::Timer::CONTROL_REG, 
                           MMIO::Timer::CTRL_ENABLE);
    
    // 设置比较值
    mmioManager.writeMemory(MMIO::TIMER_BASE + MMIO::Timer::COMPARE_REG, 1000);
    
    // 读取定时器计数器
    uint32_t counter = mmioManager.readMemory(MMIO::TIMER_BASE + MMIO::Timer::COUNTER_REG);
    cout << "定时器计数器: " << dec << counter << endl;
    
    // 演示系统外设
    cout << "\n--- 系统外设演示 ---" << endl;
    
    // 读取版本信息
    uint32_t version = mmioManager.readMemory(MMIO::SYSTEM_BASE + MMIO::System::VERSION_REG);
    cout << "系统版本: 0x" << hex << version << endl;
    
    // 读取系统状态
    status = mmioManager.readMemory(MMIO::SYSTEM_BASE + MMIO::System::STATUS_REG);
    cout << "系统状态: 0x" << hex << status << endl;
    
    // 转储设备寄存器
    cout << "\n--- 设备寄存器转储 ---" << endl;
    mmioManager.dumpDeviceRegisters();
    
    // 清理
    mmioManager.shutdown();
    cout << "\nMMIO演示完成" << endl;
}

// 演示通过虚拟机执行器访问MMIO
void demonstrateVMWithMMIO() {
    cout << "\n=== 虚拟机MMIO集成演示 ===" << endl;
    
    // 创建一个简单的虚拟机程序
    VMProgram* program = new VMProgram();
    
    // 添加一个测试函数
    VMFunction* testFunc = program->addFunction("test_mmio");
    testFunc->paramCount = 0;
    testFunc->localVarCount = 2;
    
    // 添加一些测试指令
    testFunc->addInstruction(VMInstruction(VMInstructionType::PUSH, 0x48)); // 'H'
    testFunc->addInstruction(VMInstruction(VMInstructionType::PUSH, 0x65)); // 'e'
    testFunc->addInstruction(VMInstruction(VMInstructionType::PUSH, 0x6C)); // 'l'
    testFunc->addInstruction(VMInstruction(VMInstructionType::PUSH, 0x6C)); // 'l'
    testFunc->addInstruction(VMInstruction(VMInstructionType::PUSH, 0x6F)); // 'o'
    testFunc->addInstruction(VMInstruction(VMInstructionType::RET));
    
    // 创建执行器
    VMExecutor executor(program);
    
    // 执行函数
    bool success = executor.executeFunction("test_mmio");
    cout << "虚拟机执行结果: " << (success ? "成功" << endl : "失败" << endl);
    
    // 演示MMIO访问
    cout << "通过虚拟机访问MMIO:" << endl;
    
    // 写入控制台
    executor.writeMMIO(MMIO::CONSOLE_BASE + MMIO::Console::DATA_REG, 'V');
    executor.writeMMIO(MMIO::CONSOLE_BASE + MMIO::Console::DATA_REG, 'M');
    executor.writeMMIO(MMIO::CONSOLE_BASE + MMIO::Console::DATA_REG, '\n');
    
    // 读取状态
    uint32_t status = executor.readMMIO(MMIO::CONSOLE_BASE + MMIO::Console::STATUS_REG);
    cout << "控制台状态: 0x" << hex << status << endl;
    
    delete program;
}

int main(int argc, char* argv[]) {
    cout << "虚拟机模拟器 - MMIO外设支持演示" << endl;
    cout << "================================" << endl;
    
    if (argc > 1 && string(argv[1]) == "--mmio-demo") {
        // 运行MMIO演示
        demonstrateMMIO();
        demonstrateVMWithMMIO();
    } else {
        // 正常的虚拟机执行
        if (argc < 2) {
            cout << "用法: " << argv[0] << " <binary_file> [--mmio-demo]" << endl;
            cout << "  <binary_file> - 要执行的虚拟机二进制文件" << endl;
            cout << "  --mmio-demo   - 运行MMIO外设演示" << endl;
            return 1;
        }
        
        string filename = argv[1];
        Emulator emulator;
        
        if (emulator.loadProgram(filename)) {
            cout << "程序加载成功，开始执行..." << endl;
            emulator.execute();
        } else {
            cerr << "程序加载失败" << endl;
            return 1;
        }
    }
    
    return 0;
}
