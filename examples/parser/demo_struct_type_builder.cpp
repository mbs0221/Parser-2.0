#include "interpreter/types/struct_type_builder.h"
#include "interpreter/types/type_factory.h"
#include "interpreter/types/builtin_type.h"
#include "interpreter/scope/scope_manager.h"
#include "interpreter/types/type_registry.h"
#include <iostream>

using namespace std;

// 演示StructTypeBuilder的使用
void demonstrateStructTypeBuilder() {
    cout << "=== StructTypeBuilder 演示 ===" << endl;
    
    // 创建依赖组件
    TypeRegistry typeRegistry;
    ScopeManager scopeManager(&typeRegistry);
    TypeFactory typeFactory(&scopeManager);
    
    // 方法1：使用Builder模式创建结构体
    cout << "\n1. 使用Builder模式创建Person结构体:" << endl;
    
    StructTypeBuilder personBuilder = typeFactory.createStructTypeBuilder("Person");
    
    // 链式调用配置结构体
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
    
    // 添加运算符重载
    personBuilder.addOperator("==", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Person equality comparison" << endl;
        return new Bool(true);
    });
    
    // 显示构建器信息
    cout << personBuilder.getBuilderInfo() << endl;
    
    // 构建结构体类型
    StructType* personType = personBuilder.build();
    if (personType) {
        cout << "✓ Person结构体创建成功！" << endl;
    } else {
        cout << "✗ Person结构体创建失败！" << endl;
    }
    
    // 方法2：使用Builder模式创建Point结构体
    cout << "\n2. 使用Builder模式创建Point结构体:" << endl;
    
    StructTypeBuilder pointBuilder = typeFactory.createStructTypeBuilder("Point");
    
    pointBuilder
        .addMember("x", nullptr, "public")
        .addMember("y", nullptr, "public")
        .setAutoGenerateConstructor(true)
        .setAutoGenerateStandardMethods(true)
        .setAutoGenerateAccessors(false); // 不自动生成访问器
    
    // 添加自定义方法
    pointBuilder.addMethod("distance", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Calculating distance..." << endl;
        return new Double(0.0);
    });
    
    pointBuilder.addMethod("move", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Moving point..." << endl;
        return nullptr;
    });
    
    // 构建
    StructType* pointType = pointBuilder.build();
    if (pointType) {
        cout << "✓ Point结构体创建成功！" << endl;
    } else {
        cout << "✗ Point结构体创建失败！" << endl;
    }
    
    // 方法3：重置构建器并创建新结构体
    cout << "\n3. 重置构建器并创建Student结构体:" << endl;
    
    pointBuilder.reset(); // 重置构建器状态
    
    pointBuilder
        .addMember("id", nullptr, "public")
        .addMember("name", nullptr, "public")
        .addMember("grade", nullptr, "public")
        .addMember("gpa", nullptr, "private")
        .setAutoGenerateConstructor(true)
        .setAutoGenerateNewMethod(true)
        .setAutoGenerateStandardMethods(true)
        .setAutoGenerateAccessors(true);
    
    // 添加学生特有的方法
    pointBuilder.addMethod("study", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Student is studying..." << endl;
        return nullptr;
    });
    
    pointBuilder.addMethod("takeExam", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Student is taking an exam..." << endl;
        return new Bool(true);
    });
    
    // 构建
    StructType* studentType = pointBuilder.build();
    if (studentType) {
        cout << "✓ Student结构体创建成功！" << endl;
    } else {
        cout << "✗ Student结构体创建失败！" << endl;
    }
    
    // 显示最终构建器信息
    cout << "\n最终构建器信息:" << endl;
    cout << pointBuilder.getBuilderInfo() << endl;
}

// 演示与TypeFactory的集成
void demonstrateTypeFactoryIntegration() {
    cout << "\n=== TypeFactory集成演示 ===" << endl;
    
    TypeRegistry typeRegistry;
    ScopeManager scopeManager(&typeRegistry);
    TypeFactory typeFactory(&scopeManager);
    
    // 使用TypeFactory的Builder方法
    cout << "使用TypeFactory创建Builder:" << endl;
    
    StructTypeBuilder builder = typeFactory.createStructTypeBuilder("Employee");
    
    builder
        .addMember("id", nullptr, "public")
        .addMember("name", nullptr, "public")
        .addMember("salary", nullptr, "private")
        .addMember("department", nullptr, "public")
        .setAutoGenerateConstructor(true)
        .setAutoGenerateStandardMethods(true)
        .setAutoGenerateAccessors(true);
    
    // 添加业务方法
    builder.addMethod("work", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Employee is working..." << endl;
        return nullptr;
    });
    
    builder.addMethod("getSalary", [](Value* instance, vector<Value*>& args) -> Value* {
        cout << "Getting employee salary..." << endl;
        return new Double(50000.0);
    });
    
    // 构建
    StructType* employeeType = builder.build();
    if (employeeType) {
        cout << "✓ Employee结构体创建成功！" << endl;
    } else {
        cout << "✗ Employee结构体创建失败！" << endl;
    }
}

int main() {
    cout << "StructTypeBuilder 模式演示程序" << endl;
    cout << "================================" << endl;
    
    try {
        demonstrateStructTypeBuilder();
        demonstrateTypeFactoryIntegration();
        
        cout << "\n=== 演示完成 ===" << endl;
        cout << "Builder模式的优势:" << endl;
        cout << "1. 链式调用，代码更清晰" << endl;
        cout << "2. 可配置的自动生成功能" << endl;
        cout << "3. 灵活的方法和成员添加" << endl;
        cout << "4. 易于扩展和维护" << endl;
        cout << "5. 与TypeFactory完美集成" << endl;
        
    } catch (const exception& e) {
        cerr << "演示过程中发生错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
} 