#include "common/function_signature_parser.h"
#include "common/logger.h"
#include "interpreter/values/value.h"
#include <cctype>
#include <sstream>

using namespace std;

// 解析C风格函数原型（包括函数名和参数）
FunctionSignatureParseResult FunctionSignatureParser::parseCFormatParams(const char* functionPrototype) {
    if (!functionPrototype) {
        return FunctionSignatureParseResult();
    }
    return parseCFormatParams(string(functionPrototype));
}

// 解析C风格函数原型（字符串版本）
FunctionSignatureParseResult FunctionSignatureParser::parseCFormatParams(const string& functionPrototype) {
    FunctionSignatureParseResult result;
    
    if (functionPrototype.empty()) {
        return result;
    }
    
    size_t pos = 0;
    
    // 跳过开头的空白字符
    skipWhitespace(functionPrototype, pos);
    
    // 解析函数名
    parseFunctionName(functionPrototype, pos, result.functionName);
    
    // 跳过空白字符
    skipWhitespace(functionPrototype, pos);
    
    // 检查是否有参数列表（以 '(' 开始）
    if (pos < functionPrototype.length() && functionPrototype[pos] == '(') {
        pos++; // 跳过 '('
        
        // 跳过空白字符
        skipWhitespace(functionPrototype, pos);
        
        // 检查是否以 "..." 开头（纯可变参数）
        if (pos < functionPrototype.length() && functionPrototype.substr(pos, 3) == "...") {
            // 检查前面是否有逗号，如果有则不是纯可变参数
            bool hasCommaBefore = false;
            size_t checkPos = pos - 1;
            while (checkPos > 0 && isspace(functionPrototype[checkPos])) {
                checkPos--;
            }
            if (checkPos > 0 && functionPrototype[checkPos] == ',') {
                hasCommaBefore = true;
            }
            
            if (!hasCommaBefore) {
                result.parameters.push_back(Parameter("...", "any", "", true));
                result.parameterNames.push_back("...");
                result.varArgsSupport = true;
                result.varArgsName = "...";
                LOG_DEBUG("FunctionSignatureParser: C-style prototype parsed as variadic only");
                return result;
            }
        }
        
        // 解析参数列表
        while (pos < functionPrototype.length()) {
            // 跳过空白字符
            skipWhitespace(functionPrototype, pos);
            
            if (pos >= functionPrototype.length()) break;
            
            // 检查是否到达参数列表结束
            if (functionPrototype[pos] == ')') {
                pos++;
                break;
            }
            
            // 解析单个参数
            parseParameter(functionPrototype, pos, result.parameters, result.parameterNames, 
                          result.paramDefaults, result.varArgsSupport, result.varArgsName);
            
            // 跳过空白字符
            skipWhitespace(functionPrototype, pos);
            
            // 检查是否有逗号分隔符
            if (pos < functionPrototype.length() && functionPrototype[pos] == ',') {
                pos++; // 跳过逗号
            }
        }
    }
    
    LOG_DEBUG("FunctionSignatureParser: Parsed function '" + result.functionName + 
              "' with " + to_string(result.parameters.size()) + " parameters");
    
    return result;
}

// 验证函数原型格式
bool FunctionSignatureParser::isValidFunctionPrototype(const string& prototype) {
    if (prototype.empty()) return false;
    
    size_t pos = 0;
    skipWhitespace(prototype, pos);
    
    // 检查函数名
    if (pos >= prototype.length() || !isIdentifierChar(prototype[pos])) {
        return false;
    }
    
    // 跳过函数名
    while (pos < prototype.length() && isIdentifierChar(prototype[pos])) {
        pos++;
    }
    
    skipWhitespace(prototype, pos);
    
    // 检查是否有参数列表
    if (pos >= prototype.length() || prototype[pos] != '(') {
        return false;
    }
    
    pos++; // 跳过 '('
    
    // 简单的括号匹配检查
    int parenCount = 1;
    while (pos < prototype.length() && parenCount > 0) {
        if (prototype[pos] == '(') parenCount++;
        else if (prototype[pos] == ')') parenCount--;
        pos++;
    }
    
    return parenCount == 0;
}

// 从解析结果创建Parameter对象列表
vector<Parameter> FunctionSignatureParser::createParametersFromResult(const FunctionSignatureParseResult& result) {
    return result.parameters;
}

// 内部解析方法实现

void FunctionSignatureParser::parseFunctionName(const string& prototype, size_t& pos, string& functionName) {
    size_t funcNameStart = pos;
    while (pos < prototype.length() && isIdentifierChar(prototype[pos])) {
        pos++;
    }
    
    if (pos > funcNameStart) {
        functionName = prototype.substr(funcNameStart, pos - funcNameStart);
        LOG_DEBUG("FunctionSignatureParser: Parsed function name: " + functionName);
    }
}

void FunctionSignatureParser::parseParameterList(const string& prototype, size_t& pos, 
                                                vector<Parameter>& parameters,
                                                vector<string>& parameterNames,
                                                map<string, string>& paramDefaults,
                                                bool& varArgsSupport, string& varArgsName) {
    // 解析参数列表
    while (pos < prototype.length()) {
        // 跳过空白字符
        skipWhitespace(prototype, pos);
        
        if (pos >= prototype.length()) break;
        
        // 检查是否到达参数列表结束
        if (prototype[pos] == ')') {
            pos++;
            break;
        }
        
        // 解析单个参数
        parseParameter(prototype, pos, parameters, parameterNames, paramDefaults, varArgsSupport, varArgsName);
        
        // 跳过空白字符
        skipWhitespace(prototype, pos);
        
        // 检查是否有逗号分隔符
        if (pos < prototype.length() && prototype[pos] == ',') {
            pos++; // 跳过逗号
        }
    }
}

void FunctionSignatureParser::parseParameter(const string& prototype, size_t& pos,
                                            vector<Parameter>& parameters,
                                            vector<string>& parameterNames,
                                            map<string, string>& paramDefaults,
                                            bool& varArgsSupport, string& varArgsName) {
    // 跳过空白字符
    skipWhitespace(prototype, pos);
    
    if (pos >= prototype.length()) return;
    
    // 检查是否为可变参数标记
    if (pos + 2 < prototype.length() && prototype.substr(pos, 3) == "...") {
        parameters.push_back(Parameter("...", "any", "", true));
        parameterNames.push_back("...");
        varArgsSupport = true;
        varArgsName = "...";
        pos += 3;
        LOG_DEBUG("FunctionSignatureParser: C-style prototype parsed with variadic support");
        return;
    }
    
    // 解析参数名称
    size_t start = pos;
    while (pos < prototype.length() && isIdentifierChar(prototype[pos])) {
        pos++;
    }
    
    if (pos > start) {
        string paramName = prototype.substr(start, pos - start);
        string defaultValue = "";
        bool hasDefault = false;
        
        // 检查是否有默认值
        if (pos < prototype.length() && prototype[pos] == '=') {
            pos++; // 跳过 '=' 符号
            
            // 解析默认值
            size_t defaultValueStart = pos;
            while (pos < prototype.length() && 
                   prototype[pos] != ',' && prototype[pos] != ';' && 
                   prototype[pos] != ')' && !isspace(prototype[pos])) {
                pos++;
            }
            
            if (pos > defaultValueStart) {
                defaultValue = prototype.substr(defaultValueStart, pos - defaultValueStart);
                hasDefault = true;
                paramDefaults[paramName] = defaultValue;
                LOG_DEBUG("FunctionSignatureParser: Parameter '" + paramName + "' has default value: " + defaultValue);
            }
        }
        
        // 创建Parameter对象，包含默认值信息
        parameters.push_back(Parameter(paramName, "any", defaultValue, false));
        parameterNames.push_back(paramName);
    }
}

void FunctionSignatureParser::skipWhitespace(const string& str, size_t& pos) {
    while (pos < str.length() && isspace(str[pos])) {
        pos++;
    }
}

bool FunctionSignatureParser::isIdentifierChar(char c) {
    return isalnum(c) || c == '_';
}
