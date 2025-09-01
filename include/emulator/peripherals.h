#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

// 外设基类
class Peripheral {
protected:
    std::string name;
    bool enabled;
    std::mutex mutex;
    
public:
    Peripheral(const std::string& n) : name(n), enabled(false) {}
    virtual ~Peripheral() = default;
    
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isReady() const { return enabled; }
    
    const std::string& getName() const { return name; }
    void setEnabled(bool enable) { enabled = enable; }
};

// 控制台外设（标准输入输出）
class ConsolePeripheral : public Peripheral {
private:
    std::ostream* output;
    std::istream* input;
    std::stringstream buffer;
    bool echoMode;
    
public:
    ConsolePeripheral() : Peripheral("console"), output(&std::cout), input(&std::cin), echoMode(true) {}
    
    bool initialize() override {
        enabled = true;
        return true;
    }
    
    void shutdown() override {
        enabled = false;
    }
    
    // 输出功能
    void write(const std::string& text);
    void writeLine(const std::string& text);
    void writeChar(char c);
    
    // 输入功能
    std::string readLine();
    char readChar();
    int readInt();
    double readDouble();
    
    // 缓冲功能
    void setEchoMode(bool echo) { echoMode = echo; }
    bool getEchoMode() const { return echoMode; }
    
    void clearBuffer() { buffer.str(""); buffer.clear(); }
    std::string getBuffer() const { return buffer.str(); }
};

// 文件系统外设
class FileSystemPeripheral : public Peripheral {
private:
    std::map<int, std::fstream> openFiles;
    int nextFileHandle;
    std::string currentDirectory;
    
public:
    FileSystemPeripheral() : Peripheral("filesystem"), nextFileHandle(1), currentDirectory(".") {}
    
    bool initialize() override {
        enabled = true;
        return true;
    }
    
    void shutdown() override {
        // 关闭所有打开的文件
        openFiles.clear();
        enabled = false;
    }
    
    // 文件操作
    int openFile(const std::string& filename, const std::string& mode);
    bool closeFile(int handle);
    bool isFileOpen(int handle) const;
    
    // 读写操作
    std::string readFile(int handle, size_t size);
    bool writeFile(int handle, const std::string& data);
    bool seekFile(int handle, long offset, int whence);
    long tellFile(int handle);
    
    // 文件系统操作
    bool fileExists(const std::string& filename) const;
    bool deleteFile(const std::string& filename);
    std::vector<std::string> listFiles(const std::string& directory = ".");
    std::string getCurrentDirectory() const { return currentDirectory; }
    bool changeDirectory(const std::string& dir);
    
private:
    std::fstream* getFileStream(int handle);
};

// 网络外设（模拟）
class NetworkPeripheral : public Peripheral {
private:
    std::map<int, std::string> connections;
    int nextConnectionId;
    bool serverMode;
    int port;
    
public:
    NetworkPeripheral() : Peripheral("network"), nextConnectionId(1), serverMode(false), port(8080) {}
    
    bool initialize() override {
        enabled = true;
        return true;
    }
    
    void shutdown() override {
        connections.clear();
        enabled = false;
    }
    
    // 网络操作
    int connect(const std::string& host, int port);
    bool disconnect(int connectionId);
    std::string receive(int connectionId, size_t size);
    bool send(int connectionId, const std::string& data);
    
    // 服务器操作
    bool startServer(int port);
    bool stopServer();
    int acceptConnection();
    
    bool isConnected(int connectionId) const;
    std::vector<int> getConnections() const;
};

// 定时器外设
class TimerPeripheral : public Peripheral {
private:
    std::map<int, std::function<void()>> timers;
    std::map<int, bool> timerActive;
    int nextTimerId;
    std::thread timerThread;
    bool running;
    std::mutex timerMutex;
    std::condition_variable timerCV;
    
public:
    TimerPeripheral() : Peripheral("timer"), nextTimerId(1), running(false) {}
    
    bool initialize() override {
        enabled = true;
        running = true;
        timerThread = std::thread(&TimerPeripheral::timerLoop, this);
        return true;
    }
    
    void shutdown() override {
        running = false;
        timerCV.notify_all();
        if (timerThread.joinable()) {
            timerThread.join();
        }
        enabled = false;
    }
    
    // 定时器操作
    int createTimer(int intervalMs, std::function<void()> callback);
    bool destroyTimer(int timerId);
    bool startTimer(int timerId);
    bool stopTimer(int timerId);
    
    // 延时功能
    void sleep(int milliseconds);
    long getCurrentTime() const;
    
private:
    void timerLoop();
};

// 外设管理器
class PeripheralManager {
private:
    std::map<std::string, std::unique_ptr<Peripheral>> peripherals;
    std::mutex managerMutex;
    
public:
    PeripheralManager();
    ~PeripheralManager();
    
    // 外设管理
    bool registerPeripheral(const std::string& name, std::unique_ptr<Peripheral> peripheral);
    bool unregisterPeripheral(const std::string& name);
    Peripheral* getPeripheral(const std::string& name);
    
    // 初始化所有外设
    bool initializeAll();
    void shutdownAll();
    
    // 便捷访问
    ConsolePeripheral* getConsole();
    FileSystemPeripheral* getFileSystem();
    NetworkPeripheral* getNetwork();
    TimerPeripheral* getTimer();
    
    // 外设列表
    std::vector<std::string> getPeripheralNames() const;
    bool hasPeripheral(const std::string& name) const;
};

// 外设指令扩展
enum class PeripheralInstructionType {
    // 控制台指令
    CONSOLE_WRITE,
    CONSOLE_WRITELN,
    CONSOLE_READ,
    CONSOLE_READLN,
    
    // 文件系统指令
    FILE_OPEN,
    FILE_CLOSE,
    FILE_READ,
    FILE_WRITE,
    FILE_SEEK,
    FILE_TELL,
    
    // 网络指令
    NET_CONNECT,
    NET_DISCONNECT,
    NET_SEND,
    NET_RECEIVE,
    
    // 定时器指令
    TIMER_CREATE,
    TIMER_DESTROY,
    TIMER_SLEEP,
    
    // 系统指令
    SYS_EXIT,
    SYS_GET_TIME
};

// 外设指令
struct PeripheralInstruction {
    PeripheralInstructionType type;
    std::vector<std::string> parameters;
    
    PeripheralInstruction(PeripheralInstructionType t, const std::vector<std::string>& params = {})
        : type(t), parameters(params) {}
};

#endif // PERIPHERALS_H
