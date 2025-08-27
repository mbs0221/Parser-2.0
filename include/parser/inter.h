#ifndef INTER_H
#define INTER_H

#include "lexer/lexer.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <typeinfo>

using namespace std;

// 前向声明
class ASTVisitor;
class Value;

// ==================== AST基类 ====================
// 抽象语法树节点基类
struct AST {
    virtual ~AST() = default;
    
    // 获取节点位置信息（用于错误报告）
    virtual string getLocation() const { return "unknown"; }

    // 接受访问者 - 标准访问者模式，void返回类型
    virtual void accept(ASTVisitor* visitor) = 0;
};

// ==================== 表达式基类 ====================
// 表达式基类
struct Expression : public AST {
    // 访问者模式：接受AST访问者
    virtual void accept(ASTVisitor* visitor) override = 0;
    virtual int getTypePriority() const = 0;
};

// 叶子节点基类已删除，现在只有VariableExpression和ConstantExpression作为叶子节点

#endif // INTER_H