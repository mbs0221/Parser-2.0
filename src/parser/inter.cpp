#include "parser/inter.h"

// 这里只包含在inter.h中定义的基类的实现
// 其他具体类的accept方法实现应该在各自的.cpp文件中

// Expression基类的默认实现
int Expression::getTypePriority() const {
    return 0; // 默认优先级
}
