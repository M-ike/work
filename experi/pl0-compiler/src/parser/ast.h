#ifndef ASTTREE_H
#define ASTTREE_H

#include <list>
#include <string>
#include <iostream>

#include "ast_visitor.h"

class ASTVisitor;
class SymbolTable;

#include "ast_classes.h"
#include "../common.h"
#include "symboltable.h"

namespace AST
{
     class ASTTree
     {
        public:
           position_t startPosition, endPosition;
           virtual void setProperties(position_t startPosition, position_t endPosition)
           {
               this->startPosition = startPosition;
               this->endPosition = endPosition;
           }
           virtual void accept_visitor(ASTVisitor *visitor) = 0;
           void accept(ASTVisitor *visitor);
        protected:
           ASTTree() { startPosition.column=0; startPosition.row=0; endPosition.column=0; endPosition.row=0; }
           virtual ~ASTTree() {}
     };


     class Program: public ASTTree
     {
        public:
          Block *block;
          SymbolTable *symbolTable;
          void accept_visitor(ASTVisitor *visitor);
          Program(): ASTTree(), block(NULL), symbolTable(NULL) {}
     };


     class Block: public ASTTree
     {
         public:
            Constants *constants;
            Variables *variables;
            Procedures *procedures;
            Statement *statement;
            void accept_visitor(ASTVisitor *visitor);
            Block(): ASTTree(), constants(NULL), variables(NULL), procedures(NULL), statement(NULL) {}
     };


     class Constants: public ASTTree
     {
        public:
           ConstantDeclaration* lastConstant; //not in List until writeConstantInList()
           void accept_visitor(ASTVisitor *visitor);
           Constants(): ASTTree(), lastConstant(NULL) {}
           void writeConstantInList();
        private:
           std::list<ConstantDeclaration*> constantsDeclarartions;
     };


     class ConstantDeclaration: public ASTTree
     {
        public:
          Ident *ident;
          Number *number;
          void accept_visitor(ASTVisitor *visitor);
          ConstantDeclaration(): ASTTree(), ident(NULL), number(NULL) {}
     };


     class Variables: public ASTTree
     {
        public:
           VariableDeclaration* lastVariable; //not in List until writeVariableInList()
           void accept_visitor(ASTVisitor *visitor);
           Variables(): ASTTree(), lastVariable(NULL) {}
           void writeVariableInList();
           int getNr();
        private:
           std::list<VariableDeclaration*> variablesDeclarartions;
     };


     class VariableDeclaration: public ASTTree
     {
        public:
           Ident *ident;
           void accept_visitor(ASTVisitor *visitor);
           VariableDeclaration(): ASTTree(), ident(NULL) {}
     };


     class Procedures: public ASTTree
     {
        public:
           ProcedureDeclaration* lastProcedure; //not in List until writeProcedureInList()
           void accept_visitor(ASTVisitor *visitor);
           Procedures(): ASTTree(), lastProcedure(NULL) {}
           void writeProcedureInList();
           std::list<ProcedureDeclaration*> getProcedures();
        private:
           std::list<ProcedureDeclaration*> proceduresDeclarartions;
     };


     class ProcedureDeclaration: public ASTTree
     {
         public:
            Ident *ident;
            FormalParameters *formalParameters;
            Block *block;
            SymbolTable *symbolTable;
            void accept_visitor(ASTVisitor *visitor);
            ProcedureDeclaration(): ASTTree(), ident(NULL), formalParameters(NULL), block(NULL), symbolTable(NULL) {}
     };


     class FormalParameters: public ASTTree
     {
         public:
            VariableDeclaration *lastParameter;  //not in List until writeParameterInList()
            void accept_visitor(ASTVisitor *visitor);
            FormalParameters(): ASTTree(), lastParameter(NULL) {}
            void writeParameterInList();
            int getNrParameters();
        private:
            std::list<VariableDeclaration*> variables;
     };


     class Statement: public ASTTree
     {
       public:
           virtual void accept_visitor(ASTVisitor *visitor) = 0;
       protected:
           Statement(): ASTTree() {}
     };


     class AssignmentStatement: public Statement
     {
        public:
           Ident *ident;
           Expression *expression;
           virtual void accept_visitor(ASTVisitor *visitor);
           AssignmentStatement(): Statement(), ident(NULL), expression(NULL) {}
     };


     class CallStatement: public Statement
     {
        public:
           Ident *ident;
           ActualParameters *actualParameters;
           virtual void accept_visitor(ASTVisitor *visitor);
           CallStatement(): Statement(), ident(NULL), actualParameters(NULL) {}
     };


     class ActualParameters: public ASTTree
     {
         public:
            Expression *lastParameter;  //not in List until writeParameterInList()
            virtual void accept_visitor(ASTVisitor *visitor);
            ActualParameters(): ASTTree(), lastParameter(NULL) {}
            void writeParameterInList();
            int getNrParameters();
            std::list<Expression*> getParameterExpressions();
         private:
             std::list<Expression*> expressions;
     };


     class CompoundStatement: public Statement
     {
         public:
            Statement *lastStatement;  //not in List until writeStatementInList()
            virtual void accept_visitor(ASTVisitor *visitor);
            CompoundStatement(): Statement(), lastStatement(NULL) {}
            void writeStatementInList();
            std::list<Statement*> getStatements();
         private:
            std::list<Statement*> statements;
     };


     class IfStatement: public Statement
     {
        public:
           Condition *condition;
           Statement *statement;
           virtual void accept_visitor(ASTVisitor *visitor);
           IfStatement(): Statement(), condition(NULL), statement(NULL) {}
     };


     class InputStatement: public Statement
     {
        public:
           Ident *ident;
           virtual void accept_visitor(ASTVisitor *visitor);
           InputStatement(): Statement(), ident(NULL) {}
     };


     class OutputStatement: public Statement
     {
        public:
           Expression *expression;
           virtual void accept_visitor(ASTVisitor *visitor);
           OutputStatement(): Statement(), expression(NULL) {}
     };


     class WhileStatement: public Statement
     {
        public:
           Condition *condition;
           Statement *statement;
           virtual void accept_visitor(ASTVisitor *visitor);
           WhileStatement(): Statement(), condition(NULL), statement(NULL) {}
     };


     class Condition: public ASTTree
     {
        public:
          virtual void accept_visitor(ASTVisitor *visitor) = 0;

        protected:
           Condition(): ASTTree() {}
     };


     class OddCondition: public Condition
     {
         public:
             Expression *expression;
             void accept_visitor(ASTVisitor *visitor);
             OddCondition(): Condition(), expression(NULL) {}
     };


     class CompareCondition: public Condition
     {
        public:
           Expression *leftExpression;
           Expression *rightExpression;
           virtual void accept_visitor(ASTVisitor *visitor) = 0;

        protected:
           CompareCondition(): Condition(), leftExpression(NULL), rightExpression(NULL) {}
     };


     class EqualCondition: public CompareCondition
     {
        public:
           void accept_visitor(ASTVisitor *visitor);
           EqualCondition(): CompareCondition() {}
     };


     class NotEqualCondition: public CompareCondition
     {
        public:
           void accept_visitor(ASTVisitor *visitor);
           NotEqualCondition(): CompareCondition() {}
     };


     class GreaterCondition: public CompareCondition
     {
        public:
           void accept_visitor(ASTVisitor *visitor);
           GreaterCondition(): CompareCondition() {}
     };


     class GreaterEqualCondition: public CompareCondition
     {
         public:
            void accept_visitor(ASTVisitor *visitor);
            GreaterEqualCondition(): CompareCondition() {}
     };


     class LessCondition: public CompareCondition
     {
        public:
           void accept_visitor(ASTVisitor *visitor);
           LessCondition(): CompareCondition() {}
     };


     class LessEqualCondition: public CompareCondition
     {
        public:
           void accept_visitor(ASTVisitor *visitor);
           LessEqualCondition(): CompareCondition() {}
     };


     class Expression: public ASTTree
     {
        public:
           FirstTerm *firstTerm;
           virtual void accept_visitor(ASTVisitor *visitor) = 0;
        protected:
           Expression(): ASTTree(), firstTerm(NULL) {}
     };


     class PlusExpression: public Expression
     {
        public:
           AdditionalExpression *additionalExpression;
           void accept_visitor(ASTVisitor *visitor);
           PlusExpression(): Expression(), additionalExpression(NULL) {}
     };


     class MinusExpression: public Expression
     {
        public:
           AdditionalExpression *additionalExpression;
           void accept_visitor(ASTVisitor *visitor);
           MinusExpression(): Expression(), additionalExpression(NULL) {}
     };


     class FirstTermExpression: public Expression
     {
        public:
           void accept_visitor(ASTVisitor *visitor);
           FirstTermExpression(): Expression() {}
     };


     class AdditionalExpression: public ASTTree
     {
        public:
           Term *term;
           virtual void accept_visitor(ASTVisitor *visitor) = 0;
        protected:
           AdditionalExpression(): ASTTree(), term(NULL) {}
     };


     class PlusAdditionalExpression: public AdditionalExpression
     {
        public:
            AdditionalExpression *additionalExpression;
            void accept_visitor(ASTVisitor *visitor);
            PlusAdditionalExpression(): AdditionalExpression(), additionalExpression(NULL) {}
     };


     class MinusAdditionalExpression: public AdditionalExpression
     {
        public:
           AdditionalExpression *additionalExpression;
           void accept_visitor(ASTVisitor *visitor);
           MinusAdditionalExpression(): AdditionalExpression(), additionalExpression(NULL) {}
     };


     class TermAdditionalExpression: public AdditionalExpression
     {
        public:
           void accept_visitor(ASTVisitor *visitor);
           TermAdditionalExpression(): AdditionalExpression() {}
     };


     class FirstTerm: public ASTTree
     {
        public:
           Term *term;
           virtual void accept_visitor(ASTVisitor *visitor) = 0;
        protected:
           FirstTerm(): ASTTree(), term(NULL) {}
     };


     class PlusFirstTerm: public FirstTerm
     {
        public:
           void accept_visitor(ASTVisitor *visitor);
           PlusFirstTerm(): FirstTerm() {}
     };


     class MinusFirstTerm: public FirstTerm
     {
        public:
           void accept_visitor(ASTVisitor *visitor);
           MinusFirstTerm(): FirstTerm() {}
     };


     class SignlessFirstTerm: public FirstTerm
     {
        public:
           void accept_visitor(ASTVisitor *visitor);
           SignlessFirstTerm(): FirstTerm() {}
     };


     class Term: public ASTTree
     {
        public:
           Factor *factor;
           virtual void accept_visitor(ASTVisitor *visitor) = 0;
        protected:
           Term(): ASTTree(), factor(NULL) {}
     };


     class MultTerm: public Term
     {
        public:
           Term *term;
           void accept_visitor(ASTVisitor *visitor);
           MultTerm(): Term(), term(NULL) {}
     };


     class DivTerm: public Term
     {
        public:
           Term *term;
           void accept_visitor(ASTVisitor *visitor);
           DivTerm(): Term(), term(NULL) {}
     };


     class FactorTerm: public Term
     {
        public:
           void accept_visitor(ASTVisitor *visitor);
           FactorTerm(): Term() {}
     };


     class Factor: public ASTTree
     {
        public:
           virtual void accept_visitor(ASTVisitor *visitor) = 0;
        protected:
           Factor(): ASTTree() {}
     };


     class Ident: public Factor
     {
        public:
           std::string value;
           void accept_visitor(ASTVisitor *visitor);
           Ident(): Factor(), value(std::string("<NO_VALUE>")) {}
     };


     class Number: public Factor
     {
        public:
           int value;
           void accept_visitor(ASTVisitor *visitor);
           Number(): Factor() {}
     };


     class ParenthesisFactor: public Factor
     {
         public:
             Expression *expression;
             void accept_visitor(ASTVisitor *visitor);
             ParenthesisFactor(): Factor(), expression(NULL) {}
     };


} //namespace

#endif
