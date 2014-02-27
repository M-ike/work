#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "ast.h"

#include "ast_classes.h"

class ASTVisitor
{
    public:

       virtual void accepted() = 0;

       virtual void visit(AST::Program *program) = 0;
       virtual void visit(AST::Block *block) = 0;
       virtual void visit(AST::Constants *constants) = 0;
       virtual void visit(AST::ConstantDeclaration *constantDeclaration) = 0;
       virtual void visit(AST::Variables *variables) = 0;
       virtual void visit(AST::VariableDeclaration *variableDeclaration) = 0;
       virtual void visit(AST::Procedures *procedures) = 0;
       virtual void visit(AST::ProcedureDeclaration *procedureDeclaration) = 0;
       virtual void visit(AST::ActualParameters *actualParameters) = 0;
       virtual void visit(AST::AssignmentStatement *assignmentStatement) = 0;
       virtual void visit(AST::CallStatement *callStatement) = 0;
       virtual void visit(AST::FormalParameters *formalParameters) = 0;
       virtual void visit(AST::CompoundStatement *compundStatement) = 0;
       virtual void visit(AST::IfStatement *ifStatement) = 0;
       virtual void visit(AST::InputStatement *inputStatement) = 0;
       virtual void visit(AST::OutputStatement *outputStatement) = 0;
       virtual void visit(AST::WhileStatement *whileStatement) = 0;
       virtual void visit(AST::OddCondition *oddCondition) = 0;
       virtual void visit(AST::EqualCondition *equalCondition) = 0;
       virtual void visit(AST::NotEqualCondition *notEqualCondition) = 0;
       virtual void visit(AST::GreaterCondition *greaterCondition) = 0;
       virtual void visit(AST::GreaterEqualCondition *greaterEqualCondition) = 0;
       virtual void visit(AST::LessCondition *lessCondition) = 0;
       virtual void visit(AST::LessEqualCondition *lessEqualCondition) = 0;
       virtual void visit(AST::PlusExpression *plusExpression) = 0;
       virtual void visit(AST::MinusExpression *minusExpression) = 0;
       virtual void visit(AST::FirstTermExpression *firstTermExpression) = 0;
       virtual void visit(AST::MinusAdditionalExpression *minusAdditionalExpression) = 0;
       virtual void visit(AST::PlusAdditionalExpression *plusAdditionalExpression) = 0;
       virtual void visit(AST::TermAdditionalExpression *termAdditionalExpression) = 0;
       virtual void visit(AST::PlusFirstTerm *plusFirstTerm) = 0;
       virtual void visit(AST::MinusFirstTerm *minusFirstTerm) = 0;
       virtual void visit(AST::SignlessFirstTerm *signlessFirstTerm) = 0;
       virtual void visit(AST::MultTerm *multTerm) = 0;
       virtual void visit(AST::DivTerm *divTerm) = 0;
       virtual void visit(AST::FactorTerm *factorTerm) = 0;
       virtual void visit(AST::Ident *ident) = 0;
       virtual void visit(AST::Number *number) = 0;
       virtual void visit(AST::ParenthesisFactor *parenthesisFactor) = 0;

       virtual ~ASTVisitor() {}

    protected:
       ASTVisitor() {}
};


#endif
