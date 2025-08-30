#include "parser/function.h"
#include "parser/ast_visitor.h"

// Identifier类的accept方法实现
void Identifier::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// Variable类的accept方法实现
void Variable::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// FunctionPrototype类的accept方法实现
void FunctionPrototype::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// FunctionDefinition类的accept方法实现
void FunctionDefinition::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// StructDefinition类的accept方法实现
void StructDefinition::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// ClassMethod类的accept方法实现
void ClassMethod::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// ClassDefinition类的accept方法实现
void ClassDefinition::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}
