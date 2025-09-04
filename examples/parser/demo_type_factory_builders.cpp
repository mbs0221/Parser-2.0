#include "interpreter/types/type_factory.h"
#include "interpreter/types/struct_type_builder.h"
#include "interpreter/types/class_type_builder.h"
#include "interpreter/types/builtin_type.h"
#include "interpreter/scope/scope_manager.h"
#include "interpreter/types/type_registry.h"
#include <iostream>

using namespace std;

// 演示TypeFactory使用两个构建者创建类型
void demonstrateTypeFactoryBuilders() {
    cout << "=== TypeFactory构建者模式演示 ===" << endl;
    
    // 创建依赖组件
    TypeRegistry typeRegistry;
    ScopeManager scopeManager(&typeRegistry);
    TypeFactory typeFactory(&scopeManager);
    
    // 1. 使用StructTypeBuilder创建结构体
    cout << "\n1. 使用StructTypeBuilder创建Person结构体:" << endl;
    
    StructTypeBuilder personBuilder = typeFactory.createStructTypeBuilder("Person");
    
    personBuilder
        .addMember("name", nullptr, "public")
        .addMember("age", nullptr, "public")
        .addMember("email", nullptr, "private")
        .setAutoGenerateConstructor(true)
        .setAutoGenerateNewMethod(true)
        .setAutoGenerateStandardMethods(true)
        .setAutoGenerateAccessors(true);
    
    // 添加自定义方法
    personBuilder.addMethod("greet", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Hello! I am a Person." << endl;
        return nullptr;
    });
    
    // 添加自定义构造函数
    personBuilder.addConstructor([](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Custom Person constructor called with " << args.size() << " arguments" << endl;
        return nullptr;
    });
    
    // 构建结构体
    StructType* personType = personBuilder.build();
    if (personType) {
        cout << "✓ Person结构体创建成功！" << endl;
    }
    
    // 2. 使用ClassTypeBuilder创建类
    cout << "\n2. 使用ClassTypeBuilder创建Student类:" << endl;
    
    ClassTypeBuilder studentBuilder = typeFactory.createClassTypeBuilder("Student", "Person");
    
    studentBuilder
        .addMember("studentId", nullptr, "public")
        .addMember("grade", nullptr, "public")
        .addMember("gpa", nullptr, "private")
        .setAutoGenerateConstructor(true)
        .setAutoGenerateNewMethod(true)
        .setAutoGenerateStandardMethods(true)
        .setAutoGenerateAccessors(true)
        .setAutoGenerateInheritedMethods(true);
    
    // 添加学生特有的方法
    studentBuilder.addMethod("study", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Student is studying..." << endl;
        return nullptr;
    });
    
    studentBuilder.addMethod("takeExam", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Student is taking an exam..." << endl;
        return new Bool(true);
    });
    
    // 添加运算符重载
    studentBuilder.addOperator("==", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Student equality comparison" << endl;
        return new Bool(true);
    });
    
    // 构建类
    ClassType* studentType = studentBuilder.build();
    if (studentType) {
        cout << "✓ Student类创建成功！" << endl;
    }
    
    // 3. 使用ClassTypeBuilder创建Employee类
    cout << "\n3. 使用ClassTypeBuilder创建Employee类:" << endl;
    
    ClassTypeBuilder employeeBuilder = typeFactory.createClassTypeBuilder("Employee");
    
    employeeBuilder
        .addMember("id", nullptr, "public")
        .addMember("name", nullptr, "public")
        .addMember("salary", nullptr, "private")
        .addMember("department", nullptr, "public")
        .setAutoGenerateConstructor(true)
        .setAutoGenerateStandardMethods(true)
        .setAutoGenerateAccessors(true);
    
    // 添加业务方法
    employeeBuilder.addMethod("work", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Employee is working..." << endl;
        return nullptr;
    });
    
    employeeBuilder.addMethod("getSalary", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Getting employee salary..." << endl;
        return new Double(50000.0);
    });
    
    // 构建类
    ClassType* employeeType = employeeBuilder.build();
    if (employeeType) {
        cout << "✓ Employee类创建成功！" << endl;
    }
    
    // 4. 重置构建器并重用
    cout << "\n4. 重置构建器并重用:" << endl;
    
    employeeBuilder.reset();
    
    employeeBuilder
        .addMember("x", nullptr, "public")
        .addMember("y", nullptr, "public")
        .setAutoGenerateConstructor(true)
        .setAutoGenerateStandardMethods(true);
    
    // 添加几何方法
    employeeBuilder.addMethod("distance", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Calculating distance..." << endl;
        return new Double(0.0);
    });
    
    // 构建Point类
    ClassType* pointType = employeeBuilder.build();
    if (pointType) {
        cout << "✓ Point类创建成功！" << endl;
    }
}

// 演示与解释器的集成
void demonstrateInterpreterIntegration() {
    cout << "\n=== 与解释器集成演示 ===" << endl;
    
    // 这里可以演示TypeFactory如何与解释器交互来计算成员初始值
    // 由于我们没有完整的解释器实现，这里只是展示概念
    
    cout << "TypeFactory与解释器的集成特性:" << endl;
    cout << "1. 成员初始值计算: 调用interpreter->visit(member.defaultValue)" << endl;
    cout << "2. 类型验证: 验证计算的初始值是否与成员类型兼容" << endl;
    cout << "3. 自动回退: 验证失败时使用类型系统的默认值" << endl;
    cout << "4. 批量处理: computeAllMemberInitialValues()一次性计算所有成员" << endl;
}

int main() {
    cout << "TypeFactory构建者模式演示程序" << endl;
    cout << "================================" << endl;
    
    try {
        demonstrateTypeFactoryBuilders();
        demonstrateInterpreterIntegration();
        
        cout << "\n=== 演示完成 ===" << endl;
        cout << "构建者模式的优势:" << endl;
        cout << "1. 类型工厂使用专门的构建者创建不同类型的类型" << endl;
        cout << "2. StructTypeBuilder: 专门用于构建结构体类型" << endl;
        cout << "3. ClassTypeBuilder: 专门用于构建类类型，支持继承" << endl;
        cout << "4. 统一的API: 两个构建者都支持链式调用和自动生成" << endl;
        cout << "5. 与解释器集成: 智能计算成员初始值" << endl;
        
    } catch (const exception& e) {
        cerr << "演示过程中发生错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
} 