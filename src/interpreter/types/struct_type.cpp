#include "interpreter/types/types.h"

using namespace std;

// ==================== StructType实现 ====================
StructType::StructType(const string& name) 
    : ClassType(name) {
}

void StructType::addMember(const string& name, ObjectType* type, VisibilityType visibility, Value* initialValue) {
    // 强制所有成员为公有
    ClassType::addMember(name, type, VIS_PUBLIC, initialValue);
}

void StructType::registerMethod(const string& name, function<Value*(Value*, vector<Value*>&)> method) {
    // 强制所有方法为公有
    ObjectType::registerMethod(name, method);
}
