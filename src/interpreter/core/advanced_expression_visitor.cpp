#include "interpreter/core/interpreter.h"
#include "parser/advanced_expressions.h"
#include "interpreter/values/value.h"
#include "interpreter/types/types.h"
#include "common/logger.h"

using namespace std;

// ==================== 字面量表达式访问实现 ====================

// 数组字面量表达式访问
Value* Interpreter::visit(ArrayLiteralExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问数组字面量表达式，元素数量: " + to_string(expr->getElementCount()));
    
    try {
        // 获取数组类型
        TypeRegistry* typeRegistry = TypeRegistry::getGlobalInstance();
        ObjectType* arrayType = typeRegistry->getType("array");
        
        if (!arrayType) {
            reportError("数组类型未注册");
            return nullptr;
        }
        
        // 创建数组值
        Array* arrayValue = new Array(arrayType);
        
        // 求值所有元素
        for (Expression* element : expr->getElements()) {
            if (element) {
                Value* elementValue = visit(element);
                if (elementValue) {
                    arrayValue->addElement(elementValue);
                } else {
                    reportError("数组元素求值失败");
                    delete arrayValue;
                    return nullptr;
                }
            }
        }
        
        LOG_DEBUG("数组字面量创建成功，包含 " + to_string(arrayValue->getSize()) + " 个元素");
        return arrayValue;
        
    } catch (const exception& e) {
        reportError("数组字面量求值错误: " + string(e.what()));
        return nullptr;
    }
}

// 字典字面量表达式访问
Value* Interpreter::visit(DictLiteralExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问字典字面量表达式，键值对数量: " + to_string(expr->getPairCount()));
    
    try {
        // 获取字典类型
        TypeRegistry* typeRegistry = TypeRegistry::getGlobalInstance();
        ObjectType* dictType = typeRegistry->getType("dict");
        
        if (!dictType) {
            reportError("字典类型未注册");
            return nullptr;
        }
        
        // 创建字典值
        Dict* dictValue = new Dict(dictType);
        
        // 求值所有键值对
        for (const auto& pair : expr->getPairs()) {
            if (pair.first && pair.second) {
                Value* keyValue = visit(pair.first);
                Value* valueValue = visit(pair.second);
                
                if (keyValue && valueValue) {
                    // 将键转换为字符串
                    string keyStr = keyValue->toString();
                    dictValue->setElement(keyStr, valueValue);
                } else {
                    reportError("字典键值对求值失败");
                    delete dictValue;
                    return nullptr;
                }
            }
        }
        
        LOG_DEBUG("字典字面量创建成功，包含 " + to_string(dictValue->getSize()) + " 个键值对");
        return dictValue;
        
    } catch (const exception& e) {
        reportError("字典字面量求值错误: " + string(e.what()));
        return nullptr;
    }
}

// 集合字面量表达式访问
Value* Interpreter::visit(SetLiteralExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问集合字面量表达式，元素数量: " + to_string(expr->getElementCount()));
    
    try {
        // 获取集合类型
        TypeRegistry* typeRegistry = TypeRegistry::getGlobalInstance();
        ObjectType* setType = typeRegistry->getType("set");
        
        if (!setType) {
            reportError("集合类型未注册");
            return nullptr;
        }
        
        // 创建集合值（使用Array作为底层实现）
        Array* setValue = new Array(setType);
        
        // 求值所有元素
        for (Expression* element : expr->getElements()) {
            if (element) {
                Value* elementValue = visit(element);
                if (elementValue) {
                    // 检查是否已存在（集合不允许重复元素）
                    bool exists = false;
                    for (size_t i = 0; i < setValue->getSize(); ++i) {
                        if (setValue->getElement(i)->equals(elementValue)) {
                            exists = true;
                            break;
                        }
                    }
                    if (!exists) {
                        setValue->addElement(elementValue);
                    }
                } else {
                    reportError("集合元素求值失败");
                    delete setValue;
                    return nullptr;
                }
            }
        }
        
        LOG_DEBUG("集合字面量创建成功，包含 " + to_string(setValue->getSize()) + " 个元素");
        return setValue;
        
    } catch (const exception& e) {
        reportError("集合字面量求值错误: " + string(e.what()));
        return nullptr;
    }
}

// ==================== 范围表达式访问实现 ====================

// 范围表达式访问
Value* Interpreter::visit(RangeExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问范围表达式");
    
    try {
        // 求值起始和结束值
        Value* startValue = visit(expr->getStart());
        Value* endValue = visit(expr->getEnd());
        
        if (!startValue || !endValue) {
            reportError("范围表达式求值失败");
            return nullptr;
        }
        
        // 转换为整数
        Value* startInt = calculator->tryConvertValue(startValue, "int");
        Value* endInt = calculator->tryConvertValue(endValue, "int");
        
        if (!startInt || !endInt) {
            reportError("范围表达式需要整数类型");
            return nullptr;
        }
        
        int start = dynamic_cast<Integer*>(startInt)->getValue();
        int end = dynamic_cast<Integer*>(endInt)->getValue();
        
        // 创建范围数组
        TypeRegistry* typeRegistry = TypeRegistry::getGlobalInstance();
        ObjectType* arrayType = typeRegistry->getType("array");
        Array* rangeArray = new Array(arrayType);
        
        // 生成范围值
        if (expr->isInclusive()) {
            for (int i = start; i <= end; ++i) {
                rangeArray->addElement(new Integer(i));
            }
        } else {
            for (int i = start; i < end; ++i) {
                rangeArray->addElement(new Integer(i));
            }
        }
        
        LOG_DEBUG("范围表达式创建成功，范围: " + to_string(start) + 
                 (expr->isInclusive() ? ".." : "..<") + to_string(end));
        return rangeArray;
        
    } catch (const exception& e) {
        reportError("范围表达式求值错误: " + string(e.what()));
        return nullptr;
    }
}

// ==================== 切片表达式访问实现 ====================

// 切片表达式访问
Value* Interpreter::visit(SliceExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问切片表达式");
    
    try {
        // 求值目标对象
        Value* targetValue = visit(expr->getTarget());
        if (!targetValue) {
            reportError("切片目标求值失败");
            return nullptr;
        }
        
        // 检查目标是否支持切片操作
        if (targetValue->getTypeName() != "array" && targetValue->getTypeName() != "string") {
            reportError("切片操作只支持数组和字符串类型");
            return nullptr;
        }
        
        // 求值切片参数
        int start = 0, end = 0, step = 1;
        
        if (expr->getStart()) {
            Value* startValue = visit(expr->getStart());
            if (startValue) {
                Value* startInt = calculator->tryConvertValue(startValue, "int");
                if (startInt) {
                    start = dynamic_cast<Integer*>(startInt)->getValue();
                }
            }
        }
        
        if (expr->getEnd()) {
            Value* endValue = visit(expr->getEnd());
            if (endValue) {
                Value* endInt = calculator->tryConvertValue(endValue, "int");
                if (endInt) {
                    end = dynamic_cast<Integer*>(endInt)->getValue();
                }
            }
        }
        
        if (expr->getStep()) {
            Value* stepValue = visit(expr->getStep());
            if (stepValue) {
                Value* stepInt = calculator->tryConvertValue(stepValue, "int");
                if (stepInt) {
                    step = dynamic_cast<Integer*>(stepInt)->getValue();
                }
            }
        }
        
        // 执行切片操作
        if (targetValue->getTypeName() == "array") {
            Array* arrayValue = dynamic_cast<Array*>(targetValue);
            return arrayValue->slice(start, end);
        } else if (targetValue->getTypeName() == "string") {
            String* stringValue = dynamic_cast<String*>(targetValue);
            return stringValue->slice(start, end);
        }
        
        return nullptr;
        
    } catch (const exception& e) {
        reportError("切片表达式求值错误: " + string(e.what()));
        return nullptr;
    }
}

// ==================== 条件表达式扩展访问实现 ====================

// 空值合并表达式访问
Value* Interpreter::visit(NullCoalescingExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问空值合并表达式");
    
    try {
        // 求值左操作数
        Value* leftValue = visit(expr->left);
        if (!leftValue) {
            reportError("空值合并表达式左操作数求值失败");
            return nullptr;
        }
        
        // 检查左操作数是否为空
        if (leftValue->getTypeName() == "null" || 
            (leftValue->getTypeName() == "string" && dynamic_cast<String*>(leftValue)->getValue().empty())) {
            // 左操作数为空，返回右操作数
            return visit(expr->right);
        } else {
            // 左操作数不为空，返回左操作数
            return leftValue;
        }
        
    } catch (const exception& e) {
        reportError("空值合并表达式求值错误: " + string(e.what()));
        return nullptr;
    }
}

// ==================== 模式匹配表达式访问实现 ====================

// 模式匹配表达式访问
Value* Interpreter::visit(MatchExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问模式匹配表达式，模式数量: " + to_string(expr->getPatterns().size()));
    
    try {
        // 求值匹配值
        Value* matchValue = visit(expr->getValue());
        if (!matchValue) {
            reportError("模式匹配表达式求值失败");
            return nullptr;
        }
        
        // 遍历所有模式
        for (const auto& pattern : expr->getPatterns()) {
            if (pattern.first && pattern.second) {
                // 求值模式
                Value* patternValue = visit(pattern.first);
                if (patternValue) {
                    // 检查是否匹配
                    if (matchValue->equals(patternValue)) {
                        // 匹配成功，返回结果
                        return visit(pattern.second);
                    }
                }
            }
        }
        
        // 没有匹配的模式
        reportError("模式匹配失败，没有匹配的模式");
        return nullptr;
        
    } catch (const exception& e) {
        reportError("模式匹配表达式求值错误: " + string(e.what()));
        return nullptr;
    }
}

// ==================== 异步表达式访问实现 ====================

// 异步表达式访问
Value* Interpreter::visit(AsyncExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问异步表达式");
    
    try {
        // 异步表达式在当前实现中直接求值
        // 在真正的异步实现中，这里应该返回一个Promise或Future对象
        Value* result = visit(expr->getExpression());
        
        if (result) {
            LOG_DEBUG("异步表达式求值成功");
            return result;
        } else {
            reportError("异步表达式求值失败");
            return nullptr;
        }
        
    } catch (const exception& e) {
        reportError("异步表达式求值错误: " + string(e.what()));
        return nullptr;
    }
}

// 等待表达式访问
Value* Interpreter::visit(AwaitExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问等待表达式");
    
    try {
        // 等待表达式在当前实现中直接求值
        // 在真正的异步实现中，这里应该等待Promise或Future完成
        Value* result = visit(expr->getExpression());
        
        if (result) {
            LOG_DEBUG("等待表达式求值成功");
            return result;
        } else {
            reportError("等待表达式求值失败");
            return nullptr;
        }
        
    } catch (const exception& e) {
        reportError("等待表达式求值错误: " + string(e.what()));
        return nullptr;
    }
}

// ==================== 类型检查表达式访问实现 ====================

// 类型检查表达式访问
Value* Interpreter::visit(TypeCheckExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问类型检查表达式");
    
    try {
        // 求值左操作数（值）
        Value* value = visit(expr->left);
        if (!value) {
            reportError("类型检查表达式值求值失败");
            return nullptr;
        }
        
        // 求值右操作数（类型）
        Value* typeValue = visit(expr->right);
        if (!typeValue) {
            reportError("类型检查表达式类型求值失败");
            return nullptr;
        }
        
        // 获取类型名称
        string typeName;
        if (typeValue->getTypeName() == "string") {
            typeName = dynamic_cast<String*>(typeValue)->getValue();
        } else {
            typeName = typeValue->getTypeName();
        }
        
        // 检查类型是否匹配
        bool isMatch = (value->getTypeName() == typeName);
        
        LOG_DEBUG("类型检查结果: " + value->getTypeName() + " is " + typeName + " = " + (isMatch ? "true" : "false"));
        return new Bool(isMatch);
        
    } catch (const exception& e) {
        reportError("类型检查表达式求值错误: " + string(e.what()));
        return nullptr;
    }
}

// 类型转换表达式访问
Value* Interpreter::visit(TypeCastExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问类型转换表达式");
    
    try {
        // 求值左操作数（值）
        Value* value = visit(expr->left);
        if (!value) {
            reportError("类型转换表达式值求值失败");
            return nullptr;
        }
        
        // 求值右操作数（类型）
        Value* typeValue = visit(expr->right);
        if (!typeValue) {
            reportError("类型转换表达式类型求值失败");
            return nullptr;
        }
        
        // 获取类型名称
        string typeName;
        if (typeValue->getTypeName() == "string") {
            typeName = dynamic_cast<String*>(typeValue)->getValue();
        } else {
            typeName = typeValue->getTypeName();
        }
        
        // 执行类型转换
        Value* convertedValue = calculator->tryConvertValue(value, typeName);
        
        if (convertedValue) {
            LOG_DEBUG("类型转换成功: " + value->getTypeName() + " as " + typeName);
            return convertedValue;
        } else {
            reportError("类型转换失败: " + value->getTypeName() + " as " + typeName);
            return nullptr;
        }
        
    } catch (const exception& e) {
        reportError("类型转换表达式求值错误: " + string(e.what()));
        return nullptr;
    }
}

// ==================== 复合赋值表达式访问实现 ====================

// 复合赋值表达式访问
Value* Interpreter::visit(CompoundAssignExpression* expr) {
    if (!expr) return nullptr;
    
    LOG_DEBUG("访问复合赋值表达式: " + expr->getOperation());
    
    try {
        // 求值左操作数（变量）
        Value* leftValue = visit(expr->left);
        if (!leftValue) {
            reportError("复合赋值表达式左操作数求值失败");
            return nullptr;
        }
        
        // 求值右操作数（值）
        Value* rightValue = visit(expr->right);
        if (!rightValue) {
            reportError("复合赋值表达式右操作数求值失败");
            return nullptr;
        }
        
        // 根据操作类型执行复合赋值
        string operation = expr->getOperation();
        Value* result = nullptr;
        
        if (operation == "+=") {
            result = calculator->executeBinaryOperation(leftValue, rightValue, 
                new Operator("+", '+', 1, 1, 1));
        } else if (operation == "-=") {
            result = calculator->executeBinaryOperation(leftValue, rightValue, 
                new Operator("-", '-', 1, 1, 1));
        } else if (operation == "*=") {
            result = calculator->executeBinaryOperation(leftValue, rightValue, 
                new Operator("*", '*', 1, 1, 1));
        } else if (operation == "/=") {
            result = calculator->executeBinaryOperation(leftValue, rightValue, 
                new Operator("/", '/', 1, 1, 1));
        } else if (operation == "%=") {
            result = calculator->executeBinaryOperation(leftValue, rightValue, 
                new Operator("%", '%', 1, 1, 1));
        } else {
            reportError("不支持的复合赋值操作: " + operation);
            return nullptr;
        }
        
        if (result) {
            LOG_DEBUG("复合赋值表达式执行成功: " + operation);
            return result;
        } else {
            reportError("复合赋值表达式执行失败: " + operation);
            return nullptr;
        }
        
    } catch (const exception& e) {
        reportError("复合赋值表达式求值错误: " + string(e.what()));
        return nullptr;
    }
}

