#include "interpreter/interpreter.h"
#include "parser/expression.h"
#include "parser/function.h"
#include "parser/inter.h"
#include "interpreter/logger.h"
#include "interpreter/value.h"
#include "interpreter/builtin_type.h"
#include "interpreter/type_registry.h"

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
    // 初始化类型系统
    typeRegistry = TypeRegistry::getInstance();
    
    // 初始化对象工厂
    objectFactory = getObjectFactory();
    
    // 初始化类型转换器
    typeConverter = new TypeConverter(typeRegistry, this);
    
    // 设置ObjectFactory的依赖注入
    objectFactory->setTypeRegistry(typeRegistry);
    objectFactory->setInterpreter(this);
    
    // ScopeManager在构造函数中自动初始化
    
    // 自动加载插件目录中的插件
    loadDefaultPlugins();
    
    // 注册所有插件函数
    pluginManager.registerAllPlugins(scopeManager);
}

// 解释器构造函数（可选择是否加载插件）
Interpreter::Interpreter(bool loadPlugins) {
    // 初始化类型系统
    typeRegistry = TypeRegistry::getInstance();
    
    // 初始化对象工厂
    objectFactory = getObjectFactory();
    
    // 初始化类型转换器
    typeConverter = new TypeConverter(typeRegistry, this);
    
    // 设置ObjectFactory的依赖注入
    objectFactory->setTypeRegistry(typeRegistry);
    objectFactory->setInterpreter(this);
    
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
    // 清理类型转换器
    if (typeConverter) {
        delete typeConverter;
        typeConverter = nullptr;
    }
    
    // ScopeManager在析构函数中自动清理
}

// 程序执行
void Interpreter::visit(Program* program) {
    if (!program) return;
    
    for (Statement* stmt : program->statements) {
        visit(stmt);  // 直接调用visit方法，消除execute函数依赖
    }
}

void Interpreter::visit(FunctionPrototype* funcProto) {
    if (!funcProto) return;
    // 函数原型在运行时不需要执行，实际执行的是FunctionDefinition
    // 这里可以留空，或者用于调试目的
}

void Interpreter::visit(StructDefinition* structDef) {
    if (!structDef) return;
    
    // 使用TypeConverter注册结构体定义
    StructType* structType = typeConverter->convertStructDefinition(structDef);
    if (structType) {
            LOG_DEBUG("Successfully registered struct '" + structDef->name + "' with " + 
              to_string(structDef->members.size()) + " members");
    } else {
        LOG_ERROR("Failed to register struct '" + structDef->name + "'");
    }
}

void Interpreter::visit(ClassDefinition* classDef) {
    if (!classDef) return;
    
    // 使用TypeConverter注册类定义
    ClassType* classType = typeConverter->convertClassDefinition(classDef);
    if (classType) {
        LOG_DEBUG("Successfully registered class '" + classDef->name + "' with " + 
                 to_string(classDef->members.size()) + " members and " + 
                 to_string(classDef->methods.size()) + " methods");
    } else {
        LOG_ERROR("Failed to register class '" + classDef->name + "'");
    }
}

// Identifier访问方法 - 使用类型注册系统
void Interpreter::visit(Identifier* id) {
    if (!id) return;
    
    string identifierType = id->getIdentifierType();
    LOG_DEBUG("Visiting parser identifier: " + id->name + " (type: " + identifierType + ")");
    
    // 根据标识符类型进行不同的处理
    if (identifierType == "Variable") {
        // Variable类型 - 使用类型注册系统
        Variable* var = dynamic_cast<Variable*>(id);
        if (var) {
            // TODO: 使用TypeConverter注册变量定义
            // VariableDeclaration* varDecl = new VariableDeclaration(var->name, var->getType(), nullptr);
            // typeRegistry->registerVariableDeclaration(varDecl, this);
            // delete varDecl; // 清理临时对象
            
            LOG_DEBUG("Registered Variable: " + var->name);
        }
    } else if (identifierType == "FunctionDefinition") {
        // FunctionDefinition类型 - 使用现有的函数注册逻辑
        FunctionDefinition* funcDef = dynamic_cast<FunctionDefinition*>(id);
        if (funcDef) {
            // 使用现有的函数注册逻辑
            int paramCount = funcDef->prototype ? funcDef->prototype->parameters.size() : 0;
            scopeManager.defineUserFunction(funcDef->name, funcDef, paramCount, this, [](ScopeManagerPtr scopeMgr, FunctionDefinition* funcDef, vector<Value*>& args) -> Value* {
                // 创建UserFunctionWrapper并执行
                UserFunctionWrapper* userFunc = new UserFunctionWrapper(funcDef->name, funcDef, funcDef->prototype ? funcDef->prototype->parameters.size() : 0, scopeMgr, nullptr);
                // 注意：这里无法直接调用executeUserFunction，因为lambda不能捕获this
                // 需要修改设计或使用其他方法
                return nullptr; // 临时返回nullptr
            });
            
            LOG_DEBUG("Registered FunctionDefinition: " + funcDef->name);
        }
    }
}

// Variable访问方法 - 已合并到Identifier访问方法中
void Interpreter::visit(Variable* var) {
    if (!var) return;
    // 直接调用Identifier的访问方法
    visit(static_cast<Identifier*>(var));
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

// ==================== 类型系统相关方法 ====================

// 获取值的类型名称 - 直接使用运行时类型系统
string Interpreter::getValueTypeName(Value* value) {
    if (!value) return "unknown";
    
    // 直接使用Value的valueType指针获取类型名称
    ObjectType* valueType = value->getValueType();
    if (valueType) {
        return valueType->getTypeName();
    }
    
    // 如果valueType为空，返回unknown（这种情况不应该发生）
    return "unknown";
}



// 调用类型方法
Value* Interpreter::callTypeMethod(Value* instance, const string& methodName, vector<Value*>& args) {
    if (!instance) return nullptr;
    
    // 获取类型名称并调用类型方法
    string typeName = getValueTypeName(instance);
    ObjectType* type = typeRegistry->getType(typeName);
    if (type) {
        // 直接调用类型方法
        return type->callMethod(instance, methodName, args);
    }
    throw runtime_error("Type '" + typeName + "' not found");
}

// ==================== 函数调用辅助方法 ====================

// 执行内置函数
Value* Interpreter::executeBuiltinFunction(BuiltinFunctionWrapper* builtinFunc, vector<Value*>& evaluatedArgs) {
    if (!builtinFunc) {
        reportError("Builtin function is null");
        return nullptr;
    }
    
    // 直接调用内置函数包装器
    return builtinFunc->call(evaluatedArgs);
}

// 执行用户函数
Value* Interpreter::executeUserFunction(UserFunctionWrapper* userFunc, vector<Value*>& evaluatedArgs) {
    if (!userFunc) {
        reportError("User function is null");
        return nullptr;
    }
    
    // 直接调用用户函数包装器
    return userFunc->call(evaluatedArgs);
}
