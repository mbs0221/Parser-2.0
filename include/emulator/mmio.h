#ifndef MMIO_H
#define MMIO_H

#include <cstdint>
#include <map>
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>

// MMIO地址空间定义
namespace MMIO {
    // 基础地址定义
    const uint32_t BASE_ADDRESS = 0x10000000;  // 256MB开始
    
    // 外设地址分配
    const uint32_t CONSOLE_BASE = BASE_ADDRESS + 0x0000;      // 控制台
    const uint32_t CONSOLE_SIZE = 0x1000;                     // 4KB
    
    const uint32_t FILE_SYSTEM_BASE = BASE_ADDRESS + 0x1000;  // 文件系统
    const uint32_t FILE_SYSTEM_SIZE = 0x2000;                 // 8KB
    
    const uint32_t NETWORK_BASE = BASE_ADDRESS + 0x3000;      // 网络
    const uint32_t NETWORK_SIZE = 0x1000;                     // 4KB
    
    const uint32_t TIMER_BASE = BASE_ADDRESS + 0x4000;        // 定时器
    const uint32_t TIMER_SIZE = 0x1000;                       // 4KB
    
    const uint32_t SYSTEM_BASE = BASE_ADDRESS + 0x5000;       // 系统控制
    const uint32_t SYSTEM_SIZE = 0x1000;                      // 4KB
    
    // 寄存器偏移定义
    namespace Console {
        const uint32_t DATA_REG = 0x00;       // 数据寄存器 (R/W)
        const uint32_t STATUS_REG = 0x04;     // 状态寄存器 (R)
        const uint32_t CONTROL_REG = 0x08;    // 控制寄存器 (R/W)
        const uint32_t BUFFER_REG = 0x0C;     // 缓冲区寄存器 (R/W)
        
        // 状态寄存器位定义
        const uint32_t STATUS_READY = 0x01;   // 数据就绪
        const uint32_t STATUS_BUSY = 0x02;    // 忙状态
        const uint32_t STATUS_ERROR = 0x04;   // 错误状态
        
        // 控制寄存器位定义
        const uint32_t CTRL_ENABLE = 0x01;    // 使能
        const uint32_t CTRL_ECHO = 0x02;      // 回显模式
        const uint32_t CTRL_INTERRUPT = 0x04; // 中断使能
    }
    
    namespace FileSystem {
        const uint32_t COMMAND_REG = 0x00;    // 命令寄存器 (W)
        const uint32_t STATUS_REG = 0x04;     // 状态寄存器 (R)
        const uint32_t DATA_REG = 0x08;       // 数据寄存器 (R/W)
        const uint32_t ADDRESS_REG = 0x0C;    // 地址寄存器 (R/W)
        const uint32_t SIZE_REG = 0x10;       // 大小寄存器 (R/W)
        const uint32_t RESULT_REG = 0x14;     // 结果寄存器 (R)
        
        // 命令定义
        const uint32_t CMD_OPEN = 0x01;       // 打开文件
        const uint32_t CMD_CLOSE = 0x02;      // 关闭文件
        const uint32_t CMD_READ = 0x03;       // 读取文件
        const uint32_t CMD_WRITE = 0x04;      // 写入文件
        const uint32_t CMD_SEEK = 0x05;       // 文件定位
        const uint32_t CMD_DELETE = 0x06;     // 删除文件
        const uint32_t CMD_LIST = 0x07;       // 列出文件
        
        // 状态位定义
        const uint32_t STATUS_BUSY = 0x01;    // 忙状态
        const uint32_t STATUS_DONE = 0x02;    // 操作完成
        const uint32_t STATUS_ERROR = 0x04;   // 错误状态
    }
    
    namespace Network {
        const uint32_t COMMAND_REG = 0x00;    // 命令寄存器 (W)
        const uint32_t STATUS_REG = 0x04;     // 状态寄存器 (R)
        const uint32_t DATA_REG = 0x08;       // 数据寄存器 (R/W)
        const uint32_t ADDRESS_REG = 0x0C;    // 地址寄存器 (R/W)
        const uint32_t PORT_REG = 0x10;       // 端口寄存器 (R/W)
        const uint32_t RESULT_REG = 0x14;     // 结果寄存器 (R)
        
        // 命令定义
        const uint32_t CMD_CONNECT = 0x01;    // 连接
        const uint32_t CMD_DISCONNECT = 0x02; // 断开连接
        const uint32_t CMD_SEND = 0x03;       // 发送数据
        const uint32_t CMD_RECEIVE = 0x04;    // 接收数据
        const uint32_t CMD_LISTEN = 0x05;     // 监听端口
        const uint32_t CMD_ACCEPT = 0x06;     // 接受连接
    }
    
    namespace Timer {
        const uint32_t COUNTER_REG = 0x00;    // 计数器寄存器 (R)
        const uint32_t CONTROL_REG = 0x04;    // 控制寄存器 (R/W)
        const uint32_t COMPARE_REG = 0x08;    // 比较寄存器 (R/W)
        const uint32_t INTERRUPT_REG = 0x0C;  // 中断寄存器 (R/W)
        
        // 控制寄存器位定义
        const uint32_t CTRL_ENABLE = 0x01;    // 使能定时器
        const uint32_t CTRL_RESET = 0x02;     // 重置定时器
        const uint32_t CTRL_INTERRUPT = 0x04; // 中断使能
    }
    
    namespace System {
        const uint32_t CONTROL_REG = 0x00;    // 系统控制寄存器 (R/W)
        const uint32_t STATUS_REG = 0x04;     // 系统状态寄存器 (R)
        const uint32_t VERSION_REG = 0x08;    // 版本寄存器 (R)
        const uint32_t RESET_REG = 0x0C;      // 重置寄存器 (W)
        
        // 控制寄存器位定义
        const uint32_t CTRL_ENABLE_MMIO = 0x01;   // 使能MMIO
        const uint32_t CTRL_ENABLE_INT = 0x02;    // 使能中断
        const uint32_t CTRL_DEBUG_MODE = 0x04;    // 调试模式
        
        // 重置寄存器值
        const uint32_t RESET_SOFT = 0x01;     // 软重置
        const uint32_t RESET_HARD = 0x02;     // 硬重置
    }
}

// MMIO设备基类
class MMIODevice {
protected:
    uint32_t baseAddress;
    uint32_t size;
    std::string name;
    
public:
    MMIODevice(uint32_t base, uint32_t deviceSize, const std::string& deviceName)
        : baseAddress(base), size(deviceSize), name(deviceName) {}
    
    virtual ~MMIODevice() = default;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual uint32_t read(uint32_t offset) = 0;
    virtual void write(uint32_t offset, uint32_t value) = 0;
    
    uint32_t getBaseAddress() const { return baseAddress; }
    uint32_t getSize() const { return size; }
    const std::string& getName() const { return name; }
    
    bool isInRange(uint32_t address) const {
        return address >= baseAddress && address < baseAddress + size;
    }
    
    uint32_t getOffset(uint32_t address) const {
        return address - baseAddress;
    }
};

// 控制台MMIO设备
class ConsoleMMIODevice : public MMIODevice {
private:
    std::vector<uint8_t> buffer;
    uint32_t status;
    uint32_t control;
    size_t bufferIndex;
    
public:
    ConsoleMMIODevice() : MMIODevice(MMIO::CONSOLE_BASE, MMIO::CONSOLE_SIZE, "console") {
        buffer.resize(256);
        status = MMIO::Console::STATUS_READY;
        control = MMIO::Console::CTRL_ENABLE;
        bufferIndex = 0;
    }
    
    bool initialize() override {
        status = MMIO::Console::STATUS_READY;
        control = MMIO::Console::CTRL_ENABLE;
        bufferIndex = 0;
        return true;
    }
    
    void shutdown() override {
        status = 0;
        control = 0;
    }
    
    uint32_t read(uint32_t offset) override;
    void write(uint32_t offset, uint32_t value) override;
    
    // 控制台操作
    void putChar(char c);
    char getChar();
    bool hasData() const;
    void clearBuffer();
};

// 文件系统MMIO设备
class FileSystemMMIODevice : public MMIODevice {
private:
    std::map<int, std::fstream> openFiles;
    int nextFileHandle;
    uint32_t status;
    uint32_t command;
    uint32_t data;
    uint32_t address;
    uint32_t size;
    uint32_t result;
    std::string currentCommand;
    
public:
    FileSystemMMIODevice() : MMIODevice(MMIO::FILE_SYSTEM_BASE, MMIO::FILE_SYSTEM_SIZE, "filesystem") {
        nextFileHandle = 1;
        status = 0;
        command = 0;
        data = 0;
        address = 0;
        size = 0;
        result = 0;
    }
    
    bool initialize() override {
        status = 0;
        command = 0;
        return true;
    }
    
    void shutdown() override {
        openFiles.clear();
        status = 0;
    }
    
    uint32_t read(uint32_t offset) override;
    void write(uint32_t offset, uint32_t value) override;
    
    // 文件系统操作
    void executeCommand();
    int openFile(const std::string& filename, const std::string& mode);
    bool closeFile(int handle);
    std::string readFile(int handle, size_t readSize);
    bool writeFile(int handle, const std::string& data);
};

// 网络MMIO设备
class NetworkMMIODevice : public MMIODevice {
private:
    std::map<int, std::string> connections;
    int nextConnectionId;
    uint32_t status;
    uint32_t command;
    uint32_t data;
    uint32_t address;
    uint32_t port;
    uint32_t result;
    
public:
    NetworkMMIODevice() : MMIODevice(MMIO::NETWORK_BASE, MMIO::NETWORK_SIZE, "network") {
        nextConnectionId = 1;
        status = 0;
        command = 0;
        data = 0;
        address = 0;
        port = 0;
        result = 0;
    }
    
    bool initialize() override {
        status = 0;
        command = 0;
        return true;
    }
    
    void shutdown() override {
        connections.clear();
        status = 0;
    }
    
    uint32_t read(uint32_t offset) override;
    void write(uint32_t offset, uint32_t value) override;
    
    // 网络操作
    void executeCommand();
    int connect(const std::string& host, int port);
    bool disconnect(int connectionId);
    std::string receive(int connectionId, size_t size);
    bool send(int connectionId, const std::string& data);
};

// 定时器MMIO设备
class TimerMMIODevice : public MMIODevice {
private:
    uint32_t counter;
    uint32_t control;
    uint32_t compare;
    uint32_t interrupt;
    std::chrono::steady_clock::time_point startTime;
    
public:
    TimerMMIODevice() : MMIODevice(MMIO::TIMER_BASE, MMIO::TIMER_SIZE, "timer") {
        counter = 0;
        control = 0;
        compare = 0;
        interrupt = 0;
        startTime = std::chrono::steady_clock::now();
    }
    
    bool initialize() override {
        counter = 0;
        control = 0;
        compare = 0;
        interrupt = 0;
        startTime = std::chrono::steady_clock::now();
        return true;
    }
    
    void shutdown() override {
        control = 0;
        interrupt = 0;
    }
    
    uint32_t read(uint32_t offset) override;
    void write(uint32_t offset, uint32_t value) override;
    
    // 定时器操作
    void update();
    void reset();
    bool isInterruptPending() const;
};

// 系统MMIO设备
class SystemMMIODevice : public MMIODevice {
private:
    uint32_t control;
    uint32_t status;
    uint32_t version;
    
public:
    SystemMMIODevice() : MMIODevice(MMIO::SYSTEM_BASE, MMIO::SYSTEM_SIZE, "system") {
        control = MMIO::System::CTRL_ENABLE_MMIO;
        status = 0;
        version = 0x0100; // 版本1.0
    }
    
    bool initialize() override {
        control = MMIO::System::CTRL_ENABLE_MMIO;
        status = 0;
        return true;
    }
    
    void shutdown() override {
        control = 0;
    }
    
    uint32_t read(uint32_t offset) override;
    void write(uint32_t offset, uint32_t value) override;
};

// MMIO管理器
class MMIOManager {
private:
    std::map<uint32_t, std::unique_ptr<MMIODevice>> devices;
    std::vector<uint8_t> memory;
    bool enabled;
    
public:
    MMIOManager();
    ~MMIOManager();
    
    bool initialize();
    void shutdown();
    
    // 设备管理
    bool registerDevice(std::unique_ptr<MMIODevice> device);
    bool unregisterDevice(uint32_t baseAddress);
    MMIODevice* getDevice(uint32_t address);
    
    // 内存访问
    uint32_t readMemory(uint32_t address);
    void writeMemory(uint32_t address, uint32_t value);
    uint8_t readByte(uint32_t address);
    void writeByte(uint32_t address, uint8_t value);
    uint16_t readWord(uint32_t address);
    void writeWord(uint32_t address, uint16_t value);
    
    // 状态查询
    bool isEnabled() const { return enabled; }
    void setEnabled(bool enable) { enabled = enable; }
    std::vector<std::string> getDeviceNames() const;
    
    // 调试功能
    void dumpMemory(uint32_t start, uint32_t end);
    void dumpDeviceRegisters();
};

#endif // MMIO_H
