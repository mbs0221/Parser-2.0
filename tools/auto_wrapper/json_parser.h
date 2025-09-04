#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace AutoWrapper {

// 简单的JSON解析器（仅用于演示，实际项目中应使用专业JSON库）
class SimpleJsonParser {
public:
    static std::string extractString(const std::string& json, const std::string& key);
    static std::vector<std::string> extractStringArray(const std::string& json, const std::string& key);
    static std::map<std::string, std::string> extractObject(const std::string& json, const std::string& key);
    static std::vector<std::map<std::string, std::string>> extractObjectArray(const std::string& json, const std::string& key);
    
private:
    static std::string trim(const std::string& str);
    static std::string unescapeJsonString(const std::string& str);
    static size_t findMatchingBrace(const std::string& str, size_t start);
    static size_t findMatchingBracket(const std::string& str, size_t start);
};

} // namespace AutoWrapper
