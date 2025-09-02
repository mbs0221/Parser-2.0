#include "interpreter/types/types.h"

using namespace std;

// ==================== StructType实现 ====================
StructType::StructType(const string& name) 
    : ClassType(name) {
}

void StructType::addMember(const string& name, ObjectType* type, VisibilityType visibility, Value* initialValue) {
    // 强制所有成员为公有
    ClassType::addMember(name, type, VIS_PUBLIC);
    
    // 如果提供了初始值，设置它
    if (initialValue) {
        setMemberInitialValue(name, initialValue);
    }
}

void StructType::registerMethod(const string& name, function<Value*(Value*, vector<Value*>&)> method) {
    // 强制所有方法为公有 - 注释掉，因为 registerMethod 方法不存在
    // ObjectType::registerMethod(name, method);
}
