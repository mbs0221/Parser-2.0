#include "interpreter/plugins/builtin_plugin.h"
// #include "interpreter/plugins/plugin_manager.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"

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

// ==================== IO函数实现 - 尽量通过类型系统调用方法 ====================

Value* builtin_open(Scope* scope) {
    // 获取文件名
    if (Value* fileNameVal = scope->getArgument<Value>("filename")) {
        // 通过类型系统调用to_string方法获取文件名
        vector<Value*> methodArgs;
        Value* fileNameStr = PluginManager::callMethodOnValue(fileNameVal, "to_string", methodArgs);
        if (!fileNameStr) return nullptr;
        
        string fileName = fileNameStr->toString();
        
        // 确定打开模式
        string mode = "r";  // 默认只读模式
        if (String* modeVal = scope->getArgument<String>("mode")) {
            mode = modeVal->getValue();
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
    return nullptr;
}

Value* builtin_close(Scope* scope) {
    // 获取文件句柄
    if (Integer* handleInt = scope->getArgument<Integer>("handle")) {
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
    return nullptr;
}

Value* builtin_read(Scope* scope) {
    // 获取文件句柄
    if (Integer* handleInt = scope->getArgument<Integer>("handle")) {
        int fileHandle = handleInt->getValue();
        
        // 检查文件句柄是否有效
        if (!isValidFileHandle(fileHandle)) {
            return nullptr;
        }
        
        FILE* file = fileHandles[fileHandle];
        if (!file) {
            return nullptr;
        }
        
        // 获取读取大小
        if (Integer* sizeInt = scope->getArgument<Integer>("length")) {
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
            
            // 返回fread的实际返回值
            delete[] tempBuffer;
            return new Integer(bytesRead);
        }
    }
    return nullptr;
}

Value* builtin_write(Scope* scope) {
    // 获取文件句柄
    if (Integer* handleInt = scope->getArgument<Integer>("handle")) {
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
        if (Value* contentVal = scope->getArgument<Value>("content")) {
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
            if (Bool* newlineVal = scope->getArgument<Bool>("newline")) {
                if (newlineVal->getValue()) {
                    fputs("\n", file);
                    bytesWritten += 1;  // 换行符占1个字节
                }
            }
            
            // 刷新缓冲区
            fflush(file);
            
            return new Integer(bytesWritten);  // 返回实际写入的字节数
        }
    }
    return nullptr;
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
    
protected:

    
    // 重写注册函数方法，使用新的scope接口和C函数原型解析
    void registerFunctions(ScopeManager& scopeManager) override {
        LOG_DEBUG("IOPlugin::registerFunctions called with new scope interface and C function prototype parsing");
        
        // 使用C函数原型直接创建BuiltinFunction对象
        vector<BuiltinFunction*> funcObjects = {
            new BuiltinFunction([](Scope* scope) -> Value* { return builtin_open(scope); }, "open(filename, mode)"),
            new BuiltinFunction([](Scope* scope) -> Value* { return builtin_close(scope); }, "close(handle)"),
            new BuiltinFunction([](Scope* scope) -> Value* { return builtin_read(scope); }, "read(handle, length)"),
            new BuiltinFunction([](Scope* scope) -> Value* { return builtin_write(scope); }, "write(handle, content, newline)")
        };
        
        LOG_DEBUG("Created " + to_string(funcObjects.size()) + " functions with C function prototype parsing");
        
        for (BuiltinFunction* func : funcObjects) {
            LOG_DEBUG("Registering function '" + func->getName() + "' with " + to_string(func->getParameters().size()) + " parameters");
            scopeManager.defineFunction(func->getName(), func);
        }
    }
};

// 导出插件
EXPORT_PLUGIN(IOPlugin)
