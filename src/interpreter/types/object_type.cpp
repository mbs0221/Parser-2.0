#include "interpreter/types/types.h"
#include "interpreter/values/value.h"

#include "common/logger.h"
#include <algorithm>
#include <sstream>

using namespace std;

// ==================== ObjectType实现 ====================
ObjectType::ObjectType(const string& name, bool primitive, bool mutable_, bool reference, bool container, bool userDefined, ObjectType* parent)
    : typeName(name), isPrimitive(primitive), isMutable(mutable_), 
      isReference(reference), isContainer(container), isUserDefined(userDefined),
      parentType(parent) {
    
    // 自动注册构造函数方法
    registerConstructor();
    
    // 自动注册成员访问方法
    registerMemberAccess();
}

string ObjectType::getTypeName() const {
    return typeName;
}

bool ObjectType::isPrimitiveType() const {
    return isPrimitive;
}

bool ObjectType::isMutableType() const {
    return isMutable;
}

bool ObjectType::isReferenceType() const {
    return isReference;
}

bool ObjectType::isContainerType() const {
    return isContainer;
}

bool ObjectType::isUserDefinedType() const {
    return isUserDefined;
}

ObjectType* ObjectType::getParentType() const {
    return parentType;
}

void ObjectType::setParentType(ObjectType* parent) {
    parentType = parent;
}

const vector<ObjectType*>& ObjectType::getInterfaces() const {
    return interfaces;
}

void ObjectType::addInterface(ObjectType* interface) {
    interfaces.push_back(interface);
}

bool ObjectType::isSubtypeOf(ObjectType* other) const {
    if (!other) return false;
    
    // 检查直接父类
    if (parentType == other) return true;
    
    // 递归检查父类
    if (parentType) return parentType->isSubtypeOf(other);
    
    return false;
}

bool ObjectType::implements(ObjectType* interface) const {
    if (!interface) return false;
    
    // 检查直接实现的接口
    for (ObjectType* iface : interfaces) {
        if (iface == interface) return true;
    }
    
    // 递归检查父类实现的接口
    if (parentType) return parentType->implements(interface);
    
    return false;
}

Value* ObjectType::convertTo(ObjectType* targetType, Value* value) {
    if (!targetType || !value) return nullptr;
    
    // 如果目标类型是当前类型，直接返回
    if (targetType == this) return value;
    
    // 默认实现：不支持转换
    return nullptr;
}

bool ObjectType::isCompatibleWith(ObjectType* other) const {
    if (!other) return false;
    return typeName == other->getTypeName();
}

Value* ObjectType::createDefaultValue() {
    return createValueWithArgs({});
}

Value* ObjectType::createValueWithArgs(const vector<Value*>& args) {
    // 默认实现：创建空值
    return new Null();
}

void ObjectType::bindMethodsToInstance(Value* instance) {
    // 默认实现：空操作
    // 子类可以重写此方法来自定义绑定逻辑
}

void ObjectType::initializeInstance(Value* instance, vector<Value*>& args) {
    // 默认实现：空操作
    // 子类可以重写此方法来自定义初始化逻辑
}

void ObjectType::registerConstructor() {
    // 默认实现：空操作
    // 子类可以重写此方法来自定义构造函数注册逻辑
}

void ObjectType::registerMemberAccess() {
    // 默认实现：空操作
    // 子类可以重写此方法来自定义成员访问方法注册逻辑
}

Value* ObjectType::accessMember(Value* instance, const string& memberName) {
    // 默认实现：不支持成员访问
    return nullptr;
}

string ObjectType::getTypeDescription() const {
    stringstream ss;
    ss << "Type: " << typeName;
    if (isPrimitive) ss << " (Primitive)";
    if (isMutable) ss << " (Mutable)";
    if (isReference) ss << " (Reference)";
    if (isContainer) ss << " (Container)";
    if (isUserDefined) ss << " (UserDefined)";
    if (parentType) ss << " extends " << parentType->getTypeName();
    return ss.str();
}

bool ObjectType::operator==(const ObjectType& other) const {
    return typeName == other.typeName;
}

bool ObjectType::operator!=(const ObjectType& other) const {
    return !(*this == other);
}
