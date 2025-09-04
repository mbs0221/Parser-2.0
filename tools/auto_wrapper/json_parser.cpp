#include "json_parser.h"
#include <algorithm>
#include <cctype>

namespace AutoWrapper {

std::string SimpleJsonParser::extractString(const std::string& json, const std::string& key) {
    std::string search_key = "\"" + key + "\"";
    size_t pos = json.find(search_key);
    if (pos == std::string::npos) {
        return "";
    }
    
    // 查找冒号
    pos = json.find(":", pos);
    if (pos == std::string::npos) {
        return "";
    }
    
    // 跳过空白字符
    pos = json.find_first_not_of(" \t\n\r", pos + 1);
    if (pos == std::string::npos || json[pos] != '"') {
        return "";
    }
    
    // 查找字符串结束位置
    size_t start = pos + 1;
    size_t end = start;
    while (end < json.length() && json[end] != '"') {
        if (json[end] == '\\') {
            end += 2; // 跳过转义字符
        } else {
            end++;
        }
    }
    
    if (end >= json.length()) {
        return "";
    }
    
    return unescapeJsonString(json.substr(start, end - start));
}

std::vector<std::string> SimpleJsonParser::extractStringArray(const std::string& json, const std::string& key) {
    std::vector<std::string> result;
    std::string search_key = "\"" + key + "\"";
    size_t pos = json.find(search_key);
    if (pos == std::string::npos) {
        return result;
    }
    
    // 查找冒号
    pos = json.find(":", pos);
    if (pos == std::string::npos) {
        return result;
    }
    
    // 查找数组开始位置
    pos = json.find("[", pos);
    if (pos == std::string::npos) {
        return result;
    }
    
    size_t array_end = findMatchingBracket(json, pos);
    if (array_end == std::string::npos) {
        return result;
    }
    
    // 解析数组内容
    std::string array_content = json.substr(pos + 1, array_end - pos - 1);
    std::istringstream iss(array_content);
    std::string item;
    
    while (std::getline(iss, item, ',')) {
        item = trim(item);
        if (item.length() >= 2 && item[0] == '"' && item.back() == '"') {
            result.push_back(unescapeJsonString(item.substr(1, item.length() - 2)));
        }
    }
    
    return result;
}

std::map<std::string, std::string> SimpleJsonParser::extractObject(const std::string& json, const std::string& key) {
    std::map<std::string, std::string> result;
    std::string search_key = "\"" + key + "\"";
    size_t pos = json.find(search_key);
    if (pos == std::string::npos) {
        return result;
    }
    
    // 查找冒号
    pos = json.find(":", pos);
    if (pos == std::string::npos) {
        return result;
    }
    
    // 查找对象开始位置
    pos = json.find("{", pos);
    if (pos == std::string::npos) {
        return result;
    }
    
    size_t object_end = findMatchingBrace(json, pos);
    if (object_end == std::string::npos) {
        return result;
    }
    
    // 解析对象内容
    std::string object_content = json.substr(pos + 1, object_end - pos - 1);
    
    // 简单的键值对解析
    std::istringstream iss(object_content);
    std::string line;
    while (std::getline(iss, line, ',')) {
        line = trim(line);
        size_t colon_pos = line.find(":");
        if (colon_pos != std::string::npos) {
            std::string key_part = trim(line.substr(0, colon_pos));
            std::string value_part = trim(line.substr(colon_pos + 1));
            
            // 移除引号
            if (key_part.length() >= 2 && key_part[0] == '"' && key_part.back() == '"') {
                key_part = key_part.substr(1, key_part.length() - 2);
            }
            if (value_part.length() >= 2 && value_part[0] == '"' && value_part.back() == '"') {
                value_part = value_part.substr(1, value_part.length() - 2);
            }
            
            result[key_part] = value_part;
        }
    }
    
    return result;
}

std::vector<std::map<std::string, std::string>> SimpleJsonParser::extractObjectArray(const std::string& json, const std::string& key) {
    std::vector<std::map<std::string, std::string>> result;
    std::string search_key = "\"" + key + "\"";
    size_t pos = json.find(search_key);
    if (pos == std::string::npos) {
        return result;
    }
    
    // 查找冒号
    pos = json.find(":", pos);
    if (pos == std::string::npos) {
        return result;
    }
    
    // 查找数组开始位置
    pos = json.find("[", pos);
    if (pos == std::string::npos) {
        return result;
    }
    
    size_t array_end = findMatchingBracket(json, pos);
    if (array_end == std::string::npos) {
        return result;
    }
    
    // 解析数组内容
    std::string array_content = json.substr(pos + 1, array_end - pos - 1);
    
    // 查找所有对象
    size_t current_pos = 0;
    while (current_pos < array_content.length()) {
        size_t obj_start = array_content.find("{", current_pos);
        if (obj_start == std::string::npos) {
            break;
        }
        
        size_t obj_end = findMatchingBrace(array_content, obj_start);
        if (obj_end == std::string::npos) {
            break;
        }
        
        std::string obj_content = array_content.substr(obj_start, obj_end - obj_start + 1);
        result.push_back(extractObject(obj_content, ""));
        
        current_pos = obj_end + 1;
    }
    
    return result;
}

std::string SimpleJsonParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string SimpleJsonParser::unescapeJsonString(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                default: result += str[i + 1]; break;
            }
            ++i; // 跳过转义字符
        } else {
            result += str[i];
        }
    }
    
    return result;
}

size_t SimpleJsonParser::findMatchingBrace(const std::string& str, size_t start) {
    int brace_count = 0;
    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] == '{') {
            brace_count++;
        } else if (str[i] == '}') {
            brace_count--;
            if (brace_count == 0) {
                return i;
            }
        }
    }
    return std::string::npos;
}

size_t SimpleJsonParser::findMatchingBracket(const std::string& str, size_t start) {
    int bracket_count = 0;
    for (size_t i = start; i < str.length(); ++i) {
        if (str[i] == '[') {
            bracket_count++;
        } else if (str[i] == ']') {
            bracket_count--;
            if (bracket_count == 0) {
                return i;
            }
        }
    }
    return std::string::npos;
}

} // namespace AutoWrapper
