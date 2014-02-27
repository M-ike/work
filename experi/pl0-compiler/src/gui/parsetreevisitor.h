#ifndef PARSETREEVISITOR_H
#define PARSETREEVISITOR_H

#include <stack>
#include <string>
#include <QTreeWidget>
#include <QString>

#include "treeitem.h"
#include "symboltablewindow.h"

#include "../parser/ast_visitor.h"
#include "../common.h"
#include "../parser/symboltable.h"
#include "../parser/ast.h"

class ParseTreeVisitor: public ASTVisitor
{
    public:
       ParseTreeVisitor(QTreeWidget *parseTreeWidget, SymbolTableWindow *symbolTableWindow)
       : parseTreeWidget(parseTreeWidget), symbolTableWindow(symbolTableWindow)
       {}

       virtual void accepted();

       virtual void visit(AST::Program *program);
       virtual void visit(AST::Block *block);
       virtual void visit(AST::Constants *constants);
       virtual void visit(AST::ConstantDeclaration *constantDeclaration);
       virtual void visit(AST::Variables *variables);
       virtual void visit(AST::VariableDeclaration *variableDeclaration);
       virtual void visit(AST::Procedures *procedures);
       virtual void visit(AST::ProcedureDeclaration *procedureDeclaration);
       virtual void visit(AST::FormalParameters *formalParameters);
       virtual void visit(AST::AssignmentStatement *assignmentStatement);
       virtual void visit(AST::CallStatement *callStatement);
       virtual void visit(AST::ActualParameters *actualParameters);
       virtual void visit(AST::CompoundStatement *compoundStatement);
       virtual void visit(AST::IfStatement *ifStatement);
       virtual void visit(AST::InputStatement *inputStatement);
       virtual void visit(AST::OutputStatement *outputStatement);
       virtual void visit(AST::WhileStatement *whileStatement);
       virtual void visit(AST::OddCondition *oddCondition);
       virtual void visit(AST::EqualCondition *equalCondition);
       virtual void visit(AST::NotEqualCondition *notEqualCondition);
       virtual void visit(AST::GreaterCondition *greaterCondition);
       virtual void visit(AST::GreaterEqualCondition *greaterEqualCondition);
       virtual void visit(AST::LessCondition *lessCondition);
       virtual void visit(AST::LessEqualCondition *lessEqualCondition);
       virtual void visit(AST::PlusExpression *plusExpression);
       virtual void visit(AST::MinusExpression *minusExpression);
       virtual void visit(AST::FirstTermExpression *firstTermExpression);
       virtual void visit(AST::MinusAdditionalExpression *minusAdditionalExpression);
       virtual void visit(AST::PlusAdditionalExpression *plusAdditionalExpression);
       virtual void visit(AST::TermAdditionalExpression *termAdditionalExpression);
       virtual void visit(AST::PlusFirstTerm *plusFirstTerm);
       virtual void visit(AST::MinusFirstTerm *minusFirstTerm);
       virtual void visit(AST::SignlessFirstTerm *signlessFirstTerm);
       virtual void visit(AST::MultTerm *multTerm);
       virtual void visit(AST::DivTerm *divTerm);
       virtual void visit(AST::FactorTerm *factorTerm);
       virtual void visit(AST::Ident *ident);
       virtual void visit(AST::Number *number);
       virtual void visit(AST::ParenthesisFactor *parenthesisFactor);

   private:
       QTreeWidget *parseTreeWidget;
       SymbolTableWindow *symbolTableWindow;
       std::stack<TreeItem<AST::ASTTree>*> treeWitgetItemStack;
       TreeItem<AST::ASTTree> *topStackItem();

       void addSymbolTable(SymbolTable *symboltable);

       void addTreeChild(std::string name, AST::ASTTree *astTree);
       TreeItem<AST::ASTTree> *addTreeChild(TreeItem<AST::ASTTree> *parent, QString name, int column, int row);
};

#endif
