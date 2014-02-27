#include "ast.h"
#include <list>
#include <iostream>

void AST::ASTTree::accept(ASTVisitor *visitor)
{
    accept_visitor(visitor);
    visitor->accepted();
}


void AST::Program::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (block != NULL) block->accept(visitor);
}


void AST::Block::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (constants != NULL) constants->accept(visitor);
    if (variables != NULL) variables->accept(visitor);
    if (procedures != NULL) procedures->accept(visitor);
    if (statement != NULL) statement->accept(visitor);
}


void AST::Constants::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    std::list<AST::ConstantDeclaration*>::iterator it;
    for( it = constantsDeclarartions.begin(); it != constantsDeclarartions.end(); it++ )
       if (*it != NULL) (*it)->accept(visitor);
    if (lastConstant != NULL) lastConstant->accept(visitor);
}


void AST::Constants::writeConstantInList()
{
   if (lastConstant == NULL) return;
   constantsDeclarartions.push_back(lastConstant);
   lastConstant = NULL;
}


void AST::ConstantDeclaration::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (ident != NULL) ident->accept(visitor);
    if (number != NULL) number->accept(visitor);
}


void AST::Variables::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    std::list<AST::VariableDeclaration*>::iterator it;
    for( it = variablesDeclarartions.begin(); it != variablesDeclarartions.end(); it++ )
       if (*it != NULL) (*it)->accept(visitor);
    if (lastVariable != NULL) lastVariable->accept(visitor);
}


void AST::Variables::writeVariableInList()
{
   if (lastVariable == NULL) return;
   variablesDeclarartions.push_back(lastVariable);
   lastVariable = NULL;
}


int AST::Variables::getNr()
{
   return variablesDeclarartions.size();
}


void AST::VariableDeclaration::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (ident != NULL) ident->accept(visitor);
}


void AST::Procedures::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    std::list<AST::ProcedureDeclaration*>::iterator it;
    for( it = proceduresDeclarartions.begin(); it != proceduresDeclarartions.end(); it++ )
       if (*it != NULL) (*it)->accept(visitor);
    if (lastProcedure != NULL) lastProcedure->accept(visitor);
}


void AST::Procedures::writeProcedureInList()
{
   if (lastProcedure == NULL) return;
   proceduresDeclarartions.push_back(lastProcedure);
   lastProcedure = NULL;
}


std::list<AST::ProcedureDeclaration*> AST::Procedures::getProcedures()
{
    writeProcedureInList();
    return proceduresDeclarartions;
}


void AST::ProcedureDeclaration::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (ident != NULL) ident->accept(visitor);
    if (formalParameters != NULL) formalParameters->accept(visitor);
    if (block != NULL) block->accept(visitor);
}


void AST::FormalParameters::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    std::list<AST::VariableDeclaration*>::iterator it;
    for( it = variables.begin(); it != variables.end(); it++ )
       if (*it != NULL) (*it)->accept(visitor);
    if (lastParameter != NULL) lastParameter->accept(visitor);
}


void AST::FormalParameters::writeParameterInList()
{
   if (lastParameter == NULL) return;
   variables.push_back(lastParameter);
   lastParameter = NULL;
}


int AST::FormalParameters::getNrParameters()
{
   return variables.size();
}


void AST::AssignmentStatement::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (ident != NULL) ident->accept(visitor);
    if (expression != NULL) expression->accept(visitor);
}


void AST::CallStatement::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (ident != NULL) ident->accept(visitor);
    if (actualParameters != NULL) actualParameters->accept(visitor);
}


void AST::ActualParameters::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    std::list<AST::Expression*>::iterator it;
    for( it = expressions.begin(); it != expressions.end(); it++ )
       if (*it != NULL) (*it)->accept(visitor);
    if (lastParameter != NULL) lastParameter->accept(visitor);
}


void AST::ActualParameters::writeParameterInList()
{
   if (lastParameter == NULL) return;
   expressions.push_back(lastParameter);
   lastParameter = NULL;
}


int AST::ActualParameters::getNrParameters()
{
   return expressions.size();
}


std::list<AST::Expression*> AST::ActualParameters::getParameterExpressions()
{
   writeParameterInList();
   return expressions;
}


void AST::CompoundStatement::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    std::list<AST::Statement*>::iterator it;
    for( it = statements.begin(); it != statements.end(); it++ )
       if (*it != NULL) (*it)->accept(visitor);
    if (lastStatement != NULL) lastStatement->accept(visitor);
}


std::list<AST::Statement*>  AST::CompoundStatement::getStatements()
{
    writeStatementInList();
    return statements;
}


void AST::CompoundStatement::writeStatementInList()
{
   if (lastStatement == NULL) return;

   statements.push_back(lastStatement);
   lastStatement = NULL;
}


void AST::IfStatement::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (condition != NULL) condition->accept(visitor);
    if (statement != NULL) statement->accept(visitor);
}


void AST::InputStatement::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (ident != NULL) ident->accept(visitor);
}


void AST::OutputStatement::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (expression != NULL) expression->accept(visitor);
}


void AST::WhileStatement::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (condition != NULL) condition->accept(visitor);
    if (statement != NULL) statement->accept(visitor);
}


void AST::OddCondition::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (expression != NULL) expression->accept(visitor);
}


void AST::EqualCondition::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (leftExpression != NULL) leftExpression->accept(visitor);
    if (rightExpression != NULL) rightExpression->accept(visitor);
}


void AST::NotEqualCondition::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (leftExpression != NULL) leftExpression->accept(visitor);
    if (rightExpression != NULL) rightExpression->accept(visitor);
}


void AST::GreaterCondition::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (leftExpression != NULL) leftExpression->accept(visitor);
    if (rightExpression != NULL) rightExpression->accept(visitor);
}


void AST::GreaterEqualCondition::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (leftExpression != NULL) leftExpression->accept(visitor);
    if (rightExpression != NULL) rightExpression->accept(visitor);
}


void AST::LessCondition::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (leftExpression != NULL) leftExpression->accept(visitor);
    if (rightExpression != NULL) rightExpression->accept(visitor);
}


void AST::LessEqualCondition::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (leftExpression != NULL) leftExpression->accept(visitor);
    if (rightExpression != NULL) rightExpression->accept(visitor);
}


void AST::PlusExpression::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (firstTerm != NULL) firstTerm->accept(visitor);
    if (additionalExpression != NULL) additionalExpression->accept(visitor);
}


void AST::MinusExpression::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (firstTerm != NULL) firstTerm->accept(visitor);
    if (additionalExpression != NULL) additionalExpression->accept(visitor);
}


void AST::FirstTermExpression::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (firstTerm != NULL) firstTerm->accept(visitor);
}


void AST::PlusAdditionalExpression::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (term != NULL) term->accept(visitor);
    if (additionalExpression != NULL) additionalExpression->accept(visitor);
}


void AST::MinusAdditionalExpression::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (term != NULL) term->accept(visitor);
    if (additionalExpression != NULL) additionalExpression->accept(visitor);
}


void AST::TermAdditionalExpression::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (term != NULL) term->accept(visitor);
}


void AST::PlusFirstTerm::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (term != NULL) term->accept(visitor);
}


void AST::MinusFirstTerm::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (term != NULL) term->accept(visitor);
}


void AST::SignlessFirstTerm::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (term != NULL) term->accept(visitor);
}


void AST::MultTerm::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (factor != NULL) factor->accept(visitor);
    if (term != NULL) term->accept(visitor);
}


void AST::DivTerm::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (factor != NULL) factor->accept(visitor);
    if (term != NULL) term->accept(visitor);
}


void AST::FactorTerm::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (factor != NULL) factor->accept(visitor);
}


void AST::Ident::accept_visitor(ASTVisitor *visitor)
{
    if (visitor != NULL) visitor->visit(this);
}


void AST::Number::accept_visitor(ASTVisitor *visitor)
{
    if (visitor != NULL) visitor->visit(this);
}


void AST::ParenthesisFactor::accept_visitor(ASTVisitor *visitor)
{
    visitor->visit(this);
    if (expression != NULL) expression->accept(visitor);
}
