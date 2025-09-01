#ifndef PARSER_OPERATOR_MAPPING_H
#define PARSER_OPERATOR_MAPPING_H

#include <string>
#include <unordered_map>
#include "lexer/token.h"

// ==================== 操作符到方法名映射表 ====================
// 这个命名空间负责将AST中的操作符类型转化为对应的类方法名
// 让parser能够为interpreter提供计算好的运算名称

namespace OperatorMapping {
    // 二元操作符映射 - 将AST的Operator类型转化为类方法名
    static const std::unordered_map<int, std::string> BINARY_OPERATORS = {
        // 算术运算符
        {'+', "add"},           // ADD (+)
        {'-', "sub"},           // SUB (-)
        {'*', "mul"},           // MUL (*)
        {'/', "div"},           // DIV (/)
        {'%', "mod"},           // MOD (%)
        
        // 位运算符
        {'&', "bitwiseand"},    // BITWISE_AND (&)
        {'|', "bitwiseor"},     // BITWISE_OR (|)
        {'^', "bitwisexor"},    // BITWISE_XOR (^)
        {1001, "leftshift"},    // LEFT_SHIFT (<<)
        {1002, "rightshift"},   // RIGHT_SHIFT (>>)
        
        // 逻辑运算符
        {1003, "and"},          // AND_AND (&&)
        {1004, "or"},           // OR_OR (||)
        
        // 比较运算符
        {1005, "eq"},           // EQ_EQ (==)
        {1006, "ne"},           // NE_EQ (!=)
        {'<', "lt"},            // LESS_THAN (<)
        {'>', "gt"},            // GREATER_THAN (>)
        {1007, "le"},           // LESS_EQUAL (<=)
        {1008, "ge"},           // GREATER_EQUAL (>=)
        
        // 赋值运算符（基础赋值和复合赋值）
        {'=', "assign"},        // ASSIGN (=)
        {1009, "add_assign"},   // ADD_ASSIGN (+=)
        {1010, "sub_assign"},   // SUB_ASSIGN (-=)
        {1011, "mul_assign"},   // MUL_ASSIGN (*=)
        {1012, "div_assign"},   // DIV_ASSIGN (/=)
        {1013, "mod_assign"},   // MOD_ASSIGN (%=)
        {1014, "and_assign"},   // AND_ASSIGN (&=)
        {1015, "or_assign"},    // OR_ASSIGN (|=)
        {1016, "xor_assign"},   // XOR_ASSIGN (^=)
        {1017, "lshift_assign"}, // LSHIFT_ASSIGN (<<=)
        {1018, "rshift_assign"}, // RSHIFT_ASSIGN (>>=)
        
        // 访问运算符（双目运算）
        {1019, "member_access"}, // MEMBER_ACCESS (.) - 左操作数：对象，右操作数：成员名
        {1020, "subscript"},     // SUBSCRIPT ([]) - 左操作数：容器，右操作数：索引/键
        {1021, "function_call"}, // FUNCTION_CALL (()) - 左操作数：函数，右操作数：参数列表
        
        // 其他可能的AST操作符
        {1022, "comma"},        // COMMA (,)
        {1023, "ternary"},      // TERNARY (? :)
    };
    
    // 一元操作符映射 - 将AST的Operator类型转化为类方法名
    static const std::unordered_map<int, std::string> UNARY_OPERATORS = {
        // 算术一元运算符
        {'+', "unary_plus"},
        {'-', "unary_minus"},
        
        // 逻辑一元运算符
        {'!', "not"},
        
        // 位运算一元运算符
        {'~', "bitwise_not"},
        
        // 自增自减运算符（需要区分前后缀）
        {1024, "prefix_increment"},    // PREFIX_INCREMENT (++i)
        {1025, "postfix_increment"},   // POSTFIX_INCREMENT (i++)
        {1026, "prefix_decrement"},    // PREFIX_DECREMENT (--i)
        {1027, "postfix_decrement"},   // POSTFIX_DECREMENT (i--)
        
        // 地址和引用运算符
        {1028, "address_of"},   // ADDRESS_OF (&)
        {1029, "dereference"},  // DEREFERENCE (*)
        
        // 类型转换运算符
        {1030, "cast"},         // CAST ((type))
        {1031, "sizeof"},       // SIZEOF (sizeof)
        
        // 其他一元操作符
        {1032, "throw"},        // THROW (throw)
        {1033, "delete"},       // DELETE (delete)
        {1034, "new"},          // NEW (new)
    };
    
    // ==================== 操作符查询函数 ====================
    
    // 获取二元操作符对应的方法名
    inline std::string getBinaryMethodName(int op) {
        auto it = BINARY_OPERATORS.find(op);
        return it != BINARY_OPERATORS.end() ? it->second : "";
    }
    
    // 获取操作符对应的操作名（用于calculate方法）- 使用lexer的Operator类
    inline std::string getOperationName(int op) {
        // 创建一个临时的Operator对象来获取操作名
        lexer::Operator tempOp(op, "");
        return tempOp.getOperationName();
    }
    
    // 统一的操作名获取函数（支持单目和双目操作）
    inline std::string getOperationName(int op, bool isUnary = false) {
        lexer::Operator tempOp(op, "");
        return tempOp.getOperationName();
    }
    
    // 获取一元操作符对应的方法名
    inline std::string getUnaryMethodName(int op) {
        auto it = UNARY_OPERATORS.find(op);
        return it != UNARY_OPERATORS.end() ? it->second : "";
    }
    
    // 检查是否为支持的二元操作符
    inline bool isSupportedBinaryOperator(int op) {
        return BINARY_OPERATORS.find(op) != BINARY_OPERATORS.end();
    }
    
    // 检查是否为支持的一元操作符
    inline bool isSupportedUnaryOperator(int op) {
        return UNARY_OPERATORS.find(op) != UNARY_OPERATORS.end();
    }
    
    // 检查是否为赋值操作符
    inline bool isAssignmentOperator(int op) {
        return op >= 1009 && op <= 1018 || op == '=';
    }
    
    // 检查操作符是否是访问操作符（如成员访问、数组访问）
    inline bool isAccessOperator(int op) {
        return op == 1019 || op == 1020 || op == 1021;
    }
    
    // ==================== 操作符符号表示 ====================
    
    // 获取操作符的符号表示（用于错误信息和调试）
    inline std::string getOperatorSymbol(int op) {
        switch (op) {
            // 算术运算符
            case '+': return "+";
            case '-': return "-";
            case '*': return "*";
            case '/': return "/";
            case '%': return "%";
            
            // 位运算符
            case '&': return "&";
            case '|': return "|";
            case '^': return "^";
            case 1001: return "<<";
            case 1002: return ">>";
            
            // 逻辑运算符
            case 1003: return "&&";
            case 1004: return "||";
            
            // 比较运算符
            case 1005: return "==";
            case 1006: return "!=";
            case '<': return "<";
            case '>': return ">";
            case 1007: return "<=";
            case 1008: return ">=";
            
            // 赋值运算符
            case '=': return "=";
            case 1009: return "+=";
            case 1010: return "-=";
            case 1011: return "*=";
            case 1012: return "/=";
            case 1013: return "%=";
            case 1014: return "&=";
            case 1015: return "|=";
            case 1016: return "^=";
            case 1017: return "<<=";
            case 1018: return ">>=";
            
            // 访问运算符
            case 1019: return ".";   // member_access
            case 1020: return "[]";  // subscript
            case 1021: return "()";  // function_call
            
            // 其他操作符
            case 1022: return ",";   // comma
            case 1023: return "?:";  // ternary
            
            // 一元运算符
            case '!': return "!";
            case '~': return "~";
            case 1024: return "++";  // prefix_increment
            case 1025: return "++";  // postfix_increment
            case 1026: return "--";  // prefix_decrement
            case 1027: return "--";  // postfix_decrement
            case 1028: return "&";
            case 1029: return "*";
            case 1030: return "(type)";
            case 1031: return "sizeof";
            case 1032: return "throw";
            case 1033: return "delete";
            case 1034: return "new";
            
            default: return "unknown";
        }
    }
    
    // ==================== 操作符类型描述 ====================
    
    // 获取操作符类型的描述信息
    inline std::string getOperatorTypeDescription(int op) {
        if (isAssignmentOperator(op)) {
            return "assignment";
        } else if (isAccessOperator(op)) {
            return "access";
        } else if (isSupportedBinaryOperator(op)) {
            return "binary";
        } else if (isSupportedUnaryOperator(op)) {
            return "unary";
        } else {
            return "unknown";
        }
    }
}

#endif // PARSER_OPERATOR_MAPPING_H 