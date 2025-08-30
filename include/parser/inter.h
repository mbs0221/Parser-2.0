#ifndef INTER_H
#define INTER_H


#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <typeinfo>

using namespace std;

// 前向声明
class StatementVisitor;
template<typename ReturnType>
class ExpressionVisitor;
template<typename ReturnType>
class ASTVisitor;
class Statement;
class Expression;
class Value;

// ==================== AST基类 ====================
// 抽象语法树节点基类
struct AST {
    virtual ~AST() = default;
    
    // 获取节点位置信息（用于错误报告）
    virtual string getLocation() const { return "unknown"; }
};



#endif // INTER_H