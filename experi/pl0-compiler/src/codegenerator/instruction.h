#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>

enum opcode
{
   LIT, LOD, STO, CALL, INTT, JMP, JPC, OPR
};

enum operationIdentity
{
   ADD, RET, HALT, NOP, STDIN, STDOUT, ODD, SUB, EQUAL, NEQUAL, GREATER, GEQUAL, LESS, LEQUAL,
   MULT, DIV, NOT
};


typedef struct operation
{
      enum opcode opcode;
      enum operationIdentity identity;
      std::string operand;
} operation_t;


typedef struct instruction
{
      std::string label;
      operation_t operation;
} instruction_t;


#endif
