#include "parser.h"
#include <iostream>
#include <sstream>
#include <unistd.h>

bool Parser::parse()
{
    beginParsing("file");

    parse_depth = 0;

    try
    {

        program = NULL;

        if (!parse_program(&program)) return false;

        Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
        nextToken(token);

        if (token->type != T_EOF)  throw UnexpectedSymbolException(token);
        return_code = FINISH;
    }
    catch (UndefinedVariableException uve)
    {
      return_code = EXCEPTION;
      catchedException = new UndefinedVariableException(uve.ident);
    }
    catch (UndefinedProcedureException upe)
    {
      return_code = EXCEPTION;
      catchedException = new UndefinedProcedureException(upe.ident, upe.nrParameters);
    }
    catch (UndefinedSymbolException use)
    {
      return_code = EXCEPTION;
      catchedException = new UndefinedSymbolException(use.ident);
    }
    catch (UnexpectedSymbolException use)
    {
      return_code = EXCEPTION;
      catchedException = new UnexpectedSymbolException(use.token);
    }
    catch (MultipleDefinitionException mde)
    {
      return_code = EXCEPTION;
      catchedException = new MultipleDefinitionException(mde.ident, mde.nrParameters);
    }
    catch (StopException se)
    {
      return_code = ABORTED;
      catchedException = new StopException();
    }
    catch (...)
    {
      return_code = UNKNOWN_FAULT;
      catchedException = NULL;
    }

    endParsing("file");

    //reset Parser
    m_running = false;
    m_sleeping = false;
    return true;
}

//Program -> Block "."
bool Parser::parse_program(AST::Program **program)
{
   beginParsing("program");

   *program = new AST::Program();
   (*program)->symbolTable = new SymbolTable(currentSymbolTable(), nextSymbolTableLevel() );
   symbolTableStack.push((*program)->symbolTable);
   currentSymbolTable()->setName(std::string("program"));

   if (!parse_block(&((*program)->block))) return false;

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_DOT) throw UnexpectedSymbolException(token);

   token->position.column += token->value.length() -1;
   (*program)->setProperties((*program)->block->startPosition, token->position);

   symbolTableStack.pop();
   endParsing("program");
   return true;
}


//Block -> [Constants] [Variables] [Procedures] Statement
bool Parser::parse_block(AST::Block **block)
{
   beginParsing("block");

   *block = new AST::Block();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();

   position_t startPosition; startPosition.row=0;

   //Constants
   lookAHead(token);
   if (token->type == T_CONST)
   {
      if (!parse_constants(&((*block)->constants))) return false;
      if (startPosition.row==0) startPosition = (*block)->constants->startPosition;
   }

   //Variables
   lookAHead(token);
   if (token->type == T_VAR)
   {
      if (!parse_variables(&((*block)->variables))) return false;
      if (startPosition.row==0) startPosition = (*block)->variables->startPosition;
   }

    //Procedures
   lookAHead(token);
   if (token->type == T_PROCEDURE)
   {
      if (!parse_procedures(&((*block)->procedures))) return false;
      if (startPosition.row==0) startPosition = (*block)->procedures->startPosition;
   }

   //Statement
   if (!parse_statement(&((*block)->statement))) return false;
   if (startPosition.row==0) startPosition = (*block)->statement->startPosition;

   (*block)->setProperties(startPosition, (*block)->statement->endPosition);

   endParsing("block");
   return true;
}


//Constants -> "CONST" ConstantDeclaration ("," ConstantDeclaration)*";"
bool Parser::parse_constants(AST::Constants **constants)
{
   beginParsing("constants");

   *constants = new AST::Constants();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_CONST) throw UnexpectedSymbolException(token);

   position_t startPosition = token->position;

   if (!parse_constant_declaration(&((*constants)->lastConstant))) return false;
   if (!currentSymbolTable()->registerSymbol((*constants)->lastConstant)) throw MultipleDefinitionException((*constants)->lastConstant->ident);
   (*constants)->writeConstantInList();

   lookAHead(token);
   while (token->type == T_COMMA)
   {
      nextToken(token);
      if (!parse_constant_declaration(&((*constants)->lastConstant))) return false;
      if (!currentSymbolTable()->registerSymbol((*constants)->lastConstant)) throw MultipleDefinitionException((*constants)->lastConstant->ident);
      (*constants)->writeConstantInList();
      lookAHead(token);
   }

   nextToken(token);
   if (token->type != T_SEMICOLON) throw UnexpectedSymbolException(token);

   token->position.column += token->value.length() -1;
   (*constants)->setProperties(startPosition, token->position);

   endParsing("constants");
   return true;
}


//Variables -> "VAR" VariableDeclaration ("," VariableDeclaration)*";"
bool Parser::parse_variables(AST::Variables **variables)
{
   beginParsing("variables");

   *variables = new AST::Variables();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_VAR) throw UnexpectedSymbolException(token);

   position_t startPosition = token->position;

   if (!parse_variable_declaration(&((*variables)->lastVariable))) return false;
   if (!currentSymbolTable()->registerSymbol((*variables)->lastVariable)) throw MultipleDefinitionException((*variables)->lastVariable->ident);
   (*variables)->writeVariableInList();

   lookAHead(token);
   while (token->type == T_COMMA)
   {
      nextToken(token);
      if (!parse_variable_declaration(&((*variables)->lastVariable))) return false;
      if (!currentSymbolTable()->registerSymbol((*variables)->lastVariable)) throw MultipleDefinitionException((*variables)->lastVariable->ident);
      (*variables)->writeVariableInList();
      lookAHead(token);
   }

   nextToken(token);
   if (token->type != T_SEMICOLON) throw UnexpectedSymbolException(token);

   token->position.column += token->value.length() -1;
   (*variables)->setProperties(startPosition, token->position);

   endParsing("variables");
   return true;
}


//Procedures -> (ProcedureDeclaration)+
bool Parser::parse_procedures(AST::Procedures **procedures)
{
   beginParsing("procedures");

   *procedures = new AST::Procedures();

   if (!parse_procedure_declaration(&((*procedures)->lastProcedure))) return false;
   if (!currentSymbolTable()->registerSymbol((*procedures)->lastProcedure))
   {
       int nrP = 0;
       if ((*procedures)->lastProcedure->formalParameters != NULL)
           nrP = (*procedures)->lastProcedure->formalParameters->getNrParameters();
       throw MultipleDefinitionException((*procedures)->lastProcedure->ident,nrP);
   }
   position_t startPosition = (*procedures)->lastProcedure->startPosition;
   position_t endPosition = (*procedures)->lastProcedure->endPosition;
   (*procedures)->writeProcedureInList();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   lookAHead(token);
   while (token->type == T_PROCEDURE)
   {
      if (!parse_procedure_declaration(&((*procedures)->lastProcedure))) return false;
      if (!currentSymbolTable()->registerSymbol((*procedures)->lastProcedure))
      {
          int nrP = 0;
          if ((*procedures)->lastProcedure->formalParameters != NULL)
              nrP = (*procedures)->lastProcedure->formalParameters->getNrParameters();
          throw MultipleDefinitionException((*procedures)->lastProcedure->ident,nrP);
      }
      endPosition = (*procedures)->lastProcedure->endPosition;
      (*procedures)->writeProcedureInList();
      lookAHead(token);
   }

   (*procedures)->setProperties(startPosition, endPosition);

   endParsing("procedures");
   return true;
}


//ConstantDeclaration -> Ident "=" Number
bool Parser::parse_constant_declaration(AST::ConstantDeclaration **constantDeclaration)
{
   beginParsing("constantDeclaration");

   *constantDeclaration = new AST::ConstantDeclaration();

   if (!parse_ident(&((*constantDeclaration)->ident))) return false;

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_EQUAL) throw UnexpectedSymbolException(token);

   if (!parse_number(&((*constantDeclaration)->number))) return false;

   (*constantDeclaration)->setProperties((*constantDeclaration)->ident->startPosition, (*constantDeclaration)->number->endPosition);

   endParsing("constantDeclaration");
   return true;
}


//VariableDeclaration -> Ident
bool Parser::parse_variable_declaration(AST::VariableDeclaration **variableDeclaration)
{
   beginParsing("variableDeclaration");

   *variableDeclaration = new AST::VariableDeclaration();

   if (!parse_ident(&((*variableDeclaration)->ident))) return false;

   (*variableDeclaration)->setProperties((*variableDeclaration)->ident->startPosition, (*variableDeclaration)->ident->endPosition);

   endParsing("variableDeclaration");
   return true;
}


//ProcedureDeclaration -> "PROCEDURE" Ident "(" [FormalParameters] ")" Block ";"
bool Parser::parse_procedure_declaration(AST::ProcedureDeclaration **procedureDeclaration)
{
   beginParsing("procedureDeclaration");

   *procedureDeclaration = new AST::ProcedureDeclaration();
   SymbolTable *localSymbolTable = new SymbolTable(currentSymbolTable(), nextSymbolTableLevel() );
   symbolTableStack.push(localSymbolTable);

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();

   nextToken(token);
   if (token->type != T_PROCEDURE) throw UnexpectedSymbolException(token);

   position_t startPosition = token->position;

   if (!parse_ident(&((*procedureDeclaration)->ident))) return false;

   nextToken(token);
   if (token->type != T_OPENPARENTHESIS) throw UnexpectedSymbolException(token);

   lookAHead(token);
   if (token->type != T_CLOSEPARENTHESIS)
   {
      if (!parse_formal_parameters(&((*procedureDeclaration)->formalParameters))) return false;
   }

   nextToken(token);
   if (token->type != T_CLOSEPARENTHESIS) throw UnexpectedSymbolException(token);

   (*procedureDeclaration)->symbolTable = localSymbolTable;

   if (!currentSymbolTable()->registerSymbol(*procedureDeclaration)) return false;

   if ((*procedureDeclaration)->formalParameters == NULL)
      currentSymbolTable()->setName((*procedureDeclaration)->ident->value + "(0)");
   else
   {
      std::ostringstream stringStream;
      stringStream << (*procedureDeclaration)->formalParameters->getNrParameters();
      currentSymbolTable()->setName((*procedureDeclaration)->ident->value + "(" + stringStream.str() + ")");
   }

   if (!parse_block(&((*procedureDeclaration)->block))) return false;

   nextToken(token);
   if (token->type != T_SEMICOLON) throw UnexpectedSymbolException(token);

   token->position.column += token->value.length() -1;
   (*procedureDeclaration)->setProperties(startPosition, token->position);

   symbolTableStack.pop();
   endParsing("procedureDeclaration");
   return true;
}


//FormalParameters -> VariableDeclaration ("," VariableDeclaration)*
bool Parser::parse_formal_parameters(AST::FormalParameters **formalParameters)
{
   beginParsing("formalParameters");

   *formalParameters = new AST::FormalParameters();

   if (!parse_variable_declaration(&((*formalParameters)->lastParameter))) return false;
   if (!currentSymbolTable()->registerSymbol((*formalParameters)->lastParameter)) return false;
   position_t startPosition = (*formalParameters)->lastParameter->startPosition;
   position_t endPosition =   (*formalParameters)->lastParameter->endPosition;
   (*formalParameters)->writeParameterInList();


   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   lookAHead(token);
   while (token->type == T_COMMA)
   {
      nextToken(token);
      if (!parse_variable_declaration(&((*formalParameters)->lastParameter))) return false;
      if (!currentSymbolTable()->registerSymbol((*formalParameters)->lastParameter)) return false;
      endPosition = (*formalParameters)->lastParameter->endPosition;
      (*formalParameters)->writeParameterInList();
      lookAHead(token);
   }

   (*formalParameters)->setProperties(startPosition, endPosition);

   endParsing("formalParameters");
   return true;
}


bool Parser::parse_ident(AST::Ident **ident)
{
   beginParsing("ident");

   *ident = new AST::Ident();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_IDENT) throw UnexpectedSymbolException(token);

   (*ident)->value = token->value;

   position startPosition = token->position; token->position.column += token->value.length() -1;
   (*ident)->setProperties(startPosition, token->position);

   endParsing("ident " + token->value);
   return true;
}


bool Parser::parse_number(AST::Number **number)
{
   beginParsing("number");

   *number = new AST::Number();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_NUMBER) throw UnexpectedSymbolException(token);

   (*number)->value = atoi(token->value.c_str());

   position startPosition = token->position; token->position.column += token->value.length() -1;
   (*number)->setProperties(startPosition, token->position);

   endParsing("number " + token->value);
   return true;
}


//Statement -> AssignmentStatement | CallStatement | CompoundStatement | IfStatement | WhileStatement
bool Parser::parse_statement(AST::Statement **statement)
{
   beginParsing("statement");

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   lookAHead(token);

   if (token->type == T_CALL)
      { if (!parse_call_statement((AST::CallStatement**)statement)) return false; }

   else if (token->type == T_QUESTIONMARK)
      { if (!parse_input_statement((AST::InputStatement**)statement)) return false; }

   else if (token->type == T_EXCLAMATIOMARK)
      { if (!parse_output_statement((AST::OutputStatement**)statement)) return false; }

   else if (token->type == T_BEGIN)
      { if (!parse_compound_statement((AST::CompoundStatement**)statement)) return false; }

   else if (token->type == T_IF)
      { if (!parse_if_statement((AST::IfStatement**)statement)) return false; }

   else if (token->type == T_WHILE)
      { if (!parse_while_statement((AST::WhileStatement**)statement)) return false; }

   else if (token->type == T_IDENT)
      { if (!parse_assignment_statement((AST::AssignmentStatement**)statement)) return false; }
   else
      { throw UnexpectedSymbolException(token); }

   endParsing("statement");
   return true;
}


//AssignmentStatement -> Ident ":=" Expression
bool Parser::parse_assignment_statement(AST::AssignmentStatement **assignmentStatement)
{
   beginParsing("assignmentStatement");

   *assignmentStatement = new AST::AssignmentStatement();

   if (!parse_ident(&((*assignmentStatement)->ident))) return false;
   if (!currentSymbolTable()->registerReferencedVariable((*assignmentStatement)->ident->value)) throw UndefinedVariableException((*assignmentStatement)->ident);

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_ASSIGN) throw UnexpectedSymbolException(token);

   if (!parse_expression(&((*assignmentStatement)->expression))) return false;

   (*assignmentStatement)->setProperties((*assignmentStatement)->ident->startPosition, (*assignmentStatement)->expression->endPosition);

   endParsing("assignmentStatement");
   return true;
}


//CallStatement -> "CALL" Ident "(" [ActualParameters] ")"
bool Parser::parse_call_statement(AST::CallStatement **callStatement)
{
   beginParsing("callStatement");

   *callStatement = new AST::CallStatement();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_CALL) throw UnexpectedSymbolException(token);

   position_t startPosition = token->position;

   if (!parse_ident(&((*callStatement)->ident))) return false;

   nextToken(token);
   if (token->type != T_OPENPARENTHESIS) throw UnexpectedSymbolException(token);

   lookAHead(token);
   if (token->type != T_CLOSEPARENTHESIS)
   {
      if (!parse_actual_parameters(&((*callStatement)->actualParameters))) return false;
   }

   nextToken(token);
   if (token->type != T_CLOSEPARENTHESIS) throw UnexpectedSymbolException(token);

   int nrParameters = 0;
   if ((*callStatement)->actualParameters != NULL) nrParameters = (*callStatement)->actualParameters->getNrParameters();
   if (!currentSymbolTable()->registerReferencedProcedure((*callStatement)->ident->value, nrParameters)) throw UndefinedProcedureException((*callStatement)->ident, nrParameters);

   token->position.column += token->value.length() -1;
   (*callStatement)->setProperties(startPosition, token->position);

   endParsing("callStatement");
   return true;
}


//ActualParameters -> Expression ("," Expression)*
bool Parser::parse_actual_parameters(AST::ActualParameters **actualParameters)
{
   beginParsing("actualParameters");

   *actualParameters = new AST::ActualParameters();

   if (!parse_expression(&((*actualParameters)->lastParameter))) return false;
   position_t startPosition = (*actualParameters)->lastParameter->startPosition;
   position_t endPosition =   (*actualParameters)->lastParameter->endPosition;
   (*actualParameters)->writeParameterInList();


   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   lookAHead(token);
   while (token->type == T_COMMA)
   {
      nextToken(token);
      if (!parse_expression(&((*actualParameters)->lastParameter))) return false;
      endPosition = (*actualParameters)->lastParameter->endPosition;
      (*actualParameters)->writeParameterInList();
      lookAHead(token);
   }

   (*actualParameters)->setProperties(startPosition, endPosition);

   endParsing("actualParameters");
   return true;
}


//CompoundStatement -> "BEGIN" Statement ( ";" Statement) "END"
bool Parser::parse_compound_statement(AST::CompoundStatement **compoundStatement)
{
   beginParsing("compoundStatement");

   *compoundStatement = new AST::CompoundStatement();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_BEGIN) throw UnexpectedSymbolException(token);

   position startPosition = token->position;

   if (!parse_statement(&((*compoundStatement)->lastStatement))) return false;
   (*compoundStatement)->writeStatementInList();


   lookAHead(token);
   while (token->type == T_SEMICOLON)
   {
      nextToken(token);
      if (!parse_statement(&((*compoundStatement)->lastStatement))) return false;
      (*compoundStatement)->writeStatementInList();
      lookAHead(token);
   }

   nextToken(token);
   if (token->type != T_END) throw UnexpectedSymbolException(token);

   token->position.column += token->value.length() -1;
   (*compoundStatement)->setProperties(startPosition, token->position);

   endParsing("compoundStatement");
   return true;
}


//IfStatement -> "IF" Condition "THEN" Statement
bool Parser::parse_if_statement(AST::IfStatement **ifStatement)
{
   beginParsing("ifStatement");

   *ifStatement = new AST::IfStatement();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_IF) throw UnexpectedSymbolException(token);

   position startPosition = token->position;

   if (!parse_condition(&((*ifStatement)->condition))) return false;

   nextToken(token);
   if (token->type != T_THEN) throw UnexpectedSymbolException(token);

   if (!parse_statement(&((*ifStatement)->statement))) return false;

   (*ifStatement)->setProperties(startPosition, (*ifStatement)->statement->endPosition);

   endParsing("ifStatement");
   return true;
}


//InputStatement -> "?" Ident
bool Parser::parse_input_statement(AST::InputStatement **inputStatement)
{
   beginParsing("inputStatement");

   *inputStatement = new AST::InputStatement();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_QUESTIONMARK) throw UnexpectedSymbolException(token);

   if (!parse_ident(&((*inputStatement)->ident))) return false;
   if (!currentSymbolTable()->registerReferencedVariable((*inputStatement)->ident->value)) throw UndefinedVariableException((*inputStatement)->ident);

   (*inputStatement)->setProperties(token->position, (*inputStatement)->ident->endPosition);

   endParsing("inputStatement");
   return true;
}


//OutputStatement -> "!" Expression
bool Parser::parse_output_statement(AST::OutputStatement **outputStatement)
{
   beginParsing("outputStatement");

   *outputStatement = new AST::OutputStatement();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_EXCLAMATIOMARK) throw UnexpectedSymbolException(token);

   if (!parse_expression(&((*outputStatement)->expression))) return false;

   (*outputStatement)->setProperties(token->position, (*outputStatement)->expression->endPosition);

   endParsing("outputStatement");
   return true;
}

//WhileStatement -> "WHILE" Condition "DO" Statement
bool Parser::parse_while_statement(AST::WhileStatement **whileStatement)
{
   beginParsing("whileStatement");

   *whileStatement = new AST::WhileStatement();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);
   if (token->type != T_WHILE) throw UnexpectedSymbolException(token);

   position startPosition = token->position;

   if (!parse_condition(&((*whileStatement)->condition))) return false;

   nextToken(token);
   if (token->type != T_DO) throw UnexpectedSymbolException(token);

   if (!parse_statement(&((*whileStatement)->statement))) return false;

   (*whileStatement)->setProperties(startPosition, (*whileStatement)->statement->endPosition);

   endParsing("whileStatement");
   return true;
}


//Expression -> FirstTerm [ ( "+" | "-" ) AdditionalExpression ]
bool Parser::parse_expression(AST::Expression **expression)
{
   beginParsing("expression");

   AST::FirstTerm *firstTerm;
   if (!parse_firstterm(&firstTerm)) return false;

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   lookAHead(token);

   position endPosition;

   if (token->type == T_PLUS)
   {
      nextToken(token);
      *expression = new AST::PlusExpression();
      AST::AdditionalExpression *additionalExpression;
      if (!parse_additional_expression(&additionalExpression)) return false;
      ((AST::PlusExpression*)(*expression))->additionalExpression = additionalExpression;
      endPosition = additionalExpression->endPosition;
   }
   else if (token->type == T_MINUS)
   {
      nextToken(token);
      *expression = new AST::MinusExpression();
      AST::AdditionalExpression *additionalExpression;
      if (!parse_additional_expression(&additionalExpression)) return false;
      ((AST::MinusExpression*)(*expression))->additionalExpression = additionalExpression;
      endPosition = additionalExpression->endPosition;
   }
   else
   {
      *expression = new AST::FirstTermExpression();
      endPosition = firstTerm->endPosition;
   }

   (*expression)->firstTerm = firstTerm;

   (*expression)->setProperties(firstTerm->startPosition, endPosition);

   endParsing("expression");
   return true;
}


//FirstTerm = ( "+" | "-" | eps ) Term
bool Parser::parse_firstterm(AST::FirstTerm **firstTerm)
{
   beginParsing("firstTerm");

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   lookAHead(token);

   position startPosition; startPosition.row = 0;

   if (token->type == T_PLUS)
   {
      nextToken(token);
      *firstTerm = new AST::PlusFirstTerm();
      startPosition = token->position;
   }
   else if (token->type == T_MINUS)
   {
      nextToken(token);
      *firstTerm = new AST::MinusFirstTerm();
      startPosition = token->position;
   }
   else
   {
      *firstTerm = new AST::SignlessFirstTerm();
   }

   if (!parse_term(&((*firstTerm)->term))) return false;

   if (startPosition.row == 0) startPosition = (*firstTerm)->term->startPosition;

   (*firstTerm)->setProperties(startPosition, (*firstTerm)->term->endPosition);

   endParsing("firstTerm");
   return true;
}


//AdditionalExpression -> Term [ ("+" | "-") AdditionalExpression ]
bool Parser::parse_additional_expression(AST::AdditionalExpression **additionalExpression)
{
   beginParsing("additionalExpression");

   AST::Term *term;
   if (!parse_term(&term)) return false;

   position endPosition = term->endPosition;

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   lookAHead(token);

   if (token->type == T_PLUS)
   {
      nextToken(token);
      *additionalExpression = new AST::PlusAdditionalExpression();
      AST::AdditionalExpression *additionalExpressionRecursive;
      if (!parse_additional_expression(&additionalExpressionRecursive)) return false;
      ((AST::PlusAdditionalExpression*)(*additionalExpression))->additionalExpression = additionalExpressionRecursive;
      endPosition = additionalExpressionRecursive->endPosition;
   }
   else if (token->type == T_MINUS)
   {
      nextToken(token);
      *additionalExpression = new AST::MinusAdditionalExpression();
      AST::AdditionalExpression *additionalExpressionRecursive;
      if (!parse_additional_expression(&additionalExpressionRecursive)) return false;
      ((AST::MinusAdditionalExpression*)(*additionalExpression))->additionalExpression = additionalExpressionRecursive;
      endPosition = additionalExpressionRecursive->endPosition;
   }
   else
   {
      *additionalExpression = new AST::TermAdditionalExpression();
   }

   (*additionalExpression)->term = term;

   (*additionalExpression)->setProperties(term->startPosition, endPosition);

   endParsing("additionalExpression");
   return true;
}


//Term -> Factor [ ("*" | "/") Term ]
bool Parser::parse_term(AST::Term **term)
{
   beginParsing("term");

   AST::Factor *factor;
   if (!parse_factor(&factor)) return false;

   position endPosition = factor->endPosition;

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   lookAHead(token);

   if (token->type == T_MULT)
   {
      nextToken(token);
      *term = new AST::MultTerm();
      AST::Term *termRecursive;
      if (!parse_term(&termRecursive)) return false;
      ((AST::MultTerm*)(*term))->term = termRecursive;
      endPosition = termRecursive->endPosition;
   }
   else if (token->type == T_DIV)
   {
      nextToken(token);
      *term = new AST::DivTerm();
      AST::Term *termRecursive;
      if (!parse_term(&termRecursive)) return false;
      ((AST::DivTerm*)(*term))->term = termRecursive;
      endPosition = termRecursive->endPosition;
   }
   else
   {
      *term = new AST::FactorTerm();
   }

   (*term)->factor = factor;

   (*term)->setProperties(factor->startPosition, endPosition);

   endParsing("term");
   return true;
}


//Factor -> Ident | Number | ParenthesisFactor
bool Parser::parse_factor(AST::Factor **factor)
{
   beginParsing("factor");

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   lookAHead(token);

   if (token->type == T_IDENT)
       {
           if (!parse_ident((AST::Ident**)factor)) return false;
           if (! (((currentSymbolTable()->registerReferencedVariable(((AST::Ident*)(*factor))->value))) ||
                ((currentSymbolTable()->registerReferencedConstant(((AST::Ident*)(*factor))->value)))) )
             throw UndefinedSymbolException((AST::Ident*)(*factor));
       }
   else if (token->type == T_NUMBER)
       { if (!parse_number((AST::Number**)factor)) return false; }
   else
       { if (!parse_parenthesis_factor((AST::ParenthesisFactor**)factor)) return false; }

   endParsing("factor");
   return true;
}


//ParenthesisFactor -> "(" Expression ")"
bool Parser::parse_parenthesis_factor(AST::ParenthesisFactor **parenthesisFactor)
{
   beginParsing("parenthesisFactor");

   *parenthesisFactor = new AST::ParenthesisFactor();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();

   nextToken(token);
   if (token->type != T_OPENPARENTHESIS) throw UnexpectedSymbolException(token);

   position startPosition = token->position;

   if (!parse_expression(&((*parenthesisFactor)->expression))) return false;

   nextToken(token);
   if (token->type != T_CLOSEPARENTHESIS) throw UnexpectedSymbolException(token);

   token->position.column += token->value.length() -1;
   (*parenthesisFactor)->setProperties(startPosition, token->position);

   endParsing("parenthesisFactor");
   return true;
}


//Condition -> OddCondition | CompareCondition
bool Parser::parse_condition(AST::Condition **condition)
{
   beginParsing("condition");

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   lookAHead(token);

   if (token->type == T_ODD)
       { if (!parse_odd_condition((AST::OddCondition**)condition)) return false; }
   else
       { if (!parse_compare_condition((AST::CompareCondition**)condition)) return false; }

   endParsing("condition");
   return true;
}


//OddCondition -> "ODD" Expression
bool Parser::parse_odd_condition(AST::OddCondition **oddCondition)
{
   beginParsing("oddCondition");

   *oddCondition = new AST::OddCondition();

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();

   nextToken(token);
   if (token->type != T_ODD) throw UnexpectedSymbolException(token);

   if (!parse_expression(&((*oddCondition)->expression))) return false;

   (*oddCondition)->setProperties(token->position, (*oddCondition)->expression->endPosition);

   endParsing("oddCondition");
   return true;
}


//CompareCondition -> Expression ("=" | "#" | "<" | "<=" | ">" | ">=") Expression
bool Parser::parse_compare_condition(AST::CompareCondition **compareCondition)
{
   beginParsing("compareCondition");

   AST::Expression *leftExpression;
   if (!parse_expression(&leftExpression)) return false;

   Token<enum Pl0TokenType> *token = new Token<enum Pl0TokenType>();
   nextToken(token);

   if (token->type == T_EQUAL)
       *compareCondition = new AST::EqualCondition();
   else if (token->type == T_GREATERTHEN)
       *compareCondition = new AST::GreaterCondition();
   else if (token->type == T_GREATEREQUAL)
       *compareCondition = new AST::GreaterEqualCondition();
   else if (token->type == T_LESSTHEN)
       *compareCondition = new AST::LessCondition();
   else if (token->type == T_LESSEQUAL)
       *compareCondition = new AST::LessEqualCondition();
   else if (token->type == T_NOTEQUAL)
       *compareCondition = new AST::NotEqualCondition();
   else
       throw UnexpectedSymbolException(token);

   AST::Expression *rightExpression;
   if (!parse_expression(&rightExpression)) return false;

   (*compareCondition)->leftExpression = leftExpression;
   (*compareCondition)->rightExpression = rightExpression;

   (*compareCondition)->setProperties(leftExpression->startPosition, rightExpression->endPosition);

   endParsing("compareCondition");
   return true;
}


void Parser::beginParsing(std::string ruleName)
{
    if (false)
    {
        for(int i=0; i<parse_depth; i++) std::cout << "   ";
        std::cout << "try to parse " << ruleName << " stacksize:" << symbolTableStack.size() << std::endl;
    }
    parse_depth++;
    if (parseWithoutPause) return;
    if (parse_depth <= 2) /* no file and no program */ return;

    m_sleeping = true;
    while (m_sleeping)
    {
       sleepMS(50); //50 microseconds
       if (m_stop) throw StopException();
    }
}


void Parser::endParsing(std::string ruleName)
{
    parse_depth--;
    if (false)
    {
       for(int i=0; i<parse_depth; i++) std::cout << "   ";
       std::cout << ruleName << " parsed" << " stacksize:" << symbolTableStack.size() << std::endl;
    }
}


bool Parser::nextToken(Token<enum Pl0TokenType> *token)
{
   *token = tokenList.front();
   tokenList.pop_front();
   controller->removeToken();
   return true;
}


bool Parser::lookAHead(Token<enum Pl0TokenType> *token)
{
   *token = tokenList.front();
   return false;
}
