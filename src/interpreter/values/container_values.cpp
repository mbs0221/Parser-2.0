#include <iostream>
#include <algorithm>

#include "interpreter/values/value.h"
#include "interpreter/types/types.h"

using namespace std;

// ==================== ContainerValue基类实现 ====================
ContainerValue::ContainerValue(ObjectType* vt, bool mutable_, bool ordered) 
    : Value(vt), isMutable(mutable_), isOrdered(ordered) {
}

bool ContainerValue::isMutableContainer() const {
    return isMutable;
}

bool ContainerValue::isOrderedContainer() const {
    return isOrdered;
}

// ==================== Array类实现 ====================

Array::Array(ObjectType* et, ObjectType* vt) 
    : ContainerValue(vt, true, true), elementType(et) {
    // 数组默认是可变的容器类型
}

Array::Array(const vector<Value*>& elems, ObjectType* et, ObjectType* vt)
    : ContainerValue(vt, true, true), elementType(et) {
    // 复制元素
    for (Value* elem : elems) {
        if (elem) {
            elements.push_back(elem->clone());
        }
    }
}

Array::Array(const Array& other) 
    : ContainerValue(other), elementType(other.elementType) {
    // 深度复制元素
    for (Value* elem : other.elements) {
        if (elem) {
            elements.push_back(elem->clone());
        }
    }
}

Array::~Array() {
    // 清理元素
    for (Value* elem : elements) {
        delete elem;
    }
    elements.clear();
}

// 赋值运算符重载
Array Array::operator=(const Array& other) {
    // 清理现有元素
    for (Value* elem : elements) {
        delete elem;
    }
    elements.clear();
    
    // 复制新元素
    for (Value* elem : other.elements) {
        if (elem) {
            elements.push_back(elem->clone());
        }
    }
    
    elementType = other.elementType;
    return *this;
}

// 容器操作实现
size_t Array::getSize() const {
    return elements.size();
}

bool Array::isEmpty() const {
    return elements.empty();
}

void Array::clear() {
    for (Value* elem : elements) {
        delete elem;
    }
    elements.clear();
}

// 容器类型支持
bool Array::supportsElementType() const {
    return elementType != nullptr;
}

ObjectType* Array::getElementType() const {
    return elementType;
}

// 迭代器支持
Value* Array::begin() const {
    if (elements.empty()) return nullptr;
    return new Integer(0); // 返回索引作为迭代器
}

Value* Array::end() const {
    return new Integer(elements.size()); // 返回大小作为结束标记
}

Value* Array::next(Value* iterator) const {
    if (Integer* idx = dynamic_cast<Integer*>(iterator)) {
        int nextIdx = idx->getValue() + 1;
        if (nextIdx < static_cast<int>(elements.size())) {
            return new Integer(nextIdx);
        }
    }
    return nullptr;
}

// 容量管理
void Array::reserve(size_t capacity) {
    elements.reserve(capacity);
}

size_t Array::getCapacity() const {
    return elements.capacity();
}

// 数组特有操作
void Array::addElement(Value* element) {
    if (element) {
        elements.push_back(element->clone());
    }
}

void Array::setElement(size_t index, Value* element) {
    if (index < elements.size() && element) {
        delete elements[index];
        elements[index] = element->clone();
    }
}

const vector<Value*>& Array::getElements() const {
    return elements;
}

vector<Value*>& Array::getElements() {
    return elements;
}

void Array::setElementType(ObjectType* et) {
    elementType = et;
}

Value* Array::getElement(size_t index) const {
    if (index < elements.size()) {
        return elements[index] ? elements[index]->clone() : nullptr;
    }
    return nullptr;
}

void Array::removeElement(size_t index) {
    if (index < elements.size()) {
        delete elements[index];
        elements.erase(elements.begin() + index);
    }
}

void Array::insertElement(size_t index, Value* element) {
    if (index <= elements.size() && element) {
        elements.insert(elements.begin() + index, element->clone());
    }
}







// 下标运算符重载
Value* Array::operator[](const Integer& index) const {
    int i = index.getValue();
    if (i >= 0 && static_cast<size_t>(i) < elements.size()) {
        return elements[i] ? elements[i]->clone() : nullptr;
    }
    return nullptr;
}

// 重写基类方法
string Array::toString() const {
    string result = "[";
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i > 0) result += ", ";
        if (elements[i]) {
            result += elements[i]->toString();
        } else {
            result += "null";
        }
    }
    result += "]";
    return result;
}

bool Array::toBool() const {
    return !elements.empty();
}

Value* Array::clone() const {
    return new Array(*this);
}

string Array::getBuiltinTypeName() const {
    return "array";
}

// 数组函数
Value* Array::sort() const {
    Array* result = new Array(*this);
    // 这里需要实现排序逻辑
    // 简化实现：返回副本
    return result;
}

Value* Array::reverse() const {
    Array* result = new Array(*this);
    std::reverse(result->elements.begin(), result->elements.end());
    return result;
}

Value* Array::slice(int start, int end) const {
    Array* result = new Array(elementType);
    
    if (start < 0) start = 0;
    if (end > static_cast<int>(elements.size())) end = elements.size();
    
    for (int i = start; i < end; ++i) {
        if (elements[i]) {
            result->addElement(elements[i]);
        }
    }
    
    return result;
}

Value* Array::filter(Value* predicate) const {
    Array* result = new Array(elementType);
    
    for (Value* elem : elements) {
        if (elem && predicate) {
            // 这里需要调用predicate函数
            // 简化实现：添加所有元素
            result->addElement(elem);
        }
    }
    
    return result;
}

Value* Array::map(Value* mapper) const {
    Array* result = new Array(elementType);
    
    for (Value* elem : elements) {
        if (elem && mapper) {
            // 这里需要调用mapper函数
            // 简化实现：添加所有元素
            result->addElement(elem);
        }
    }
    
    return result;
}

Value* Array::indexOf(Value* element) const {
    if (!element) {
        return new Integer(-1);
    }
    
    // 遍历数组元素，查找匹配的元素
    for (size_t i = 0; i < elements.size(); ++i) {
        if (elements[i]) {
            // 比较元素值（使用toString进行比较）
            if (elements[i]->toString() == element->toString()) {
                return new Integer(i);
            }
        }
    }
    
    // 未找到匹配的元素
    return new Integer(-1);
}

Value* Array::contains(Value* element) const {
    if (!element) {
        return new Bool(false);
    }
    
    // 遍历数组元素，检查是否包含指定元素
    for (Value* elem : elements) {
        if (elem) {
            // 比较元素值（使用toString进行比较）
            if (elem->toString() == element->toString()) {
                return new Bool(true);
            }
        }
    }
    
    // 未找到匹配的元素
    return new Bool(false);
}



// ==================== Dict实现 ====================

Dict::Dict(ObjectType* kt, ObjectType* vt) 
    : ContainerValue(kt, true, true), keyType(kt), valueType(vt) {
}

Dict::Dict(const map<string, Value*>& ents, ObjectType* kt, ObjectType* vt)
    : ContainerValue(kt, true, true), entries(ents), keyType(kt), valueType(vt) {
}

Dict::Dict(const Dict& other) 
    : ContainerValue(other), entries(), keyType(other.keyType), valueType(other.valueType) {
    // 深拷贝所有条目
    for (const auto& pair : other.entries) {
        if (pair.second) {
            entries[pair.first] = pair.second->clone();
        }
    }
}

Dict::~Dict() {
    // 清理所有条目
    for (auto& pair : entries) {
        if (pair.second) {
            delete pair.second;
        }
    }
    entries.clear();
}

// 赋值运算符重载
Dict Dict::operator=(const Dict& other) {
    if (this != &other) {
        // 清理当前条目
        for (auto& pair : entries) {
            if (pair.second) {
                delete pair.second;
            }
        }
        entries.clear();
        
        // 深拷贝新条目
        for (const auto& pair : other.entries) {
            if (pair.second) {
                entries[pair.first] = pair.second->clone();
            }
        }
        
        keyType = other.keyType;
        valueType = other.valueType;
    }
    return *this;
}

// 容器操作实现
size_t Dict::getSize() const {
    return entries.size();
}

bool Dict::isEmpty() const {
    return entries.empty();
}

void Dict::clear() {
    for (auto& pair : entries) {
        if (pair.second) {
            delete pair.second;
        }
    }
    entries.clear();
}

// 容器类型支持
bool Dict::supportsElementType() const {
    return true;
}

ObjectType* Dict::getElementType() const {
    return valueType;
}

// 迭代器支持
Value* Dict::begin() const {
    // 返回第一个键值对的迭代器
    if (!entries.empty()) {
        return new String(entries.begin()->first);
    }
    return nullptr;
}

Value* Dict::end() const {
    // 返回结束迭代器
    return nullptr;
}

Value* Dict::next(Value* iterator) const {
    // 简单的迭代器实现
    if (String* key = dynamic_cast<String*>(iterator)) {
        auto it = entries.find(key->getValue());
        if (it != entries.end()) {
            ++it;
            if (it != entries.end()) {
                return new String(it->first);
            }
        }
    }
    return nullptr;
}

// 容量管理（字典通常不需要预分配）
void Dict::reserve(size_t capacity) {
    // 字典不需要预分配容量
}

size_t Dict::getCapacity() const {
    // 字典没有容量概念
    return entries.size();
}

// 字典特有操作
void Dict::setEntry(const string& key, Value* value) {
    // 如果键已存在，先删除旧值
    auto it = entries.find(key);
    if (it != entries.end() && it->second) {
        delete it->second;
    }
    entries[key] = value;
}

Value* Dict::getEntry(const string& key) const {
    auto it = entries.find(key);
    return (it != entries.end()) ? it->second : nullptr;
}

bool Dict::hasKey(const string& key) const {
    return entries.find(key) != entries.end();
}

void Dict::removeEntry(const string& key) {
    auto it = entries.find(key);
    if (it != entries.end()) {
        if (it->second) {
            delete it->second;
        }
        entries.erase(it);
    }
}

// 字典运算已移除

// 重写基类方法
string Dict::toString() const {
    string result = "{";
    bool first = true;
    for (const auto& pair : entries) {
        if (!first) result += ", ";
        result += "\"" + pair.first + "\": ";
        if (pair.second) {
            result += pair.second->toString();
        } else {
            result += "null";
        }
        first = false;
    }
    result += "}";
    return result;
}

bool Dict::toBool() const {
    return !entries.empty();
}

Value* Dict::clone() const {
    return new Dict(*this);
}

string Dict::getBuiltinTypeName() const {
    return "dict";
}

// 获取条目引用
const map<string, Value*>& Dict::getEntries() const {
    return entries;
}

// 获取键列表
vector<string> Dict::getKeys() const {
    vector<string> keys;
    for (const auto& pair : entries) {
        keys.push_back(pair.first);
    }
    return keys;
}

// 获取值列表
vector<Value*> Dict::getValues() const {
    vector<Value*> values;
    for (const auto& pair : entries) {
        values.push_back(pair.second);
    }
    return values;
}



// ==================== Array类型运算符重载实现 ====================

// 比较运算符重载
Bool Array::operator==(const Array& other) const {
    if (elements.size() != other.elements.size()) {
        return Bool(false);
    }
    
    for (size_t i = 0; i < elements.size(); ++i) {
        if (!elements[i] || !other.elements[i]) {
            if (elements[i] != other.elements[i]) {
                return Bool(false);
            }
            continue;
        }
        
        // 递归比较元素
        if (elements[i]->toString() != other.elements[i]->toString()) {
            return Bool(false);
        }
    }
    
    return Bool(true);
}

Bool Array::operator!=(const Array& other) const {
    Bool eqResult = operator==(other);
    return Bool(!eqResult.toBool());
}

Bool Array::operator<(const Array& other) const {
    size_t minSize = std::min(elements.size(), other.elements.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        if (!elements[i] || !other.elements[i]) {
            if (elements[i] != other.elements[i]) {
                return Bool(elements[i] < other.elements[i]);
            }
            continue;
        }
        
        // 比较元素
        if (elements[i]->toString() < other.elements[i]->toString()) {
            return Bool(true);
        } else if (elements[i]->toString() > other.elements[i]->toString()) {
            return Bool(false);
        }
    }
    
    // 如果前面都相等，比较长度
    return Bool(elements.size() < other.elements.size());
}

Bool Array::operator<=(const Array& other) const {
    Bool ltResult = operator<(other);
    Bool eqResult = operator==(other);
    return Bool(ltResult.toBool() || eqResult.toBool());
}

Bool Array::operator>(const Array& other) const {
    Bool leResult = operator<=(other);
    return Bool(!leResult.toBool());
}

Bool Array::operator>=(const Array& other) const {
    Bool ltResult = operator<(other);
    return Bool(!ltResult.toBool());
}

// ==================== Dict类型运算符重载实现 ====================

// 比较运算符重载
Bool Dict::operator==(const Dict& other) const {
    if (entries.size() != other.entries.size()) {
        return Bool(false);
    }
    
    for (const auto& pair : entries) {
        auto it = other.entries.find(pair.first);
        if (it == other.entries.end()) {
            return Bool(false);
        }
        
        if (!pair.second || !it->second) {
            if (pair.second != it->second) {
                return Bool(false);
            }
            continue;
        }
        
        // 比较值
        if (pair.second->toString() != it->second->toString()) {
            return Bool(false);
        }
    }
    
    return Bool(true);
}

Bool Dict::operator!=(const Dict& other) const {
    Bool eqResult = operator==(other);
    return Bool(!eqResult.toBool());
}

Bool Dict::operator<(const Dict& other) const {
    // 字典的比较基于键值对的数量和内容
    if (entries.size() != other.entries.size()) {
        return Bool(entries.size() < other.entries.size());
    }
    
    // 如果大小相同，比较内容
    auto thisIt = entries.begin();
    auto otherIt = other.entries.begin();
    
    while (thisIt != entries.end() && otherIt != other.entries.end()) {
        // 比较键
        if (thisIt->first < otherIt->first) {
            return Bool(true);
        } else if (thisIt->first > otherIt->first) {
            return Bool(false);
        }
        
        // 键相等，比较值
        if (thisIt->second && otherIt->second) {
            if (thisIt->second->toString() < otherIt->second->toString()) {
                return Bool(true);
            } else if (thisIt->second->toString() > otherIt->second->toString()) {
                return Bool(false);
            }
        }
        
        ++thisIt;
        ++otherIt;
    }
    
    return Bool(false);
}

Bool Dict::operator<=(const Dict& other) const {
    Bool ltResult = operator<(other);
    Bool eqResult = operator==(other);
    return Bool(ltResult.toBool() || eqResult.toBool());
}

Bool Dict::operator>(const Dict& other) const {
    Bool leResult = operator<=(other);
    return Bool(!leResult.toBool());
}

Bool Dict::operator>=(const Dict& other) const {
    Bool ltResult = operator<(other);
    return Bool(!ltResult.toBool());
}
