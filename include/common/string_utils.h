#ifndef INTERPRETER_UTILS_STRING_UTILS_H
#define INTERPRETER_UTILS_STRING_UTILS_H

#include <string>
#include <vector>

namespace interpreter {
namespace utils {

/**
 * 连接字符串向量
 * @param vec 要连接的字符串向量
 * @param delimiter 分隔符
 * @return 连接后的字符串
 */
std::string join(const std::vector<std::string>& vec, const std::string& delimiter);

} // namespace utils
} // namespace interpreter

#endif // INTERPRETER_UTILS_STRING_UTILS_H
