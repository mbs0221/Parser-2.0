#include "emulator/mmio.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <cstring>
#include <memory>

using namespace std;

// ==================== 控制台MMIO设备实现 ====================

uint32_t ConsoleMMIODevice::read(uint32_t offset) {
    switch (offset) {
        case MMIO::Console::DATA_REG:
            if (hasData()) {
                return getChar();
            }
            return 0;
            
        case MMIO::Console::STATUS_REG:
            return status;
            
        case MMIO::Console::CONTROL_REG:
            return control;
            
        case MMIO::Console::BUFFER_REG:
            if (bufferIndex > 0) {
                return buffer[bufferIndex - 1];
            }
            return 0;
            
        default:
            return 0;
    }
}

void ConsoleMMIODevice::write(uint32_t offset, uint32_t value) {
    switch (offset) {
        case MMIO::Console::DATA_REG:
            putChar(static_cast<char>(value & 0xFF));
            break;
            
        case MMIO::Console::CONTROL_REG:
            control = value;
            break;
            
        case MMIO::Console::BUFFER_REG:
            if (bufferIndex < buffer.size()) {
                buffer[bufferIndex++] = static_cast<uint8_t>(value & 0xFF);
            }
            break;
            
        default:
            break;
    }
}

void ConsoleMMIODevice::putChar(char c) {
    if (control & MMIO::Console::CTRL_ENABLE) {
        std::cout << c;
        std::cout.flush();
        
        if (control & MMIO::Console::CTRL_ECHO) {
            // 回显模式，将字符也放入缓冲区
            if (bufferIndex < buffer.size()) {
                buffer[bufferIndex++] = static_cast<uint8_t>(c);
            }
        }
    }
}

char ConsoleMMIODevice::getChar() {
    if (control & MMIO::Console::CTRL_ENABLE) {
        if (bufferIndex > 0) {
            // 从缓冲区读取
            return static_cast<char>(buffer[--bufferIndex]);
        } else {
            // 从标准输入读取
            char c;
            std::cin.get(c);
            return c;
        }
    }
    return 0;
}

bool ConsoleMMIODevice::hasData() const {
    return bufferIndex > 0 || std::cin.peek() != EOF;
}

void ConsoleMMIODevice::clearBuffer() {
    bufferIndex = 0;
}

// ==================== 文件系统MMIO设备实现 ====================

uint32_t FileSystemMMIODevice::read(uint32_t offset) {
    switch (offset) {
        case MMIO::FileSystem::STATUS_REG:
            return status;
            
        case MMIO::FileSystem::DATA_REG:
            return data;
            
        case MMIO::FileSystem::ADDRESS_REG:
            return address;
            
        case MMIO::FileSystem::SIZE_REG:
            return size;
            
        case MMIO::FileSystem::RESULT_REG:
            return result;
            
        default:
            return 0;
    }
}

void FileSystemMMIODevice::write(uint32_t offset, uint32_t value) {
    switch (offset) {
        case MMIO::FileSystem::COMMAND_REG:
            command = value;
            executeCommand();
            break;
            
        case MMIO::FileSystem::DATA_REG:
            data = value;
            break;
            
        case MMIO::FileSystem::ADDRESS_REG:
            address = value;
            break;
            
        case MMIO::FileSystem::SIZE_REG:
            size = value;
            break;
            
        default:
            break;
    }
}

void FileSystemMMIODevice::executeCommand() {
    status = MMIO::FileSystem::STATUS_BUSY;
    
    switch (command) {
        case MMIO::FileSystem::CMD_OPEN: {
            // 从内存中读取文件名和模式
            std::string filename = currentCommand; // 简化处理
            std::string mode = "r";
            int handle = openFile(filename, mode);
            result = handle;
            break;
        }
        
        case MMIO::FileSystem::CMD_CLOSE: {
            int handle = static_cast<int>(data);
            bool success = closeFile(handle);
            result = success ? 0 : -1;
            break;
        }
        
        case MMIO::FileSystem::CMD_READ: {
            int handle = static_cast<int>(address);
            size_t readSize = static_cast<size_t>(size);
            std::string content = readFile(handle, readSize);
            // 将读取的内容写入内存（简化处理）
            data = content.length();
            result = content.length();
            break;
        }
        
        case MMIO::FileSystem::CMD_WRITE: {
            int handle = static_cast<int>(address);
            std::string content = currentCommand; // 简化处理
            bool success = writeFile(handle, content);
            result = success ? content.length() : -1;
            break;
        }
        
        default:
            result = -1;
            break;
    }
    
    status = MMIO::FileSystem::STATUS_DONE;
}

int FileSystemMMIODevice::openFile(const std::string& filename, const std::string& mode) {
    std::fstream file;
    if (mode == "r") {
        file.open(filename, std::ios::in | std::ios::binary);
    } else if (mode == "w") {
        file.open(filename, std::ios::out | std::ios::binary);
    } else if (mode == "a") {
        file.open(filename, std::ios::app | std::ios::binary);
    }
    
    if (file.is_open()) {
        int handle = nextFileHandle++;
        openFiles[handle] = std::move(file);
        return handle;
    }
    
    return -1;
}

bool FileSystemMMIODevice::closeFile(int handle) {
    auto it = openFiles.find(handle);
    if (it != openFiles.end()) {
        it->second.close();
        openFiles.erase(it);
        return true;
    }
    return false;
}

std::string FileSystemMMIODevice::readFile(int handle, size_t readSize) {
    auto it = openFiles.find(handle);
    if (it != openFiles.end()) {
        std::string content;
        content.resize(readSize);
        it->second.read(&content[0], readSize);
        return content;
    }
    return "";
}

bool FileSystemMMIODevice::writeFile(int handle, const std::string& data) {
    auto it = openFiles.find(handle);
    if (it != openFiles.end()) {
        it->second.write(data.c_str(), data.length());
        return true;
    }
    return false;
}

// ==================== 网络MMIO设备实现 ====================

uint32_t NetworkMMIODevice::read(uint32_t offset) {
    switch (offset) {
        case MMIO::Network::STATUS_REG:
            return status;
            
        case MMIO::Network::DATA_REG:
            return data;
            
        case MMIO::Network::ADDRESS_REG:
            return address;
            
        case MMIO::Network::PORT_REG:
            return port;
            
        case MMIO::Network::RESULT_REG:
            return result;
            
        default:
            return 0;
    }
}

void NetworkMMIODevice::write(uint32_t offset, uint32_t value) {
    switch (offset) {
        case MMIO::Network::COMMAND_REG:
            command = value;
            executeCommand();
            break;
            
        case MMIO::Network::DATA_REG:
            data = value;
            break;
            
        case MMIO::Network::ADDRESS_REG:
            address = value;
            break;
            
        case MMIO::Network::PORT_REG:
            port = value;
            break;
            
        default:
            break;
    }
}

void NetworkMMIODevice::executeCommand() {
    status = MMIO::Network::STATUS_BUSY;
    
    switch (command) {
        case MMIO::Network::CMD_CONNECT: {
            // 简化实现：模拟连接成功
            int connectionId = nextConnectionId++;
            connections[connectionId] = "connected";
            result = connectionId;
            break;
        }
        
        case MMIO::Network::CMD_DISCONNECT: {
            int connectionId = static_cast<int>(data);
            bool success = disconnect(connectionId);
            result = success ? 0 : -1;
            break;
        }
        
        case MMIO::Network::CMD_SEND: {
            int connectionId = static_cast<int>(address);
            std::string message = "data"; // 简化处理
            bool success = send(connectionId, message);
            result = success ? message.length() : -1;
            break;
        }
        
        case MMIO::Network::CMD_RECEIVE: {
            int connectionId = static_cast<int>(address);
            size_t size = static_cast<size_t>(data);
            std::string received = receive(connectionId, size);
            result = received.length();
            break;
        }
        
        default:
            result = -1;
            break;
    }
    
    status = MMIO::Network::STATUS_DONE;
}

int NetworkMMIODevice::connect(const std::string& host, int port) {
    // 简化实现：总是返回成功
    int connectionId = nextConnectionId++;
    connections[connectionId] = host + ":" + std::to_string(port);
    return connectionId;
}

bool NetworkMMIODevice::disconnect(int connectionId) {
    auto it = connections.find(connectionId);
    if (it != connections.end()) {
        connections.erase(it);
        return true;
    }
    return false;
}

std::string NetworkMMIODevice::receive(int connectionId, size_t size) {
    auto it = connections.find(connectionId);
    if (it != connections.end()) {
        // 简化实现：返回模拟数据
        return "received_data";
    }
    return "";
}

bool NetworkMMIODevice::send(int connectionId, const std::string& data) {
    auto it = connections.find(connectionId);
    if (it != connections.end()) {
        // 简化实现：总是成功
        return true;
    }
    return false;
}

// ==================== 定时器MMIO设备实现 ====================

uint32_t TimerMMIODevice::read(uint32_t offset) {
    update();
    
    switch (offset) {
        case MMIO::Timer::COUNTER_REG:
            return counter;
            
        case MMIO::Timer::CONTROL_REG:
            return control;
            
        case MMIO::Timer::COMPARE_REG:
            return compare;
            
        case MMIO::Timer::INTERRUPT_REG:
            return interrupt;
            
        default:
            return 0;
    }
}

void TimerMMIODevice::write(uint32_t offset, uint32_t value) {
    switch (offset) {
        case MMIO::Timer::CONTROL_REG:
            control = value;
            if (value & MMIO::Timer::CTRL_RESET) {
                reset();
            }
            break;
            
        case MMIO::Timer::COMPARE_REG:
            compare = value;
            break;
            
        case MMIO::Timer::INTERRUPT_REG:
            interrupt = value;
            break;
            
        default:
            break;
    }
}

void TimerMMIODevice::update() {
    if (control & MMIO::Timer::CTRL_ENABLE) {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime);
        counter = static_cast<uint32_t>(duration.count());
        
        // 检查比较器
        if (compare > 0 && counter >= compare) {
            if (control & MMIO::Timer::CTRL_INTERRUPT) {
                interrupt = 1;
            }
            if (control & MMIO::Timer::CTRL_RESET) {
                reset();
            }
        }
    }
}

void TimerMMIODevice::reset() {
    counter = 0;
    startTime = std::chrono::steady_clock::now();
}

bool TimerMMIODevice::isInterruptPending() const {
    return interrupt != 0;
}

// ==================== 系统MMIO设备实现 ====================

uint32_t SystemMMIODevice::read(uint32_t offset) {
    switch (offset) {
        case MMIO::System::CONTROL_REG:
            return control;
            
        case MMIO::System::STATUS_REG:
            return status;
            
        case MMIO::System::VERSION_REG:
            return version;
            
        default:
            return 0;
    }
}

void SystemMMIODevice::write(uint32_t offset, uint32_t value) {
    switch (offset) {
        case MMIO::System::CONTROL_REG:
            control = value;
            break;
            
        case MMIO::System::RESET_REG:
            if (value == MMIO::System::RESET_SOFT) {
                // 软重置
                status = 0;
            } else if (value == MMIO::System::RESET_HARD) {
                // 硬重置
                control = MMIO::System::CTRL_ENABLE_MMIO;
                status = 0;
            }
            break;
            
        default:
            break;
    }
}

// ==================== MMIO管理器实现 ====================

MMIOManager::MMIOManager() : enabled(false) {
    // 分配MMIO内存空间
    memory.resize(0x10000); // 64KB MMIO空间
}

MMIOManager::~MMIOManager() {
    shutdown();
}

bool MMIOManager::initialize() {
    enabled = true;
    
    // 注册默认设备
    registerDevice(std::make_unique<ConsoleMMIODevice>());
    registerDevice(std::make_unique<FileSystemMMIODevice>());
    registerDevice(std::make_unique<NetworkMMIODevice>());
    registerDevice(std::make_unique<TimerMMIODevice>());
    registerDevice(std::make_unique<SystemMMIODevice>());
    
    // 初始化所有设备
    for (auto& device : devices) {
        if (!device.second->initialize()) {
            std::cerr << "Failed to initialize device: " << device.second->getName() << std::endl;
            return false;
        }
    }
    
    std::cout << "MMIO Manager initialized with " << devices.size() << " devices" << std::endl;
    return true;
}

void MMIOManager::shutdown() {
    for (auto& device : devices) {
        device.second->shutdown();
    }
    devices.clear();
    enabled = false;
}

bool MMIOManager::registerDevice(std::unique_ptr<MMIODevice> device) {
    if (!device) return false;
    
    uint32_t baseAddr = device->getBaseAddress();
    if (devices.find(baseAddr) != devices.end()) {
        std::cerr << "Device already registered at address 0x" << std::hex << baseAddr << std::endl;
        return false;
    }
    
    devices[baseAddr] = std::move(device);
    std::cout << "Registered device: " << devices[baseAddr]->getName() 
              << " at 0x" << std::hex << baseAddr << std::endl;
    return true;
}

bool MMIOManager::unregisterDevice(uint32_t baseAddress) {
    auto it = devices.find(baseAddress);
    if (it != devices.end()) {
        it->second->shutdown();
        devices.erase(it);
        return true;
    }
    return false;
}

MMIODevice* MMIOManager::getDevice(uint32_t address) {
    for (auto& device : devices) {
        if (device.second->isInRange(address)) {
            return device.second.get();
        }
    }
    return nullptr;
}

uint32_t MMIOManager::readMemory(uint32_t address) {
    if (!enabled) return 0;
    
    MMIODevice* device = getDevice(address);
    if (device) {
        uint32_t offset = device->getOffset(address);
        return device->read(offset);
    }
    
    // 如果不是MMIO地址，从普通内存读取
    if (address < memory.size() - 3) {
        return (memory[address] << 24) | (memory[address + 1] << 16) | 
               (memory[address + 2] << 8) | memory[address + 3];
    }
    
    return 0;
}

void MMIOManager::writeMemory(uint32_t address, uint32_t value) {
    if (!enabled) return;
    
    MMIODevice* device = getDevice(address);
    if (device) {
        uint32_t offset = device->getOffset(address);
        device->write(offset, value);
        return;
    }
    
    // 如果不是MMIO地址，写入普通内存
    if (address < memory.size() - 3) {
        memory[address] = (value >> 24) & 0xFF;
        memory[address + 1] = (value >> 16) & 0xFF;
        memory[address + 2] = (value >> 8) & 0xFF;
        memory[address + 3] = value & 0xFF;
    }
}

uint8_t MMIOManager::readByte(uint32_t address) {
    if (!enabled) return 0;
    
    MMIODevice* device = getDevice(address);
    if (device) {
        uint32_t offset = device->getOffset(address);
        return static_cast<uint8_t>(device->read(offset) & 0xFF);
    }
    
    if (address < memory.size()) {
        return memory[address];
    }
    
    return 0;
}

void MMIOManager::writeByte(uint32_t address, uint8_t value) {
    if (!enabled) return;
    
    MMIODevice* device = getDevice(address);
    if (device) {
        uint32_t offset = device->getOffset(address);
        device->write(offset, value);
        return;
    }
    
    if (address < memory.size()) {
        memory[address] = value;
    }
}

uint16_t MMIOManager::readWord(uint32_t address) {
    if (!enabled) return 0;
    
    MMIODevice* device = getDevice(address);
    if (device) {
        uint32_t offset = device->getOffset(address);
        return static_cast<uint16_t>(device->read(offset) & 0xFFFF);
    }
    
    if (address < memory.size() - 1) {
        return (memory[address] << 8) | memory[address + 1];
    }
    
    return 0;
}

void MMIOManager::writeWord(uint32_t address, uint16_t value) {
    if (!enabled) return;
    
    MMIODevice* device = getDevice(address);
    if (device) {
        uint32_t offset = device->getOffset(address);
        device->write(offset, value);
        return;
    }
    
    if (address < memory.size() - 1) {
        memory[address] = (value >> 8) & 0xFF;
        memory[address + 1] = value & 0xFF;
    }
}

std::vector<std::string> MMIOManager::getDeviceNames() const {
    std::vector<std::string> names;
    for (const auto& device : devices) {
        names.push_back(device.second->getName());
    }
    return names;
}

void MMIOManager::dumpMemory(uint32_t start, uint32_t end) {
    std::cout << "Memory dump from 0x" << std::hex << start << " to 0x" << end << std::endl;
    for (uint32_t addr = start; addr < end && addr < memory.size(); addr += 16) {
        std::cout << std::hex << std::setw(8) << std::setfill('0') << addr << ": ";
        for (int i = 0; i < 16 && addr + i < memory.size(); ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') 
                      << static_cast<int>(memory[addr + i]) << " ";
        }
        std::cout << std::endl;
    }
}

void MMIOManager::dumpDeviceRegisters() {
    std::cout << "Device registers dump:" << std::endl;
    for (const auto& device : devices) {
        std::cout << "Device: " << device.second->getName() 
                  << " (base: 0x" << std::hex << device.first << ")" << std::endl;
        
        // 读取设备的前几个寄存器
        for (uint32_t offset = 0; offset < 16; offset += 4) {
            uint32_t value = device.second->read(offset);
            std::cout << "  [0x" << std::hex << offset << "] = 0x" 
                      << std::hex << value << std::endl;
        }
    }
}
