#ifndef EXPRESSION_SPECIALIZATIONS_H
#define EXPRESSION_SPECIALIZATIONS_H

#include "parser/expression.h"

// 模板特化声明（实现在源文件中）
template<> std::string ConstantExpression<int>::getLocation() const;
template<> std::string ConstantExpression<double>::getLocation() const;
template<> std::string ConstantExpression<bool>::getLocation() const;
template<> std::string ConstantExpression<char>::getLocation() const;
template<> std::string ConstantExpression<std::string>::getLocation() const;

template<> std::string ConstantExpression<int>::getTypeName() const;
template<> std::string ConstantExpression<double>::getTypeName() const;
template<> std::string ConstantExpression<bool>::getTypeName() const;
template<> std::string ConstantExpression<char>::getTypeName() const;
template<> std::string ConstantExpression<std::string>::getTypeName() const;

#endif // EXPRESSION_SPECIALIZATIONS_H
