#ifndef UNIFIED_PARSER_H
#define UNIFIED_PARSER_H

#include <string>
#include <memory>
#include "parser/ast.h"
#include "parser/definition.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include "parser/advanced_expressions.h"

/**
 * 统一Parser接口
 * 支持递归下降和flex/bison两种实现方式
 */
class UnifiedParser {
public:
    /**
     * Parser实现类型
     */
    enum ParserType {
        RECURSIVE_DESCENT,  // 递归下降解析器
        FLEX_BISON         // Flex/Bison解析器
    };
    
private:
    ParserType parserType;
    std::string sourceCode;
    std::string fileName;
    
    // 解析结果
    std::unique_ptr<Program> program;
    
    // 错误信息
    std::string errorMessage;
    bool hasError;
    
    // 当前行号和列号
    int currentLine;
    int currentColumn;
    
public:
    /**
     * 构造函数
     * @param type Parser实现类型
     */
    explicit UnifiedParser(ParserType type = RECURSIVE_DESCENT);
    
    /**
     * 析构函数
     */
    ~UnifiedParser();
    
    /**
     * 设置Parser类型
     * @param type Parser实现类型
     */
    void setParserType(ParserType type);
    
    /**
     * 获取当前Parser类型
     * @return Parser实现类型
     */
    ParserType getParserType() const;
    
    /**
     * 解析源代码
     * @param source 源代码字符串
     * @param filename 文件名（用于错误报告）
     * @return 解析是否成功
     */
    bool parse(const std::string& source, const std::string& filename = "");
    
    /**
     * 从文件解析
     * @param filename 文件名
     * @return 解析是否成功
     */
    bool parseFile(const std::string& filename);
    
    /**
     * 获取解析结果
     * @return 解析得到的Program AST节点
     */
    Program* getProgram() const;
    
    /**
     * 获取错误信息
     * @return 错误信息字符串
     */
    const std::string& getErrorMessage() const;
    
    /**
     * 检查是否有错误
     * @return 是否有错误
     */
    bool hasErrors() const;
    
    /**
     * 清除错误状态
     */
    void clearErrors();
    
    /**
     * 获取当前行号（用于错误报告）
     * @return 当前行号
     */
    int getCurrentLine() const;
    
    /**
     * 获取当前列号（用于错误报告）
     * @return 当前列号
     */
    int getCurrentColumn() const;
    
    /**
     * 获取Parser类型名称
     * @return Parser类型名称字符串
     */
    std::string getParserTypeName() const;
    
    /**
     * 比较两种Parser的解析结果
     * @param other 另一个Parser实例
     * @return 解析结果是否相同
     */
    bool compareWith(const UnifiedParser& other) const;
    
private:
    /**
     * 使用递归下降Parser解析
     * @param source 源代码
     * @param filename 文件名
     * @return 解析是否成功
     */
    bool parseWithRecursiveDescent(const std::string& source, const std::string& filename);
    
    /**
     * 使用Flex/Bison Parser解析
     * @param source 源代码
     * @param filename 文件名
     * @return 解析是否成功
     */
    bool parseWithFlexBison(const std::string& source, const std::string& filename);
    
    /**
     * 设置错误信息
     * @param message 错误信息
     */
    void setError(const std::string& message);
    
    /**
     * 设置位置信息
     * @param line 行号
     * @param column 列号
     */
    void setPosition(int line, int column);
};

#endif // UNIFIED_PARSER_H
