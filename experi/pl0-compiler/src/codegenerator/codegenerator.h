#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <list>

#include "../parser/ast.h"
#include "../parser/symboltable.h"
#include "instruction.h"

class CodeGenerator
{
   public:
      CodeGenerator(): program(NULL), label(0), gen_depth(0) {}
      void setProgram(AST::Program *program) { this->program = program; }
      std::list<instruction_t> generate();

   private:
      AST::Program *program;
      int label;
      int nextLabel() { label = label +1;
                        return label; }

      std::list<instruction_t> generate(bool isMainBlock, AST::Block *block, SymbolTable *symbolTable);
      std::list<instruction_t> generate(AST::Variables *variables);
      std::list<instruction_t> generate(AST::Procedures *procedures);
      std::list<instruction_t> generate(AST::ProcedureDeclaration *procedure);

      std::list<instruction_t> generate(AST::Statement *statement, SymbolTable *symbolTable);
      std::list<instruction_t> generate_assignment_statement(AST::AssignmentStatement *statement, SymbolTable *symbolTable);
      std::list<instruction_t> generate_call_statement(AST::CallStatement *statement, SymbolTable *symbolTable);
      std::list<instruction_t> generate_compound_statement(AST::CompoundStatement *statement, SymbolTable *symbolTable);
      std::list<instruction_t> generate_if_statement(AST::IfStatement *statement, SymbolTable *symbolTable);
      std::list<instruction_t> generate_input_statement(AST::InputStatement *statement, SymbolTable *symbolTable);
      std::list<instruction_t> generate_output_statement(AST::OutputStatement *statement, SymbolTable *symbolTable);
      std::list<instruction_t> generate_while_statement(AST::WhileStatement *statement, SymbolTable *symbolTable);

      std::list<instruction_t> generate(AST::Expression *expression, SymbolTable *symbolTable);
      std::list<instruction_t> generate_AdditionalExpression(AST::AdditionalExpression *additionalExpression, SymbolTable *symbol);
      std::list<instruction_t> generate_Term(AST::Term *term, SymbolTable *symbolTable);
      std::list<instruction_t> generate_FirstTerm(AST::FirstTerm *firstTerm, SymbolTable *symbolTable);
      std::list<instruction_t> generate_Factor(AST::Factor *factor, SymbolTable *symbolTable);

      std::list<instruction_t> generate(AST::Condition *condition, SymbolTable *symbolTable);
      std::list<instruction_t> generate_oddCondition(AST::OddCondition *condition, SymbolTable *symbolTable);
      std::list<instruction_t> generate_compareCondition(AST::CompareCondition *condition, SymbolTable *symbolTable);



      std::list<instruction_t> concat(std::list<instruction_t> list_a, std::list<instruction_t> list_b);
      std::string intToString(int i);

      void goIn(std::string tree);
      void goOut(std::string tree);
      int gen_depth;

      std::string getLabelName(SymbolTable *s);
};


#endif
