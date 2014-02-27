#ifndef PARSER_H
#define PARSER_H

#include <list>
#include <stack>
#include <pthread.h>
#include <iostream>

#include "../lexer/token.h"
#include "../lexer/pl0token.h"
#include "../common.h"
#include "../pl0controller.h"

#include "symboltable.h"

#include "ast.h"
#include "exceptions.h"

class PL0Controller;

class Parser
{
   public:
      Parser(PL0Controller *controller)
        : controller(controller), m_running(false), m_sleeping(false), m_stop(true),
          parseWithoutPause(true),
          return_code(SUCCESS),
          parse_depth(0)
      {}

      ~Parser()
      {
         abortParser();
      }

      void abortParser()
      {
         m_running = false;
         m_sleeping = false;
         m_stop = true;
         tokenList.clear();
         return_code = ABORTED;
      }

      void setTokenList(std::list<Token<enum Pl0TokenType> > tokenList)
      { this->tokenList = tokenList;  }

      AST::Program *getProgramTree() { return program; }
      void pausingAfterEachRule(bool pausing) { parseWithoutPause = !pausing; }

      void run()
      {
         if(!m_running) //start thread
         {
           pthread_create(&threadID, 0, &Parser::start_thread, this);
           m_running = true;
           m_stop = false;
         }
         else
         {
           m_sleeping = false;
         }
      }

      enum ParserReturn
      {
          SUCCESS,
          FINISH,
          EXCEPTION,
          ABORTED,
          UNKNOWN_FAULT
      };

      bool isRunning()  { return m_running;  }
      bool isSleeping() { return m_sleeping; }

      enum ParserReturn returnCode() { return return_code; }
      ParseException *getCatchedException() { return catchedException; }

   private:
      PL0Controller *controller;

      volatile bool m_running;
      volatile bool m_sleeping;
      volatile bool m_stop;

      pthread_t threadID;
      bool parseWithoutPause;
      enum ParserReturn return_code;
      ParseException *catchedException;

      static void* start_thread(void *obj)
      {
        reinterpret_cast<Parser*>(obj)->parse();
        reinterpret_cast<Parser*>(obj)->m_running = false;
        return 0;
      }

      std::list<Token<enum Pl0TokenType> > tokenList;
      AST::Program *program;

      std::stack<SymbolTable*> symbolTableStack;
      SymbolTable* currentSymbolTable()
      {
          if (symbolTableStack.empty()) return NULL;
          return symbolTableStack.top();
      }
      int nextSymbolTableLevel() { return symbolTableStack.size(); }

      void beginParsing(std::string ruleName);
      void endParsing(std::string ruleName);
      int parse_depth;

      bool nextToken(Token<enum Pl0TokenType> *token);
      bool lookAHead(Token<enum Pl0TokenType> *token);

      bool parse();
      bool parse_program(AST::Program **program);
      bool parse_block(AST::Block **block);
      bool parse_constants(AST::Constants **constants);
      bool parse_variables(AST::Variables **variables);
      bool parse_procedures(AST::Procedures **procedures);
      bool parse_constant_declaration(AST::ConstantDeclaration **constantDeclaration);
      bool parse_variable_declaration(AST::VariableDeclaration **variableDeclaration);
      bool parse_procedure_declaration(AST::ProcedureDeclaration **procedureDeclaration);
      bool parse_formal_parameters(AST::FormalParameters **formalParameters);
      bool parse_number(AST::Number **number);
      bool parse_ident(AST::Ident **ident);
      bool parse_statement(AST::Statement **statement);
      bool parse_assignment_statement(AST::AssignmentStatement **assignmentStatement);
      bool parse_call_statement(AST::CallStatement **callStatement);
      bool parse_actual_parameters(AST::ActualParameters **actualParameters);
      bool parse_compound_statement(AST::CompoundStatement **compoundStatement);
      bool parse_if_statement(AST::IfStatement **ifStatement);
      bool parse_input_statement(AST::InputStatement **inputStatement);
      bool parse_output_statement(AST::OutputStatement **outputStatement);
      bool parse_while_statement(AST::WhileStatement **whileStatement);
      bool parse_expression(AST::Expression **expression);
      bool parse_firstterm(AST::FirstTerm **firstTerm);
      bool parse_additional_expression(AST::AdditionalExpression **additionalExpression);
      bool parse_term(AST::Term **term);
      bool parse_factor(AST::Factor **factor);
      bool parse_parenthesis_factor(AST::ParenthesisFactor **parenthesisFactor);
      bool parse_condition(AST::Condition **condition);
      bool parse_odd_condition(AST::OddCondition **oddCondition);
      bool parse_compare_condition(AST::CompareCondition **compareCondition);
};


#endif
