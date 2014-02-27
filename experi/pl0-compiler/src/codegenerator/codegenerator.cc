#include "codegenerator.h"
#include <sstream>
#include <iostream>
#include <typeinfo>
#include <exception>

#include "../parser/symboltable.h"

std::list<instruction_t> CodeGenerator::generate()
{
   goIn("Program");
   std::list<instruction_t> instructions;
   if (program == NULL) return instructions;

   instructions = generate(true, program->block, program->symbolTable);

   goOut("Program");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate(bool isMainBlock, AST::Block *block, SymbolTable *symbolTable)
{
   goIn("Block");
   std::list<instruction_t> instructions;
   if (block == NULL) { goOut("Block"); return instructions; }

   // constants are treated via references

   std::list<instruction_t> variables = generate(block->variables);
   std::list<instruction_t> procedures = generate(block->procedures);
   std::list<instruction_t> statement = generate(block->statement, symbolTable);

   if ( isMainBlock)
   {
      instruction_t programEnd;
      programEnd.operation.opcode = OPR;
      programEnd.operation.identity = HALT;
      statement.push_back(programEnd);
   }
   else
   {
      instruction_t retInstruction;
      retInstruction.operation.opcode = OPR;
      retInstruction.operation.identity = RET;
      statement.push_back(retInstruction);
   }

   goOut("Block");
   return concat(concat(variables,statement),procedures);
}


std::list<instruction_t> CodeGenerator::generate(AST::Variables *variables)
{
   goIn("Variables");
   std::list<instruction_t> instructions;
   if (variables == NULL) { goOut("Variables"); return instructions; }

   int nr = variables->getNr();

   instruction_t variable;
   variable.operation.opcode = INTT;
   variable.operation.operand = intToString(nr);
   instructions.push_back(variable);

   goOut("Variables");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate(AST::Procedures *procedures)
{
   goIn("Procedures");
   std::list<instruction_t> instructions;
   if (procedures == NULL) { goOut("Procedures"); return instructions; }

   std::list<AST::ProcedureDeclaration*> allProcedures = procedures->getProcedures();
   std::list<AST::ProcedureDeclaration*>::iterator it;
   for( it = allProcedures.begin(); it != allProcedures.end(); it++ )
   {
       std::list<instruction_t> procedure = generate(*it);
       instructions = concat(instructions, procedure);
   }
   goOut("Procedures");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate(AST::ProcedureDeclaration *procedure)
{
   goIn("ProcedureDeclaration");
   std::string label;
   SymbolTable::symbol_t symbol;

   int nr = 0;
   if (procedure->formalParameters != NULL) nr = procedure->formalParameters->getNrParameters();

   procedure->symbolTable->getParentSymbolTable()->findProcedure(procedure->ident->value,nr,&symbol);
   label = getLabelName(symbol.savedAtSymbolTable) +procedure->ident->value + "(";
   if (procedure->formalParameters != NULL) label += intToString(procedure->formalParameters->getNrParameters());
   else label += "0";
   label += ")";

   std::list<instruction_t> block = generate(false, procedure->block, procedure->symbolTable);
   instruction_t firstInstruction = block.front();
   block.pop_front();
   firstInstruction.label = label;
   block.push_front(firstInstruction);

   goOut("ProcedureDeclaration");
   return block;
}


std::list<instruction_t> CodeGenerator::generate(AST::Statement *statement, SymbolTable *symbolTable)
{
   goIn("Statement");
   std::list<instruction_t> instructions;

   if (AST::AssignmentStatement* s = dynamic_cast<AST::AssignmentStatement*>(statement))
      { instructions = generate_assignment_statement(s, symbolTable); }
   else if (AST::CallStatement* s = dynamic_cast<AST::CallStatement*>(statement))
      { instructions = generate_call_statement(s, symbolTable); }
   else if (AST::CompoundStatement* s = dynamic_cast<AST::CompoundStatement*>(statement))
      { instructions = generate_compound_statement(s, symbolTable); }
   else if (AST::IfStatement* s = dynamic_cast<AST::IfStatement*>(statement))
      { instructions = generate_if_statement(s, symbolTable); }
   else if (AST::InputStatement* s = dynamic_cast<AST::InputStatement*>(statement))
      { instructions = generate_input_statement(s, symbolTable); }
   else if (AST::OutputStatement* s = dynamic_cast<AST::OutputStatement*>(statement))
      { instructions = generate_output_statement(s, symbolTable); }
   else if (AST::WhileStatement* s = dynamic_cast<AST::WhileStatement*>(statement))
      { instructions = generate_while_statement(s, symbolTable); }

   goOut("Statement");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate_assignment_statement(AST::AssignmentStatement *statement, SymbolTable *symbolTable)
{
   goIn("AssignmentStatement");
   std::list<instruction_t> instructions = generate(statement->expression, symbolTable);

   SymbolTable::symbol_t variable;
   symbolTable->findVariable(statement->ident->value, &variable);

   instruction_t store;
   store.operation.opcode = STO;
   store.operation.operand = intToString(variable.level) + "," + intToString(variable.offset);
   instructions.push_back(store);

   goOut("AssignmentStatement");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate_call_statement(AST::CallStatement *statement, SymbolTable *symbolTable)
{
   goIn("CallStatement");
   std::list<instruction_t> instructions;

   int nr = 0;
   if (statement->actualParameters != NULL) nr = statement->actualParameters->getNrParameters();

   if (nr > 0)
   {
       std::list<AST::Expression*> allParameters = statement->actualParameters->getParameterExpressions();
       std::list<AST::Expression*>::iterator it;
       for( it  = allParameters.begin(); it != allParameters.end(); it++ )
       {
           std::list<instruction_t> expression = generate(*it, symbolTable);
           instructions = concat(instructions, expression);
       }
   }

   instruction_t variable;
   SymbolTable::symbol_t symbol;

   variable.operation.opcode = LIT;
   variable.operation.operand = intToString(nr);
   instructions.push_back(variable);

   variable.operation.opcode = LIT;
   variable.operation.operand = intToString(symbolTable->getLevel() + 1);
   instructions.push_back(variable);

   variable.operation.opcode = CALL;
   symbolTable->findProcedure(statement->ident->value,nr,&symbol);
   variable.operation.operand = getLabelName(symbol.savedAtSymbolTable) + statement->ident->value + "(" +  intToString(nr) + ")";
   instructions.push_back(variable);

   goOut("CallStatement");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate_compound_statement(AST::CompoundStatement *statement, SymbolTable *symbolTable)
{
   goIn("CompoundStatement");
   std::list<instruction_t> instructions;

   std::list<AST::Statement*> allStatement = statement->getStatements();

   std::list<AST::Statement*>::iterator it;
   for( it  = allStatement.begin();
        it != allStatement.end(); it++ )
   {
       std::list<instruction_t> statementInstructions = generate(*it, symbolTable);
       instructions = concat(instructions, statementInstructions);
   }

   goOut("CompoundStatement");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate_if_statement(AST::IfStatement *statement, SymbolTable *symbolTable)
{
   goIn("IfStatement");
   std::list<instruction_t> instructions;

   instructions = generate(statement->condition, symbolTable);
   int endIfPathLabel = nextLabel();

   instruction_t instr;

   instr.operation.opcode = JPC;
   instr.operation.operand = intToString(endIfPathLabel);
   instructions.push_back(instr);

   instr.operation.opcode = OPR;
   instr.operation.identity = NOP;
   instr.label = intToString(endIfPathLabel);
   instructions.push_back(instr);

   goOut("IfStatement");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate_input_statement(AST::InputStatement *statement, SymbolTable *symbolTable)
{
   goIn("InputStatement");
   std::list<instruction_t> instructions;

   instruction_t instr;

   instr.operation.opcode = OPR;
   instr.operation.identity = STDIN;
   instructions.push_back(instr);

   SymbolTable::symbol_t variable;
   symbolTable->findVariable(statement->ident->value, &variable);

   instr.operation.opcode = STO;
   instr.operation.operand = intToString(variable.level) + "," + intToString(variable.offset);
   instructions.push_back(instr);

   goOut("InputStatement");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate_output_statement(AST::OutputStatement *statement, SymbolTable *symbolTable)
{
   goIn("OutputStatement");
   std::list<instruction_t> instructions = generate(statement->expression, symbolTable);

   instruction_t instr;
   instr.operation.opcode = OPR;
   instr.operation.identity = STDOUT;
   instructions.push_back(instr);

   goOut("OutputStatement");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate_while_statement(AST::WhileStatement *statement, SymbolTable *symbolTable)
{
   goIn("WhileStatement");
   std::list<instruction_t> instructions;

   instructions = generate(statement->condition, symbolTable);

   int beginWhileLabel = nextLabel();

   instruction_t instr;
   instr = instructions.front();
   instructions.pop_front();
   instr.label = intToString(beginWhileLabel);
   instructions.push_front(instr);

   instr.label="";

   int endWhileLabel = nextLabel();

   instr.operation.opcode = OPR;
   instr.operation.identity = NOT;
   instructions.push_back(instr);

   instr.operation.opcode = JPC;
   instr.operation.operand = intToString(endWhileLabel);
   instructions.push_back(instr);

   std::list<instruction_t> statementInstructions = generate(statement->statement, symbolTable);
   instructions = concat(instructions, statementInstructions);

   instr.operation.opcode = JMP;
   instr.operation.operand = intToString(beginWhileLabel);
   instructions.push_back(instr);

   instr.operation.opcode = OPR;
   instr.operation.identity = NOP;
   instr.label = intToString(endWhileLabel);
   instructions.push_back(instr);

   goOut("WhileStatement");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate(AST::Expression *expression, SymbolTable *symbolTable)
{
   goIn("Expression");
   std::list<instruction_t> instructions;

   instructions = generate_FirstTerm(expression->firstTerm, symbolTable);

   if (AST::PlusExpression* s = dynamic_cast<AST::PlusExpression*>(expression))
   {
      instructions =concat(instructions, generate_AdditionalExpression(s->additionalExpression, symbolTable));
      instruction_t instr;
      instr.operation.opcode = OPR;
      instr.operation.identity = ADD;
      instructions.push_back(instr);
   }
   else if (AST::MinusExpression* s = dynamic_cast<AST::MinusExpression*>(expression))
   {
      instructions = concat(instructions, generate_AdditionalExpression(s->additionalExpression, symbolTable));
      instruction_t instr;
      instr.operation.opcode = OPR;
      instr.operation.identity = SUB;
      instructions.push_back(instr);
   }
   else if (dynamic_cast<AST::FirstTermExpression*>(expression))
   {
   }

   goOut("Expression");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate_AdditionalExpression(AST::AdditionalExpression *additionalExpression, SymbolTable *symbolTable)
{
   goIn("AdditionalExpression");
   std::list<instruction_t> instructions;

   instructions = generate_Term(additionalExpression->term, symbolTable);

   if (AST::PlusAdditionalExpression* s = dynamic_cast<AST::PlusAdditionalExpression*>(additionalExpression))
   {
      instructions =concat(instructions, generate_AdditionalExpression(s->additionalExpression, symbolTable));
      instruction_t instr;
      instr.operation.opcode = OPR;
      instr.operation.identity = ADD;
      instructions.push_back(instr);
   }
   else if (AST::MinusAdditionalExpression* s = dynamic_cast<AST::MinusAdditionalExpression*>(additionalExpression))
   {
      instructions = concat(instructions, generate_AdditionalExpression(s->additionalExpression, symbolTable));
      instruction_t instr;
      instr.operation.opcode = OPR;
      instr.operation.identity = SUB;
      instructions.push_back(instr);
   }
   else if (dynamic_cast<AST::TermAdditionalExpression*>(additionalExpression))
   {
   }

   goOut("AdditionalExpression");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate_Term(AST::Term *term, SymbolTable *symbolTable)
{
   goIn("Term");
   std::list<instruction_t> instructions;

   instructions = generate_Factor(term->factor, symbolTable);

   if (AST::MultTerm* s = dynamic_cast<AST::MultTerm*>(term))
   {
      instructions =concat(instructions, generate_Term(s->term, symbolTable));
      instruction_t instr;
      instr.operation.opcode = OPR;
      instr.operation.identity = MULT;
      instructions.push_back(instr);
   }
   else if (AST::DivTerm* s = dynamic_cast<AST::DivTerm*>(term))
   {
      instructions = concat(instructions, generate_Term(s->term, symbolTable));
      instruction_t instr;
      instr.operation.opcode = OPR;
      instr.operation.identity = DIV;
      instructions.push_back(instr);
   }
   else if (dynamic_cast<AST::FactorTerm*>(term))
   {
   }

   goOut("Term");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate_FirstTerm(AST::FirstTerm *firstTerm, SymbolTable *symbolTable)
{
   goIn("FirstTerm");
   std::list<instruction_t> instructions;

   instructions = generate_Term(firstTerm->term, symbolTable);

   if (dynamic_cast<AST::PlusFirstTerm*>(firstTerm))
   {

   }
   else if (dynamic_cast<AST::MinusFirstTerm*>(firstTerm))
   {
      instruction_t instr;
      instr.operation.opcode = LIT;
      instr.operation.operand = "-1";
      instructions.push_back(instr);
      instr.operation.opcode = OPR;
      instr.operation.identity = MULT;
      instructions.push_back(instr);
   }
   else if (dynamic_cast<AST::SignlessFirstTerm*>(firstTerm))
   {
   }

   goOut("FirstTerm");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate_Factor(AST::Factor *factor, SymbolTable *symbolTable)
{
   goIn("Factor");
   std::list<instruction_t> instructions;

   if (AST::Ident* s = dynamic_cast<AST::Ident*>(factor))
   {
      SymbolTable::symbol_t symbol;
      if (symbolTable->findConstant(s->value, &symbol))
      {
         instruction_t instr;
         instr.operation.opcode = LIT;
         instr.operation.operand = intToString(((AST::ConstantDeclaration*) (symbol.reference))->number->value);
         instructions.push_back(instr);
      }
      else if (symbolTable->findVariable(s->value, &symbol))
      {
         instruction_t instr;
         instr.operation.opcode = LOD;
         instr.operation.operand = intToString(symbol.level)+","+intToString(symbol.offset);
         instructions.push_back(instr);
      }
   }
   else if (AST::Number* s = dynamic_cast<AST::Number*>(factor))
   {
        instruction_t instr;
        instr.operation.opcode = LIT;
        instr.operation.operand = intToString(s->value);
        instructions.push_back(instr);
   }
   else if (AST::ParenthesisFactor* s =dynamic_cast<AST::ParenthesisFactor*>(factor))
   {
         instructions = generate(s->expression, symbolTable);
   }

   goOut("Factor");
   return instructions;
}


std::list<instruction_t> CodeGenerator::generate(AST::Condition *condition, SymbolTable *symbolTable)
{
   goIn("Condition");
   std::list<instruction_t> instructions;


   if (AST::OddCondition* s = dynamic_cast<AST::OddCondition*>(condition))
      { instructions = generate_oddCondition(s, symbolTable); }
   else if (AST::CompareCondition* s = dynamic_cast<AST::CompareCondition*>(condition))
      { instructions = generate_compareCondition(s, symbolTable); }

   goOut("Condition");
   return instructions;
}

std::list<instruction_t> CodeGenerator::generate_oddCondition(AST::OddCondition *condition, SymbolTable *symbolTable)
{
   goIn("OddCondition");
   std::list<instruction_t> instructions;

   instructions = generate(condition->expression,symbolTable);

   instruction_t instr;
   instr.operation.opcode = OPR;
   instr.operation.identity = ODD;
   instructions.push_back(instr);

   goOut("OddCondition");
   return instructions;
}

std::list<instruction_t> CodeGenerator::generate_compareCondition(AST::CompareCondition *condition, SymbolTable *symbolTable)
{
   goIn("CompareCondition");
   std::list<instruction_t> instructions;

   instructions = generate(condition->leftExpression,symbolTable);
   instructions = concat(instructions, generate(condition->rightExpression,symbolTable));

   instruction_t instr;

   if (dynamic_cast<AST::EqualCondition*>(condition))
      {
          instr.operation.opcode = OPR;
          instr.operation.identity = EQUAL;
          instructions.push_back(instr);
      }
   else if (dynamic_cast<AST::NotEqualCondition*>(condition))
      {
           instr.operation.opcode = OPR;
          instr.operation.identity = NEQUAL;
          instructions.push_back(instr);
      }
   else if (dynamic_cast<AST::GreaterCondition*>(condition))
      {
          instr.operation.opcode = OPR;
          instr.operation.identity = GREATER;
          instructions.push_back(instr);
      }
   else if (dynamic_cast<AST::GreaterEqualCondition*>(condition))
      {
          instr.operation.opcode = OPR;
          instr.operation.identity = GEQUAL;
          instructions.push_back(instr);
      }
   else if (dynamic_cast<AST::LessCondition*>(condition))
      {
          instr.operation.opcode = OPR;
          instr.operation.identity = LESS;
          instructions.push_back(instr);
      }
   else if (dynamic_cast<AST::LessEqualCondition*>(condition))
      {
          instr.operation.opcode = OPR;
          instr.operation.identity = LEQUAL;
          instructions.push_back(instr);
      }

   goOut("CompareCondition");
   return instructions;
}


std::list<instruction_t> CodeGenerator::concat(std::list<instruction_t> list_a, std::list<instruction_t> list_b)
{
   if (list_b.empty()) return list_a;
   if (list_a.empty()) return list_b;

   std::list<instruction_t>::iterator it;
   for( it = list_b.begin(); it != list_b.end(); it++ )
       list_a.push_back(*it);

   return list_a;
}


std::string CodeGenerator::intToString(int i)
{

   std::stringstream out;
   out << i;
   return out.str();
}

void CodeGenerator::goIn(std::string tree)
{
    if (false)
    {
        for(int i=0; i<gen_depth; i++) std::cout << "   ";
        std::cout << tree << " (in)" << std::endl;
    }
    gen_depth++;
}

void CodeGenerator::goOut(std::string tree)
{
    gen_depth--;
    if (false)
    {
       for(int i=0; i<gen_depth; i++) std::cout << "   ";
       std::cout << tree << " (out)" << std::endl;
    }
}

std::string CodeGenerator::getLabelName(SymbolTable *s)
{
    std::string ret="";
    while (s!=NULL)
    {
       ret = s->name() + "::"+ ret;
       s = s->getParentSymbolTable();
    }
    return ret;
}

