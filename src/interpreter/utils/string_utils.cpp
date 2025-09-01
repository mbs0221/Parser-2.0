#include "interpreter/utils/string_utils.h"

namespace interpreter {
namespace utils {

std::string join(const std::vector<std::string>& vec, const std::string& delimiter) {
    if (vec.empty()) return "";
    
    std::string result = vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        result += delimiter + vec[i];
    }
    return result;
}

} // namespace utils
} // namespace interpreter
