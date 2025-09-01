#include "interpreter/types/types.h"

using namespace std;

// ==================== Interface实现 ====================
Interface::Interface(const string& name) 
    : ObjectType(name, false, false, false, false, false) {
}

void Interface::addMethodPrototype(const string& name, function<Value*(Value*, vector<Value*>&)> prototype) {
    methodPrototypes[name] = prototype;
    registerMethod(name, prototype);
}

function<Value*(Value*, vector<Value*>&)> Interface::getMethodPrototype(const string& name) const {
    auto it = methodPrototypes.find(name);
    return (it != methodPrototypes.end()) ? it->second : nullptr;
}

bool Interface::hasMethodPrototype(const string& name) const {
    return methodPrototypes.find(name) != methodPrototypes.end();
}

vector<string> Interface::getMethodPrototypeNames() const {
    vector<string> names;
    for (const auto& pair : methodPrototypes) {
        names.push_back(pair.first);
    }
    return names;
}

bool Interface::isCompatibleWith(ObjectType* other) const {
    return typeName == other->getTypeName();
}

Value* Interface::createDefaultValue() {
    // 接口类型不能创建实例
    return nullptr;
}
