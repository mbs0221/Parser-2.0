#include "parser/definition.h"
#include "parser/ast_visitor.h"

// Definition类的accept方法实现
void Definition::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// VariableDefinition类的accept方法实现
void VariableDefinition::accept(StatementVisitor* visitor) {
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

// ClassDefinition类的accept方法实现
void ClassDefinition::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}

// VisibilityStatement类的accept方法实现
void VisibilityStatement::accept(StatementVisitor* visitor) {
    visitor->visit(this);
}
