#ifndef SAFE_CALCULATOR_H
#define SAFE_CALCULATOR_H

#include "interpreter/core/memory_manager.h"
#include "interpreter/values/calculate.h"
#include "interpreter/values/value.h"
#include "parser/expression.h"
#include "lexer/token.h"

namespace InterpreterCore {

// 内存安全的计算器包装器
class SafeCalculator {
private:
    Calculator* calculator;
    MemoryManager& memoryManager;
    
public:
    explicit SafeCalculator(Calculator* calc) 
        : calculator(calc), memoryManager(MemoryManager::getInstance()) {}
    
    // 内存安全的二元运算
    ValuePtr<Value> executeBinaryOperation(Value* left, Value* right, Operator* op) {
        if (!left || !right || !op) {
            return ValuePtr<Value>(nullptr);
        }
        
        try {
            Value* result = calculator->executeBinaryOperation(left, right, op);
            if (result) {
                return ValuePtr<Value>(result, true);
            }
        } catch (const std::exception& e) {
            // 记录错误但不抛出异常
            std::cerr << "二元运算错误: " << e.what() << std::endl;
        }
        
        return ValuePtr<Value>(nullptr);
    }
    
    // 内存安全的一元运算
    ValuePtr<Value> executeUnaryOperation(Value* operand, Operator* op) {
        if (!operand || !op) {
            return ValuePtr<Value>(nullptr);
        }
        
        try {
            Value* result = calculator->executeUnaryOperation(operand, op);
            if (result) {
                return ValuePtr<Value>(result, true);
            }
        } catch (const std::exception& e) {
            // 记录错误但不抛出异常
            std::cerr << "一元运算错误: " << e.what() << std::endl;
        }
        
        return ValuePtr<Value>(nullptr);
    }
    
    // 内存安全的三元运算
    ValuePtr<Value> executeTernaryOperation(Value* condition, Value* trueValue, Value* falseValue) {
        if (!condition || !trueValue || !falseValue) {
            return ValuePtr<Value>(nullptr);
        }
        
        try {
            Value* result = calculator->executeTernaryOperation(condition, trueValue, falseValue);
            if (result) {
                return ValuePtr<Value>(result, true);
            }
        } catch (const std::exception& e) {
            // 记录错误但不抛出异常
            std::cerr << "三元运算错误: " << e.what() << std::endl;
        }
        
        return ValuePtr<Value>(nullptr);
    }
    
    // 内存安全的类型转换
    ValuePtr<Value> tryConvertValue(Value* sourceValue, const std::string& targetTypeName) {
        if (!sourceValue) {
            return ValuePtr<Value>(nullptr);
        }
        
        try {
            Value* result = calculator->tryConvertValue(sourceValue, targetTypeName);
            if (result) {
                return ValuePtr<Value>(result, true);
            }
        } catch (const std::exception& e) {
            // 记录错误但不抛出异常
            std::cerr << "类型转换错误: " << e.what() << std::endl;
        }
        
        return ValuePtr<Value>(nullptr);
    }
    
    // 获取底层计算器
    Calculator* getCalculator() const { return calculator; }
};

// 内存安全的表达式求值器
class SafeExpressionEvaluator {
private:
    SafeCalculator safeCalculator;
    InterpreterCore::ErrorHandler* errorHandler;
    
public:
    SafeExpressionEvaluator(Calculator* calc, InterpreterCore::ErrorHandler* handler = nullptr)
        : safeCalculator(calc), errorHandler(handler) {}
    
    // 安全求值表达式
    ValuePtr<Value> evaluate(Expression* expr) {
        if (!expr) {
            if (errorHandler) {
                errorHandler->reportError("表达式为空");
            }
            return ValuePtr<Value>(nullptr);
        }
        
        try {
            // 这里需要访问Interpreter的visit方法
            // 由于循环依赖问题，这里提供一个接口
            return ValuePtr<Value>(nullptr); // 占位符
        } catch (const std::exception& e) {
            if (errorHandler) {
                errorHandler->reportError("表达式求值错误: " + std::string(e.what()));
            }
            return ValuePtr<Value>(nullptr);
        }
    }
    
    // 安全求值常量表达式
    template<typename T>
    ValuePtr<Value> evaluateConstant(const T& value) {
        try {
            if constexpr (std::is_same_v<T, int>) {
                return makeValue<Integer>(value);
            } else if constexpr (std::is_same_v<T, double>) {
                return makeValue<Double>(value);
            } else if constexpr (std::is_same_v<T, bool>) {
                return makeValue<Bool>(value);
            } else if constexpr (std::is_same_v<T, char>) {
                return makeValue<Char>(value);
            } else if constexpr (std::is_same_v<T, std::string>) {
                return makeValue<String>(value);
            } else {
                return ValuePtr<Value>(nullptr);
            }
        } catch (const std::exception& e) {
            if (errorHandler) {
                errorHandler->reportError("常量求值错误: " + std::string(e.what()));
            }
            return ValuePtr<Value>(nullptr);
        }
    }
};

// 内存池管理器 - 用于高效分配常用类型
class ValuePoolManager {
private:
    static ValuePoolManager* instance;
    static std::mutex instanceMutex;
    
    ValuePool<Integer> intPool;
    ValuePool<Double> doublePool;
    ValuePool<Bool> boolPool;
    ValuePool<String> stringPool;
    ValuePool<Char> charPool;
    
    ValuePoolManager() : intPool(1000), doublePool(1000), boolPool(1000), 
                        stringPool(1000), charPool(1000) {}
    
public:
    static ValuePoolManager& getInstance() {
        std::lock_guard<std::mutex> lock(instanceMutex);
        if (!instance) {
            instance = new ValuePoolManager();
        }
        return *instance;
    }
    
    // 获取和释放Integer
    Integer* acquireInteger(int value = 0) {
        Integer* ptr = intPool.acquire();
        *ptr = value;
        return ptr;
    }
    
    void releaseInteger(Integer* ptr) {
        intPool.release(ptr);
    }
    
    // 获取和释放Double
    Double* acquireDouble(double value = 0.0) {
        Double* ptr = doublePool.acquire();
        *ptr = value;
        return ptr;
    }
    
    void releaseDouble(Double* ptr) {
        doublePool.release(ptr);
    }
    
    // 获取和释放Bool
    Bool* acquireBool(bool value = false) {
        Bool* ptr = boolPool.acquire();
        *ptr = value;
        return ptr;
    }
    
    void releaseBool(Bool* ptr) {
        boolPool.release(ptr);
    }
    
    // 获取和释放String
    String* acquireString(const std::string& value = "") {
        String* ptr = stringPool.acquire();
        *ptr = value;
        return ptr;
    }
    
    void releaseString(String* ptr) {
        stringPool.release(ptr);
    }
    
    // 获取和释放Char
    Char* acquireChar(char value = '\0') {
        Char* ptr = charPool.acquire();
        *ptr = value;
        return ptr;
    }
    
    void releaseChar(Char* ptr) {
        charPool.release(ptr);
    }
    
    // 获取池统计信息
    std::string getPoolStatistics() const {
        std::ostringstream oss;
        oss << "内存池统计:" << std::endl;
        oss << "  Integer池: " << intPool.size() << " 个对象" << std::endl;
        oss << "  Double池: " << doublePool.size() << " 个对象" << std::endl;
        oss << "  Bool池: " << boolPool.size() << " 个对象" << std::endl;
        oss << "  String池: " << stringPool.size() << " 个对象" << std::endl;
        oss << "  Char池: " << charPool.size() << " 个对象" << std::endl;
        return oss.str();
    }
};

} // namespace InterpreterCore

#endif // SAFE_CALCULATOR_H
