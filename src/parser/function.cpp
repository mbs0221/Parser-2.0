#include "parser/function.h"
#include "parser/ast_visitor.h"

// Identifier类的accept方法实现
void Identifier::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// Variable类的accept方法实现
void Variable::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// FunctionPrototype类的accept方法实现
void FunctionPrototype::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// FunctionDefinition类的accept方法实现
void FunctionDefinition::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// BuiltinFunction类的accept方法实现
void BuiltinFunction::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// UserFunction类的accept方法实现
void UserFunction::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// StructDefinition类的accept方法实现
void StructDefinition::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ClassMethod类的accept方法实现
void ClassMethod::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}

// ClassDefinition类的accept方法实现
void ClassDefinition::accept(ASTVisitor* visitor) {
    visitor->visit(this);
}
