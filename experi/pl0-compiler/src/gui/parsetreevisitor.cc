#include "parsetreevisitor.h"
#include <QStringList>
#include <sstream>

void ParseTreeVisitor::addTreeChild(std::string name, AST::ASTTree *astTree)
{
   TreeItem<AST::ASTTree> *item = topStackItem();
   TreeItem<AST::ASTTree> *newItem = addTreeChild(item, QString::fromStdString(name), astTree->startPosition.column, astTree->startPosition.row);
   newItem->setReference(astTree);
   treeWitgetItemStack.push(newItem);
}

TreeItem<AST::ASTTree> *ParseTreeVisitor::addTreeChild(TreeItem<AST::ASTTree> *parent, QString name, int column, int row)
{
   QStringList line;
   line.append(name);

   if (column != 0 && row != 0)
   {
      std::ostringstream stringStream1, stringStream2;
      stringStream1 << row;
      stringStream2 << column;

      line.append(QString::fromStdString(stringStream1.str()));
      line.append(QString::fromStdString(stringStream2.str()));
   }

   TreeItem<AST::ASTTree> *child = new TreeItem<AST::ASTTree>(parent, line);
   if (parent == NULL)
      parseTreeWidget->addTopLevelItem(child);
   else
      parent->addChild(child);

   return child;
}


void ParseTreeVisitor::addSymbolTable(SymbolTable *symbolTable)
{
    if (symbolTable == NULL) return;
    symbolTableWindow->addSymbolTable(symbolTable);
}


void ParseTreeVisitor::accepted()
{
   if (!treeWitgetItemStack.empty()) treeWitgetItemStack.pop();
}


TreeItem<AST::ASTTree> *ParseTreeVisitor::topStackItem()
{
   if (treeWitgetItemStack.empty()) return NULL;
   return treeWitgetItemStack.top();
}

void ParseTreeVisitor::visit(AST::Program *program)
{
   addTreeChild(std::string("Program"), program);
   addSymbolTable(program->symbolTable);
}

void ParseTreeVisitor::visit(AST::Block *block)
{
   addTreeChild(std::string("Block"), block);
}


void ParseTreeVisitor::visit(AST::Constants *constants)
{
   addTreeChild(std::string("Constants"), constants);
}


void ParseTreeVisitor::visit(AST::ConstantDeclaration *constantDeclaration)
{
   addTreeChild(std::string("ConstantDeclaration"), constantDeclaration);
}


void ParseTreeVisitor::visit(AST::Variables *variables)
{
   addTreeChild(std::string("Variables"), variables);
}


void ParseTreeVisitor::visit(AST::VariableDeclaration *variableDeclaration)
{
   addTreeChild(std::string("VariableDeclaration"), variableDeclaration);
}


void ParseTreeVisitor::visit(AST::Procedures *procedures)
{
   addTreeChild(std::string("Procedures"), procedures);
}


void ParseTreeVisitor::visit(AST::ProcedureDeclaration *procedureDeclaration)
{
   addTreeChild(std::string("ProcedureDeclaration"), procedureDeclaration);
   addSymbolTable(procedureDeclaration->symbolTable);
}


void ParseTreeVisitor::visit(AST::FormalParameters *formalParameters)
{
   addTreeChild(std::string("FormalParameters"), formalParameters);
}


void ParseTreeVisitor::visit(AST::AssignmentStatement *assignmentStatement)
{
   addTreeChild(std::string("AssignmentStatement"), assignmentStatement);
}


void ParseTreeVisitor::visit(AST::CallStatement *callStatement)
{
   addTreeChild(std::string("CallStatement"), callStatement);
}


void ParseTreeVisitor::visit(AST::ActualParameters *actualParameters)
{
   addTreeChild(std::string("ActualParameters"), actualParameters);
}


void ParseTreeVisitor::visit(AST::CompoundStatement *compoundStatement)
{
   addTreeChild(std::string("CompoundStatement"), compoundStatement);
}


void ParseTreeVisitor::visit(AST::IfStatement *ifStatement)
{
   addTreeChild(std::string("IfStatement"), ifStatement);
}


void ParseTreeVisitor::visit(AST::InputStatement *inputStatement)
{
   addTreeChild(std::string("InputStatement"), inputStatement);
}


void ParseTreeVisitor::visit(AST::OutputStatement *outputStatement)
{
   addTreeChild(std::string("OutputStatement"), outputStatement);
}


void ParseTreeVisitor::visit(AST::WhileStatement *whileStatement)
{
   addTreeChild(std::string("WhileStatement"), whileStatement);
}


void ParseTreeVisitor::visit(AST::OddCondition *oddCondition)
{
   addTreeChild(std::string("OddCondition"), oddCondition);
}


void ParseTreeVisitor::visit(AST::EqualCondition *equalCondition)
{
   addTreeChild(std::string("EqualCondition"), equalCondition);
}


void ParseTreeVisitor::visit(AST::NotEqualCondition *notEqualCondition)
{
   addTreeChild(std::string("NotEqualCondition"), notEqualCondition);
}


void ParseTreeVisitor::visit(AST::GreaterCondition *greaterCondition)
{
   addTreeChild(std::string("GreaterCondition"), greaterCondition);
}


void ParseTreeVisitor::visit(AST::GreaterEqualCondition *greaterEqualCondition)
{
   addTreeChild(std::string("GreaterEqualCondition"), greaterEqualCondition);
}


void ParseTreeVisitor::visit(AST::LessCondition *lessCondition)
{
   addTreeChild(std::string("LessCondition"), lessCondition);
}


void ParseTreeVisitor::visit(AST::LessEqualCondition *lessEqualCondition)
{
   addTreeChild(std::string("LessEqualCondition"), lessEqualCondition);
}


void ParseTreeVisitor::visit(AST::PlusExpression *plusExpression)
{
   addTreeChild(std::string("PlusExpression"), plusExpression);
}


void ParseTreeVisitor::visit(AST::MinusExpression *minusExpression)
{
   addTreeChild(std::string("MinusExpression"), minusExpression);
}


void ParseTreeVisitor::visit(AST::FirstTermExpression *firstTermExpression)
{
   addTreeChild(std::string("FirstTermExpression"), firstTermExpression);
}


void ParseTreeVisitor::visit(AST::MinusAdditionalExpression *minusAdditionalExpression)
{
   addTreeChild(std::string("MinusAdditionalExpression"), minusAdditionalExpression);
}


void ParseTreeVisitor::visit(AST::PlusAdditionalExpression *plusAdditionalExpression)
{
   addTreeChild(std::string("PlusAdditionalExpression"), plusAdditionalExpression);
}


void ParseTreeVisitor::visit(AST::TermAdditionalExpression *termAdditionalExpression)
{
   addTreeChild(std::string("TermAdditionalExpression"), termAdditionalExpression);
}


void ParseTreeVisitor::visit(AST::PlusFirstTerm *plusFirstTerm)
{
   addTreeChild(std::string("PlusFirstTerm"), plusFirstTerm);
}


void ParseTreeVisitor::visit(AST::MinusFirstTerm *minusFirstTerm)
{
   addTreeChild(std::string("MinusFirstTerm"), minusFirstTerm);
}


void ParseTreeVisitor::visit(AST::SignlessFirstTerm *signlessFirstTerm)
{
   addTreeChild(std::string("SignlessFirstTerm"), signlessFirstTerm);
}


void ParseTreeVisitor::visit(AST::MultTerm *multTerm)
{
   addTreeChild(std::string("MultTerm"), multTerm);
}


void ParseTreeVisitor::visit(AST::DivTerm *divTerm)
{
   addTreeChild(std::string("DivTerm"), divTerm);
}


void ParseTreeVisitor::visit(AST::FactorTerm *factorTerm)
{
   addTreeChild(std::string("FactorTerm"), factorTerm);
}


void ParseTreeVisitor::visit(AST::Ident *ident)
{
   addTreeChild(std::string("Ident: ") + ident->value, ident);
}


void ParseTreeVisitor::visit(AST::Number *number)
{
   std::ostringstream stringStream;
   stringStream << number->value;

   addTreeChild(std::string("Number: ") + stringStream.str(), number);
}


void ParseTreeVisitor::visit(AST::ParenthesisFactor *parenthesisFactor)
{
   addTreeChild(std::string("ParenthesisFactor"), parenthesisFactor);
}
