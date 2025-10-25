#ifndef FUNCTION_SIGNATURE_PARSER_H
#define FUNCTION_SIGNATURE_PARSER_H

#include <string>
#include <vector>
#include <map>

// Parameter类定义
class Parameter {
private:
    std::string name;
    std::string typeName;
    std::string defaultValue;
    bool variadic;

public:
    Parameter(const std::string& paramName, const std::string& paramType, 
              const std::string& defaultVal = "", bool isVariadic = false)
        : name(paramName), typeName(paramType), defaultValue(defaultVal), variadic(isVariadic) {}
    
    std::string getName() const { return name; }
    std::string getTypeName() const { return typeName; }
    std::string getDefaultValue() const { return defaultValue; }
    bool hasDefaultValue() const { return !defaultValue.empty(); }
    bool isVariadic() const { return variadic; }
    
    std::string toString() const {
        std::string result = name + ":" + typeName;
        if (hasDefaultValue()) {
            result += "=" + defaultValue;
        }
        if (isVariadic()) {
            result += "...";
        }
        return result;
    }
    
    bool operator==(const Parameter& other) const {
        return name == other.name && typeName == other.typeName && 
               defaultValue == other.defaultValue && variadic == other.variadic;
    }
    
    bool operator!=(const Parameter& other) const {
        return !(*this == other);
    }
};

// 函数签名解析结果
struct FunctionSignatureParseResult {
    std::string functionName;                    // 函数名
    std::vector<Parameter> parameters;           // 参数列表
    std::vector<std::string> parameterNames;     // 参数名称列表（兼容性）
    std::map<std::string, std::string> paramDefaults;  // 参数默认值映射
    bool varArgsSupport;                         // 是否支持可变参数
    std::string varArgsName;                     // 可变参数名称
    
    FunctionSignatureParseResult() : varArgsSupport(false) {}
};

// 统一的函数签名解析器
class FunctionSignatureParser {
public:
    // 解析C风格函数原型（包括函数名和参数）
    static FunctionSignatureParseResult parseCFormatParams(const char* functionPrototype);
    
    // 解析C风格函数原型（字符串版本）
    static FunctionSignatureParseResult parseCFormatParams(const std::string& functionPrototype);
    
    // 验证函数原型格式
    static bool isValidFunctionPrototype(const std::string& prototype);
    
    // 从解析结果创建Parameter对象列表
    static std::vector<Parameter> createParametersFromResult(const FunctionSignatureParseResult& result);
    
private:
    // 内部解析方法
    static void parseFunctionName(const std::string& prototype, size_t& pos, std::string& functionName);
    static void parseParameterList(const std::string& prototype, size_t& pos, 
                                  std::vector<Parameter>& parameters,
                                  std::vector<std::string>& parameterNames,
                                  std::map<std::string, std::string>& paramDefaults,
                                  bool& varArgsSupport, std::string& varArgsName);
    static void parseParameter(const std::string& prototype, size_t& pos,
                              std::vector<Parameter>& parameters,
                              std::vector<std::string>& parameterNames,
                              std::map<std::string, std::string>& paramDefaults,
                              bool& varArgsSupport, std::string& varArgsName);
    static void skipWhitespace(const std::string& str, size_t& pos);
    static bool isIdentifierChar(char c);
};

#endif // FUNCTION_SIGNATURE_PARSER_H
