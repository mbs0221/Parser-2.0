#ifndef FLEX_BISON_PARSER_H
#define FLEX_BISON_PARSER_H

#include <string>
#include <memory>
#include "parser/ast.h"
#include "parser/definition.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include "parser/advanced_expressions.h"

/**
 * Flex/Bison Parser包装类
 * 提供与递归下降Parser相同的接口，但使用flex和bison实现
 */
class FlexBisonParser {
private:
    std::string sourceCode;
    std::string fileName;
    
    // 解析结果
    std::unique_ptr<Program> program;
    
    // 错误信息
    std::string errorMessage;
    bool hasError;
    
public:
    FlexBisonParser();
    ~FlexBisonParser();
    
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
    
private:
    /**
     * 初始化flex/bison解析器
     */
    void initializeParser();
    
    /**
     * 清理flex/bison解析器
     */
    void cleanupParser();
    
    /**
     * 设置错误信息
     * @param message 错误信息
     */
    void setError(const std::string& message);
};

#endif // FLEX_BISON_PARSER_H
