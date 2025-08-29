#include "interpreter/interpreter.h"
#include "parser/expression.h"
#include "parser/function.h"
#include "parser/inter.h"
#include "lexer/lexer.h"
#include "interpreter/logger.h"
#include "lexer/value.h"

#include <iostream>
#include <sstream>
#include <typeinfo>
#include <map>
#include <functional>
#include <cstdio>
#include <cstdlib>

using namespace std;

// 解释器构造函数
Interpreter::Interpreter() {
    // ScopeManager在构造函数中自动初始化
    
    // 自动加载插件目录中的插件
    loadDefaultPlugins();
    
    // 注册所有插件函数
    pluginManager.registerAllPlugins(scopeManager);
}

// 解释器构造函数（可选择是否加载插件）
Interpreter::Interpreter(bool loadPlugins) {
    // ScopeManager在构造函数中自动初始化
    
    if (loadPlugins) {
        // 自动加载插件目录中的插件
        loadDefaultPlugins();
        
        // 注册所有插件函数
        pluginManager.registerAllPlugins(scopeManager);
    }
}

// 解释器析构函数
Interpreter::~Interpreter() {
    // ScopeManager在析构函数中自动清理
}

// 执行语句
void Interpreter::execute(Statement* stmt) {
    if (!stmt) return;
    stmt->accept(this);
}

// 执行程序
void Interpreter::execute(Program* program) {
    if (!program) return;
    program->accept(this);
}

// 程序执行
void Interpreter::visit(Program* program) {
    if (!program) return;
    
    for (Statement* stmt : program->statements) {
        execute(stmt);
    }
}

void Interpreter::visit(FunctionPrototype* funcProto) {
    if (!funcProto) return;
    // 函数原型不需要执行，只是声明
    LOG_DEBUG("Function prototype: " + funcProto->name);
}

void Interpreter::visit(StructDefinition* structDef) {
    if (!structDef) return;
    
    // 将结构体定义注册到作用域中
    scopeManager.defineStruct(structDef->name, structDef);
    
    LOG_DEBUG("Registered struct '" + structDef->name + "' with " + to_string(structDef->members.size()) + " members");
}

void Interpreter::visit(ClassDefinition* classDef) {
    if (!classDef) return;
    
    // 将类定义注册到作用域中
    scopeManager.defineClass(classDef->name, classDef);
    
    LOG_DEBUG("Registered class '" + classDef->name + "' with " + to_string(classDef->members.size()) + " members and " + to_string(classDef->methods.size()) + " methods");
}

// Identifier访问方法
void Interpreter::visit(Identifier* id) {
    if (!id) return;
    // Identifier主要用于类型检查，不需要执行
    LOG_DEBUG("Visiting identifier: " + id->name);
}

// Variable访问方法
void Interpreter::visit(Variable* var) {
    if (!var) return;
    // Variable主要用于类型检查，不需要执行
    LOG_DEBUG("Visiting variable: " + var->name);
}

// 类实例化
Value* Interpreter::instantiateClass(ClassDefinition* classDef, vector<Value*>& args) {
    if (!classDef) return nullptr;
    
    // 创建字典来存储实例的成员
    Dict* instance = new Dict();
    
    // 根据参数顺序初始化成员
    const vector<StructMember>& members = classDef->members;
    for (size_t i = 0; i < members.size() && i < args.size(); ++i) {
        const StructMember& member = members[i];
        instance->setEntry(member.name, args[i]);
    }
    
    // 为未初始化的成员设置默认值
    for (size_t i = args.size(); i < members.size(); ++i) {
        const StructMember& member = members[i];
        if (member.defaultValue) {
            // 如果有默认值表达式，求值它
            Value* defaultVal = visit(member.defaultValue);
            instance->setEntry(member.name, defaultVal);
        } else {
            // 否则使用类型的默认值
            Value* defaultVal = createDefaultValue(member.type);
            instance->setEntry(member.name, defaultVal);
        }
    }
    
    return instance;
}

// 结构体实例化
Value* Interpreter::instantiateStruct(StructDefinition* structDef, vector<Value*>& args) {
    if (!structDef) return nullptr;
    
    // 创建字典来存储实例的成员
    Dict* instance = new Dict();
    
    // 检查是否是通过结构体实例化语法传递的参数（第一个参数是Dict）
    if (!args.empty() && dynamic_cast<Dict*>(args[0])) {
        // 结构体实例化语法：Person {name: "Alice", age: 25}
        Dict* memberDict = dynamic_cast<Dict*>(args[0]);
        const vector<StructMember>& members = structDef->members;
        
        // 按照结构体定义的成员顺序初始化
        for (const StructMember& member : members) {
            Value* memberValue = memberDict->getEntry(member.name);
            if (memberValue) {
                // 如果提供了该成员的值，使用提供的值
                instance->setEntry(member.name, memberValue);
            } else if (member.defaultValue) {
                // 如果有默认值表达式，求值它
                Value* defaultVal = visit(member.defaultValue);
                instance->setEntry(member.name, defaultVal);
            } else {
                // 否则使用类型的默认值
                Value* defaultVal = createDefaultValue(member.type);
                instance->setEntry(member.name, defaultVal);
            }
        }
    } else {
        // 构造函数语法：Person("Alice", 25) - 按参数顺序初始化
        const vector<StructMember>& members = structDef->members;
        for (size_t i = 0; i < members.size() && i < args.size(); ++i) {
            const StructMember& member = members[i];
            instance->setEntry(member.name, args[i]);
        }
        
        // 为未初始化的成员设置默认值
        for (size_t i = args.size(); i < members.size(); ++i) {
            const StructMember& member = members[i];
            if (member.defaultValue) {
                // 如果有默认值表达式，求值它
                Value* defaultVal = visit(member.defaultValue);
                instance->setEntry(member.name, defaultVal);
            } else {
                // 否则使用类型的默认值
                Value* defaultVal = createDefaultValue(member.type);
                instance->setEntry(member.name, defaultVal);
            }
        }
    }
    
    return instance;
}

// 创建默认值
Value* Interpreter::createDefaultValue(Type* type) {
    if (!type) return new Integer(0);
    
    if (type == Type::Int) {
        return new Integer(0);
    } else if (type == Type::Double) {
        return new Double(0.0);
    } else if (type == Type::Char) {
        return new Char('\0');
    } else if (type == Type::Bool) {
        return new Bool(false);
    } else if (type == Type::String) {
        return new String("");
    } else {
            return new Integer(0); // 默认返回整数0
    }
}

// 错误处理
void Interpreter::reportError(const string& message) {
    cerr << "Error: " << message << endl;
}

void Interpreter::reportTypeError(const string& expected, const string& actual) {
    cerr << "Type Error: expected " << expected << ", got " << actual << endl;
}

// 插件管理方法
void Interpreter::loadPlugin(const string& pluginPath) {
    if (pluginManager.loadPlugin(pluginPath)) {
        // 重新注册所有插件函数到作用域
        pluginManager.registerAllPlugins(scopeManager);
        LOG_INFO("插件加载成功: " + pluginPath);
    } else {
        LOG_ERROR("插件加载失败: " + pluginPath);
    }
}

void Interpreter::unloadPlugin(const string& pluginName) {
    if (pluginManager.unloadPlugin(pluginName)) {
        LOG_INFO("插件卸载成功: " + pluginName);
    } else {
        LOG_ERROR("插件卸载失败: " + pluginName);
    }
}

vector<string> Interpreter::getLoadedPlugins() const {
    return pluginManager.getLoadedPlugins();
}

// 自动加载默认插件
void Interpreter::loadDefaultPlugins(const string& pluginDir) {
    LOG_INFO("开始扫描插件目录: " + pluginDir);
    
    // 使用系统命令列出目录中的所有.so文件
    string command = "find " + pluginDir + " -name \"*.so\" 2>/dev/null";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        LOG_ERROR("无法打开插件目录: " + pluginDir);
        return;
    }
    
    char buffer[1024];
    vector<string> pluginFiles;
    
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        string filePath = string(buffer);
        // 移除换行符
        if (!filePath.empty() && filePath[filePath.length()-1] == '\n') {
            filePath.erase(filePath.length()-1);
        }
        if (!filePath.empty()) {
            pluginFiles.push_back(filePath);
        }
    }
    
    pclose(pipe);
    
    if (pluginFiles.empty()) {
        LOG_WARN("插件目录中没有找到.so文件: " + pluginDir);
        return;
    }
    
    LOG_INFO("找到 " + to_string(pluginFiles.size()) + " 个插件文件");
    
    // 加载每个插件
    for (const string& pluginPath : pluginFiles) {
        LOG_DEBUG("尝试加载插件: " + pluginPath);
        
        if (pluginManager.loadPlugin(pluginPath)) {
            // 获取插件信息
            string pluginName = extractPluginName(pluginPath);
            PluginInfo info = pluginManager.getPluginInfo(pluginName);
            
            LOG_INFO("成功加载插件: " + info.name + " v" + info.version);
            LOG_DEBUG("插件描述: " + info.description);
            LOG_DEBUG("插件函数: " + join(info.functions, ", "));
        } else {
            LOG_ERROR("加载插件失败: " + pluginPath);
        }
    }
    
    LOG_INFO("插件加载完成，共加载 " + to_string(pluginManager.getLoadedPlugins().size()) + " 个插件");
}

// 重载版本，使用默认插件目录
void Interpreter::loadDefaultPlugins() {
    loadDefaultPlugins("./plugins/");
}

// 辅助函数：从文件路径提取插件名称
string Interpreter::extractPluginName(const string& filePath) {
    size_t lastSlash = filePath.find_last_of('/');
    if (lastSlash == string::npos) {
        lastSlash = -1;
    }
    
    string fileName = filePath.substr(lastSlash + 1);
    
    // 移除lib前缀和.so后缀
    if (fileName.substr(0, 3) == "lib") {
        fileName = fileName.substr(3);
    }
    if (fileName.length() > 3 && fileName.substr(fileName.length() - 3) == ".so") {
        fileName = fileName.substr(0, fileName.length() - 3);
    }
    
    return fileName;
}

// 辅助函数：连接字符串向量
string Interpreter::join(const vector<string>& vec, const string& delimiter) {
    if (vec.empty()) return "";
    
    string result = vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        result += delimiter + vec[i];
    }
    return result;
}
