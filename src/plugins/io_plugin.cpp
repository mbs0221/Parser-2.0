#include "interpreter/builtin_plugin.h"
#include "interpreter/value.h"
#include "parser/function.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <map>
#include <cstring>

using namespace std;

// ==================== 文件句柄管理系统 ====================
// 全局文件句柄映射，用于管理打开的文件
static map<int, FILE*> fileHandles;
static int nextFileHandle = 1;

// 获取下一个可用的文件句柄
int getNextFileHandle() {
    return nextFileHandle++;
}

// 检查文件句柄是否有效
bool isValidFileHandle(int handle) {
    return fileHandles.find(handle) != fileHandles.end() && fileHandles[handle] != nullptr;
}

// ==================== IO函数实现 ====================

Value* builtin_open(vector<Value*>& args) {
    if (args.size() < 1 || !args[0]) {
        return nullptr;
    }
    
    // 获取文件名
    Value* fileNameVal = args[0];
    if (!fileNameVal) {
        return nullptr;
    }
    
    String* fileNameStr = dynamic_cast<String*>(fileNameVal);
    if (!fileNameStr) {
        return nullptr;
    }
    
    string fileName = fileNameStr->getValue();
    
    // 确定打开模式
    string mode = "r";  // 默认只读模式
    if (args.size() >= 2 && args[1]) {
        Value* modeVal = args[1];
        if (String* modeStr = dynamic_cast<String*>(modeVal)) {
            mode = modeStr->getValue();
        }
    }
    
    // 根据模式确定fopen的模式字符串
    const char* fopenMode = "r";  // 默认只读模式
    if (mode == "r" || mode == "read") {
        fopenMode = "r";
    } else if (mode == "w" || mode == "write") {
        fopenMode = "w";
    } else if (mode == "a" || mode == "append") {
        fopenMode = "a";
    } else if (mode == "r+" || mode == "readwrite") {
        fopenMode = "r+";
    }
    
    // 使用fopen打开文件
    FILE* file = fopen(fileName.c_str(), fopenMode);
    
    // 检查文件是否成功打开
    if (!file) {
        return nullptr;
    }
    
    // 分配文件句柄
    int fileHandle = getNextFileHandle();
    fileHandles[fileHandle] = file;
    
    return new Integer(fileHandle);
}

Value* builtin_close(vector<Value*>& args) {
    if (args.size() < 1 || !args[0]) {
        return nullptr;
    }
    
    // 获取文件句柄
    Value* handleVal = args[0];
    if (!handleVal) {
        return nullptr;
    }
    
    Integer* handleInt = dynamic_cast<Integer*>(handleVal);
    if (!handleInt) {
        return nullptr;
    }
    
    int fileHandle = handleInt->getValue();
    
    // 检查文件句柄是否有效
    if (!isValidFileHandle(fileHandle)) {
        return nullptr;
    }
    
    // 关闭文件
    FILE* file = fileHandles[fileHandle];
    if (file) {
        fclose(file);
        fileHandles.erase(fileHandle);
    }
    
    return new Integer(0);  // 成功返回0
}

Value* builtin_read(vector<Value*>& args) {
    if (args.size() < 3 || !args[0] || !args[1] || !args[2]) {
        return nullptr;
    }
    
    // 获取文件句柄
    Value* handleVal = args[0];
    if (!handleVal) {
        return nullptr;
    }
    
    Integer* handleInt = dynamic_cast<Integer*>(handleVal);
    if (!handleInt) {
        return nullptr;
    }
    
    int fileHandle = handleInt->getValue();
    
    // 检查文件句柄是否有效
    if (!isValidFileHandle(fileHandle)) {
        return nullptr;
    }
    
    FILE* file = fileHandles[fileHandle];
    if (!file) {
        return nullptr;
    }
    
    // 获取缓冲区（可以是任何类型，我们将其转换为字符串）
    Value* bufferVal = args[1];
    if (!bufferVal) {
        return nullptr;
    }
    
    // 将Buffer参数转换为字符串类型
    String* bufferStr = dynamic_cast<String*>(bufferVal);
    if (!bufferStr) {
        // 如果不是字符串类型，创建一个新的字符串
        bufferStr = new String("");
        // 这里需要更新args[1]指向新的字符串
        // 但是直接修改参数可能不安全，我们先返回错误
        return nullptr;
    }
    
    // 获取读取大小
    Value* sizeVal = args[2];
    if (!sizeVal) {
        return nullptr;
    }
    
    Integer* sizeInt = dynamic_cast<Integer*>(sizeVal);
    if (!sizeInt) {
        return nullptr;
    }
    
    int readSize = sizeInt->getValue();
    if (readSize <= 0) {
        return nullptr;
    }
    
    // 检查文件流状态
    if (feof(file)) {
        return new Integer(0);  // 文件已结束，返回0
    }
    
    // 读取指定大小的数据
    char* tempBuffer = new char[readSize + 1];
    size_t bytesRead = fread(tempBuffer, 1, readSize, file);
    tempBuffer[bytesRead] = '\0';  // 确保字符串结束
    
    // 将读取到的内容赋值给Buffer参数
    if (bytesRead > 0) {
        bufferStr->setValue(string(tempBuffer));
    }
    
    // 返回fread的实际返回值
    delete[] tempBuffer;
    return new Integer(bytesRead);
}

Value* builtin_write(vector<Value*>& args) {
    if (args.size() < 2 || !args[0] || !args[1]) {
        return nullptr;
    }
    
    // 获取文件句柄
    Value* handleVal = args[0];
    if (!handleVal) {
        return nullptr;
    }
    
    Integer* handleInt = dynamic_cast<Integer*>(handleVal);
    if (!handleInt) {
        return nullptr;
    }
    
    int fileHandle = handleInt->getValue();
    
    // 检查文件句柄是否有效
    if (!isValidFileHandle(fileHandle)) {
        return nullptr;
    }
    
    FILE* file = fileHandles[fileHandle];
    if (!file) {
        return nullptr;
    }
    
    // 获取要写入的内容
    Value* contentVal = args[1];
    if (!contentVal) {
        return nullptr;
    }
    
    // 将内容转换为字符串并写入文件
    string content;
    if (String* strVal = dynamic_cast<String*>(contentVal)) {
        content = strVal->getValue();
    } else {
        // 如果不是字符串，尝试转换为字符串
        content = contentVal->getTypeName();
    }
    size_t bytesWritten = fputs(content.c_str(), file);
    
    // 如果指定了换行，则添加换行符
    if (args.size() >= 3 && args[2]) {
        Value* newlineVal = args[2];
        if (newlineVal) {
            // 检查是否为true或非零值
            bool addNewline = false;
            if (Integer* intVal = dynamic_cast<Integer*>(newlineVal)) {
                addNewline = (intVal->getValue() != 0);
            } else if (Bool* boolVal = dynamic_cast<Bool*>(newlineVal)) {
                addNewline = boolVal->getValue();
            } else if (String* strVal = dynamic_cast<String*>(newlineVal)) {
                addNewline = (strVal->getValue() == "true" || strVal->getValue() == "1");
            }
            
            if (addNewline) {
                fputs("\n", file);
                bytesWritten += 1;  // 换行符占1个字节
            }
        }
    }
    
    // 刷新缓冲区
    fflush(file);
    
    return new Integer(bytesWritten);  // 返回实际写入的字节数
}

// IO插件类
class IOPlugin : public BuiltinPlugin {
public:
    PluginInfo getPluginInfo() const override {
        return PluginInfo{
            "io",
            "1.0.0",
            "IO操作插件，包含文件读写功能",
            {"open", "close", "read", "write"}
        };
    }
    
    void registerFunctions(ScopeManager& scopeManager) override {
        // 使用辅助方法批量注册函数
        defineBuiltinFunctions(scopeManager, getFunctionMap());
    }
    
    map<string, BuiltinFunctionPtr> getFunctionMap() const override {
        return {
            {"open", builtin_open},
            {"close", builtin_close},
            {"read", builtin_read},
            {"write", builtin_write}
        };
    }
};

// 导出插件
EXPORT_PLUGIN(IOPlugin)
