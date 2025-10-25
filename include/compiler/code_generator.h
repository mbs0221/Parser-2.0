#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "parser/definition.h"
#include "parser/expression.h"
#include "parser/statement.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace Compiler {

// 代码生成器基类
class CodeGenerator {
public:
    virtual ~CodeGenerator() = default;
    
    // 生成代码
    virtual std::string generate() = 0;
    
    // 获取生成器类型
    virtual std::string getType() const = 0;
    
    // 设置选项
    virtual void setOption(const std::string& key, const std::string& value) = 0;
    
    // 获取选项
    virtual std::string getOption(const std::string& key) const = 0;
};

// C++代码生成器
class CppCodeGenerator : public CodeGenerator {
public:
    CppCodeGenerator();
    ~CppCodeGenerator();
    
    std::string generate() override;
    std::string getType() const override { return "C++"; }
    void setOption(const std::string& key, const std::string& value) override;
    std::string getOption(const std::string& key) const override;
    
    // C++特定方法
    void setTarget(const std::string& target);
    void setOptimizationLevel(int level);
    void setIncludePaths(const std::vector<std::string>& paths);
    void setDefines(const std::map<std::string, std::string>& defines);
    
    // 生成特定类型的代码
    std::string generateHeader();
    std::string generateSource();
    std::string generateClass(ClassDefinition* classDef);
    std::string generateStruct(StructDefinition* structDef);
    std::string generateFunction(FunctionDefinition* funcDef);
    std::string generateExpression(Expression* expr);
    std::string generateStatement(Statement* stmt);

private:
    std::map<std::string, std::string> options;
    std::string target;
    int optimizationLevel;
    std::vector<std::string> includePaths;
    std::map<std::string, std::string> defines;
    
    // 辅助方法
    std::string getCppType(Type* type);
    std::string getCppVisibility(const std::string& visibility);
    std::string indent(int level);
};

// JavaScript代码生成器
class JavaScriptCodeGenerator : public CodeGenerator {
public:
    JavaScriptCodeGenerator();
    ~JavaScriptCodeGenerator();
    
    std::string generate() override;
    std::string getType() const override { return "JavaScript"; }
    void setOption(const std::string& key, const std::string& value) override;
    std::string getOption(const std::string& key) const override;
    
    // JavaScript特定方法
    void setTarget(const std::string& target);
    void setModuleSystem(const std::string& moduleSystem);
    void setAsyncSupport(bool enabled);
    
    // 生成特定类型的代码
    std::string generateModule();
    std::string generateClass(ClassDefinition* classDef);
    std::string generateFunction(FunctionDefinition* funcDef);
    std::string generateExpression(Expression* expr);
    std::string generateStatement(Statement* stmt);

private:
    std::map<std::string, std::string> options;
    std::string target;
    std::string moduleSystem;
    bool asyncSupport;
    
    // 辅助方法
    std::string getJsType(Type* type);
    std::string indent(int level);
};

// Python代码生成器
class PythonCodeGenerator : public CodeGenerator {
public:
    PythonCodeGenerator();
    ~PythonCodeGenerator();
    
    std::string generate() override;
    std::string getType() const override { return "Python"; }
    void setOption(const std::string& key, const std::string& value) override;
    std::string getOption(const std::string& key) const override;
    
    // Python特定方法
    void setTarget(const std::string& target);
    void setPythonVersion(const std::string& version);
    void setTypeHints(bool enabled);
    
    // 生成特定类型的代码
    std::string generateModule();
    std::string generateClass(ClassDefinition* classDef);
    std::string generateFunction(FunctionDefinition* funcDef);
    std::string generateExpression(Expression* expr);
    std::string generateStatement(Statement* stmt);

private:
    std::map<std::string, std::string> options;
    std::string target;
    std::string pythonVersion;
    bool typeHints;
    
    // 辅助方法
    std::string getPythonType(Type* type);
    std::string indent(int level);
};

// 代码生成管理器
class CodeGenerationManager {
public:
    static CodeGenerationManager& getInstance();
    
    // 注册代码生成器
    void registerGenerator(const std::string& name, std::unique_ptr<CodeGenerator> generator);
    
    // 获取代码生成器
    CodeGenerator* getGenerator(const std::string& name);
    
    // 生成代码
    std::string generateCode(const std::string& generatorName, const std::vector<Definition*>& definitions);
    
    // 生成特定类型的代码
    std::string generateClass(const std::string& generatorName, ClassDefinition* classDef);
    std::string generateStruct(const std::string& generatorName, StructDefinition* structDef);
    std::string generateFunction(const std::string& generatorName, FunctionDefinition* funcDef);
    
    // 获取所有可用的生成器
    std::vector<std::string> getAvailableGenerators() const;
    
    // 设置默认生成器
    void setDefaultGenerator(const std::string& name);
    std::string getDefaultGenerator() const;

private:
    CodeGenerationManager();
    ~CodeGenerationManager();
    
    // 禁用拷贝构造和赋值
    CodeGenerationManager(const CodeGenerationManager&) = delete;
    CodeGenerationManager& operator=(const CodeGenerationManager&) = delete;
    
    std::map<std::string, std::unique_ptr<CodeGenerator>> generators;
    std::string defaultGenerator;
};

// 代码优化器
class CodeOptimizer {
public:
    CodeOptimizer();
    ~CodeOptimizer();
    
    // 优化代码
    std::string optimize(const std::string& code, const std::string& language);
    
    // 优化特定类型
    std::string optimizeExpression(Expression* expr);
    std::string optimizeStatement(Statement* stmt);
    std::string optimizeFunction(FunctionDefinition* funcDef);
    
    // 优化选项
    void enableConstantFolding(bool enabled);
    void enableDeadCodeElimination(bool enabled);
    void enableInlining(bool enabled);
    void enableLoopOptimization(bool enabled);

private:
    bool constantFolding;
    bool deadCodeElimination;
    bool inlining;
    bool loopOptimization;
    
    // 优化方法
    std::string foldConstants(const std::string& code);
    std::string eliminateDeadCode(const std::string& code);
    std::string inlineFunctions(const std::string& code);
    std::string optimizeLoops(const std::string& code);
};

// 代码分析器
class CodeAnalyzer {
public:
    CodeAnalyzer();
    ~CodeAnalyzer();
    
    // 分析代码
    struct AnalysisResult {
        int linesOfCode;
        int cyclomaticComplexity;
        int numberOfFunctions;
        int numberOfClasses;
        int numberOfVariables;
        std::vector<std::string> dependencies;
        std::vector<std::string> warnings;
        std::vector<std::string> errors;
    };
    
    AnalysisResult analyze(const std::string& code, const std::string& language);
    AnalysisResult analyzeAST(const std::vector<Definition*>& definitions);
    
    // 分析特定类型
    int analyzeComplexity(FunctionDefinition* funcDef);
    std::vector<std::string> analyzeDependencies(const std::vector<Definition*>& definitions);
    std::vector<std::string> findUnusedVariables(const std::vector<Definition*>& definitions);
    std::vector<std::string> findUnusedFunctions(const std::vector<Definition*>& definitions);

private:
    // 分析辅助方法
    int calculateCyclomaticComplexity(Statement* stmt);
    std::vector<std::string> extractDependencies(Expression* expr);
    std::vector<std::string> extractDependencies(Statement* stmt);
};

// 代码格式化器
class CodeFormatter {
public:
    CodeFormatter();
    ~CodeFormatter();
    
    // 格式化代码
    std::string format(const std::string& code, const std::string& language);
    
    // 格式化选项
    void setIndentSize(int size);
    void setIndentType(const std::string& type); // "spaces" or "tabs"
    void setLineLength(int length);
    void setBraceStyle(const std::string& style); // "allman", "k&r", "stroustrup", etc.

private:
    int indentSize;
    std::string indentType;
    int lineLength;
    std::string braceStyle;
    
    // 格式化方法
    std::string formatCpp(const std::string& code);
    std::string formatJavaScript(const std::string& code);
    std::string formatPython(const std::string& code);
};

} // namespace Compiler

#endif // CODE_GENERATOR_H
