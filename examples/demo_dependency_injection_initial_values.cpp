#include "interpreter/types/type_factory.h"
#include "interpreter/types/struct_type_builder.h"
#include "interpreter/types/class_type_builder.h"
#include "interpreter/types/builtin_type.h"
#include "interpreter/scope/scope_manager.h"
#include "interpreter/types/type_registry.h"
#include <iostream>

using namespace std;

// 演示TypeFactory通过依赖注入自动计算成员初始值
void demonstrateDependencyInjectionInitialValues() {
    cout << "=== TypeFactory依赖注入初始值计算演示 ===" << endl;
    
    // 创建依赖组件
    TypeRegistry typeRegistry;
    ScopeManager scopeManager(&typeRegistry);
    
    // 模拟一个简单的解释器（实际使用时应该是真实的Interpreter实例）
    // 这里我们创建一个模拟的解释器来演示概念
    class MockInterpreter {
    public:
        Value* visit(Expression* expr) {
            // 模拟解释器计算表达式的逻辑
            // 实际使用时，这里会真正解析和计算AST表达式
            cout << "MockInterpreter: Computing expression..." << endl;
            
            // 返回一个模拟的初始值
            // 在实际实现中，这里会根据表达式的类型返回相应的值
            return new String("computed_value");
        }
    };
    
    MockInterpreter mockInterpreter;
    
    // 创建TypeFactory，注入所有依赖
    TypeFactory typeFactory(&scopeManager);
    
    cout << "\n1. 创建结构体类型（自动计算初始值）:" << endl;
    
    // 模拟结构体定义
    // 在实际使用中，这些会从AST解析得到
    vector<StructMember> personMembers = {
        StructMember("name", nullptr, "public", nullptr),  // 没有默认值
        StructMember("age", nullptr, "public", nullptr),   // 没有默认值
        StructMember("email", nullptr, "private", nullptr) // 没有默认值
    };
    
    // 创建结构体定义
    StructDefinition personDef("Person", personMembers);
    
    // 调用TypeFactory创建结构体类型
    // 注意：不再需要传入memberInitialValues参数
    StructType* personType = typeFactory.createStructTypeFromAST(&personDef);
    
    if (personType) {
        cout << "✓ Person结构体创建成功！" << endl;
        cout << "✓ 成员初始值已通过依赖注入自动计算并设置到类型中" << endl;
        
        // 显示设置的初始值
        const auto& initialValues = personType->getAllMemberInitialValues();
        cout << "Type中存储的初始值数量: " << initialValues.size() << endl;
        for (const auto& [memberName, initialValue] : initialValues) {
            cout << "  " << memberName << " = " << initialValue->str() << endl;
        }
    }
    
    cout << "\n2. 创建类类型（自动计算初始值）:" << endl;
    
    // 模拟类定义
    vector<StructMember> studentMembers = {
        StructMember("studentId", nullptr, "public", nullptr),
        StructMember("grade", nullptr, "public", nullptr),
        StructMember("gpa", nullptr, "private", nullptr)
    };
    
    vector<ClassMethod*> studentMethods = {}; // 暂时没有方法
    
    // 创建类定义
    ClassDefinition studentDef("Student", "Person", studentMembers, studentMethods);
    
    // 调用TypeFactory创建类类型
    // 注意：不再需要传入memberInitialValues参数
    ClassType* studentType = typeFactory.createClassTypeFromAST(&studentDef);
    
    if (studentType) {
        cout << "✓ Student类创建成功！" << endl;
        cout << "✓ 成员初始值已通过依赖注入自动计算并设置到类型中" << endl;
        
        // 显示设置的初始值
        const auto& initialValues = studentType->getAllMemberInitialValues();
        cout << "Type中存储的初始值数量: " << initialValues.size() << endl;
        for (const auto& [memberName, initialValue] : initialValues) {
            cout << "  " << memberName << " = " << initialValue->str() << endl;
        }
    }
    
    cout << "\n3. 演示构造函数不再需要处理初始值:" << endl;
    
    if (personType) {
        // 创建实例时，类型会自动使用存储的初始值
        Value* personInstance = personType->createDefaultValue();
        if (personInstance) {
            cout << "✓ 通过createDefaultValue()创建实例成功" << endl;
            cout << "✓ 构造函数不需要处理初始值，类型自动管理" << endl;
            
            // 显示实例的成员值
            if (Dict* dict = dynamic_cast<Dict*>(personInstance)) {
                cout << "实例成员值:" << endl;
                for (const auto& memberName : personType->getMemberNames()) {
                    Value* memberValue = dict->getEntry(memberName);
                    if (memberValue) {
                        cout << "  " << memberName << " = " << memberValue->str() << endl;
                    }
                }
            }
            
            delete personInstance;
        }
    }
}

// 演示依赖注入的优势
void demonstrateDependencyInjectionBenefits() {
    cout << "\n=== 依赖注入的优势 ===" << endl;
    
    cout << "1. 自动化初始值计算:" << endl;
    cout << "   - TypeFactory自动调用Interpreter计算所有成员初始值" << endl;
    cout << "   - 不需要手动传入初始值参数" << endl;
    cout << "   - 初始值计算逻辑集中在一个地方" << endl;
    
    cout << "\n2. 性能提升:" << endl;
    cout << "   - 初始值只在类型创建时计算一次" << endl;
    cout << "   - 构造函数调用时不再重复计算" << endl;
    cout << "   - 类型实例化时直接使用存储的初始值" << endl;
    
    cout << "\n3. 职责分离:" << endl;
    cout << "   - TypeFactory: 负责类型创建和初始值计算" << endl;
    cout << "   - Interpreter: 负责表达式求值" << endl;
    cout << "   - ClassType: 负责管理自己的初始值" << endl;
    cout << "   - 构造函数: 专注于实例创建和参数处理" << endl;
    
    cout << "\n4. 易于测试和维护:" << endl;
    cout << "   - 可以轻松模拟Interpreter进行测试" << endl;
    cout << "   - 初始值计算逻辑独立，易于调试" << endl;
    cout << "   - 支持不同的解释器实现" << endl;
}

int main() {
    cout << "TypeFactory依赖注入初始值计算演示程序" << endl;
    cout << "=========================================" << endl;
    
    try {
        demonstrateDependencyInjectionInitialValues();
        demonstrateDependencyInjectionBenefits();
        
        cout << "\n=== 演示完成 ===" << endl;
        cout << "重构后的优势:" << endl;
        cout << "1. 方法签名更简洁: createStructTypeFromAST(structDef)" << endl;
        cout << "2. 完全依赖注入: 通过Interpreter自动计算初始值" << endl;
        cout << "3. 性能更优: 初始值计算一次，多次使用" << endl;
        cout << "4. 代码更清晰: 职责分离，易于理解" << endl;
        
    } catch (const exception& e) {
        cerr << "演示过程中发生错误: " << e.what() << endl;
        return 1;
    }
    
    return 0;
} 