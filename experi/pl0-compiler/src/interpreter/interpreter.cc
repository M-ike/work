#include <iostream>
#include <sstream>

#include "interpreter.h"
#include "../common.h"

int stringToInt(std::string str);
int get_level(std::string adr);
int get_offset(std::string adr);
std::string intToString(int i);

void Interpreter::interprete()
{
   instruction_pointer = instructions.begin();

   stackframe = new Stackframe(NULL, 0, instruction_pointer);

   while (true)
   {
      instruction_t instruction = *instruction_pointer;
      incrementInstructionPointer();

      switch (instruction.operation.opcode)
      {
          case LIT:   { compute("LIT"); compute_LIT(instruction); break; }
          case LOD:   { compute("LOD"); compute_LOD(instruction); break; }
          case STO:   { compute("STO"); compute_STO(instruction); break; }
          case CALL:   { compute("CAL"); compute_CAL(instruction); break; }
          case INTT:   { compute("INT"); compute_INT(instruction); break; }
          case JMP:   { compute("JMP"); compute_JMP(instruction); break; }
          case JPC:   { compute("JPC"); compute_JPC(instruction); break; }
          case OPR:
          {
              switch (instruction.operation.identity)
              {
                 case ADD:     { compute("ADD"); compute_ADD(instruction); break; }
                 case SUB:     { compute("SUB"); compute_SUB(instruction); break; }
                 case MULT:    { compute("MULT"); compute_MULT(instruction); break; }
                 case DIV:     { compute("DIV"); compute_DIV(instruction); break; }
                 case RET:     { compute("RET"); compute_RET(instruction); break; }
                 case NOP:     { compute("NOP"); break; }
                 case NOT:     { compute("NOT"); compute_NOT(instruction); break; }
                 case ODD:     { compute("ODD"); compute_ODD(instruction); break; }
                 case EQUAL:   { compute("EQUAL"); compute_EQUAL(instruction); break; }
                 case NEQUAL:  { compute("NEQUAL"); compute_NEQUAL(instruction); break; }
                 case GREATER: { compute("GREATER"); compute_GREATER(instruction); break; }
                 case GEQUAL:  { compute("GEQUAL"); compute_GEQUAL(instruction); break; }
                 case LESS:    { compute("LESS"); compute_LESS(instruction); break; }
                 case LEQUAL:  { compute("LEQUAL"); compute_LEQUAL(instruction); break; }
                 case STDIN:   { compute("STDIN"); compute_STDIN(instruction); break; }
                 case STDOUT:  { compute("STDOUT"); compute_STDOUT(instruction); break; }
                 case HALT:    {
                                 compute("HALT");
                                 delete stackframe;
                                 window->finishedInterpretaton();
                                 return;
                                 break;
                               }
                 default:      { std::cout << "Unknown OPR" << std::endl; break; }
              }
              break;
          }
          default: { std::cout << "Unknown OPCODE" << std::endl; break; }
      }
   }

}


void Interpreter::incrementInstructionPointer()
{
   if (instruction_pointer == instructions.end()) return;
   instruction_pointer++;
}


void Interpreter::compute_LIT(instruction_t instruction)
{
   memStack.push(stringToInt(instruction.operation.operand));
}
void Interpreter::compute_LOD(instruction_t instruction)
{
   //std::cout << "LOD: " << instruction.operation.operand << std::endl;
   int level = get_level(instruction.operation.operand);
   int offset = get_offset(instruction.operation.operand);
   int value = stackframe->getMemory(level, offset);
   memStack.push(value);
}
void Interpreter::compute_STO(instruction_t instruction)
{
   //std::cout << "STO: " << instruction.operation.operand << std::endl;
   int level = get_level(instruction.operation.operand);
   int offset = get_offset(instruction.operation.operand);
   int value = memStack.top(); memStack.pop();
   stackframe->setMemory(level, offset, value);
}
void Interpreter::compute_CAL(instruction_t instruction)
{
   //std::cout << "call: " << instruction.operation.operand;
   int level = memStack.top(); memStack.pop(); std::cout << " level: " << level;
   int variables = memStack.top(); memStack.pop(); std::cout << " variables: " << variables << std::endl;
   Stackframe *newStackframe = new Stackframe(stackframe, level, instruction_pointer);
   stackframe = newStackframe;
   stackframe->incrementMemory(variables);
   for (int i=variables-1; i>=0; i--)
   {
       //std::cout << "call sets 1 value to: " << level << "," << i << std::endl;
       int value = memStack.top(); memStack.pop();
       stackframe->setMemory(stackframe->header.level, i, value);
   }
   jumpTo(instruction.operation.operand);
}
void Interpreter::compute_INT(instruction_t instruction)
{
   stackframe->incrementMemory(stringToInt(instruction.operation.operand));
}
void Interpreter::compute_JMP(instruction_t instruction)
{
   jumpTo(instruction.operation.operand);
}
void Interpreter::compute_JPC(instruction_t instruction)
{
   int con = memStack.top(); memStack.pop();
   if (con != 0) jumpTo(instruction.operation.operand);
}
void Interpreter::compute_ADD(instruction_t instruction)
{
   int a0 = memStack.top(); memStack.pop();
   int a1 = memStack.top(); memStack.pop();
   memStack.push(a0+a1);
}
void Interpreter::compute_SUB(instruction_t instruction)
{
   int a0 = memStack.top(); memStack.pop();
   int a1 = memStack.top(); memStack.pop();
   memStack.push(a1-a0);
}
void Interpreter::compute_MULT(instruction_t instruction)
{
   int a0 = memStack.top(); memStack.pop();
   int a1 = memStack.top(); memStack.pop();
   memStack.push(a0*a1);
}
void Interpreter::compute_DIV(instruction_t instruction)
{
   int a0 = memStack.top(); memStack.pop();
   int a1 = memStack.top(); memStack.pop();
   memStack.push(a0/a1);
}
void Interpreter::compute_RET(instruction_t instruction)
{
   Stackframe *oldStackframe = stackframe->header.upper;
   instruction_pointer = stackframe->header.returnIP;
   delete stackframe;
   stackframe = oldStackframe;
   //std::cout << "returned" << std::endl;
}
void Interpreter::compute_NOT(instruction_t instruction)
{
   int a = memStack.top(); memStack.pop();
   if (a == 0) memStack.push(1);
   else memStack.push(0);
}
void Interpreter::compute_ODD(instruction_t instruction)
{
   int a = memStack.top(); memStack.pop();
   if (a % 2 == 1) memStack.push(1);
   else memStack.push(0);
}
void Interpreter::compute_EQUAL(instruction_t instruction)
{
   int a0 = memStack.top(); memStack.pop();
   int a1 = memStack.top(); memStack.pop();
   if (a0 == a1) memStack.push(1);
   else memStack.push(0);
}
void Interpreter::compute_NEQUAL(instruction_t instruction)
{
   int a0 = memStack.top(); memStack.pop();
   int a1 = memStack.top(); memStack.pop();
   if (a0 != a1) memStack.push(1);
   else memStack.push(0);
}
void Interpreter::compute_GREATER(instruction_t instruction)
{
   int a0 = memStack.top(); memStack.pop();
   int a1 = memStack.top(); memStack.pop();
   if (a1 > a0) memStack.push(1);
   else memStack.push(0);
}
void Interpreter::compute_GEQUAL(instruction_t instruction)
{
   int a0 = memStack.top(); memStack.pop();
   int a1 = memStack.top(); memStack.pop();
   if (a1 >= a0) memStack.push(1);
   else memStack.push(0);
}
void Interpreter::compute_LESS(instruction_t instruction)
{
   int a0 = memStack.top(); memStack.pop();
   int a1 = memStack.top(); memStack.pop();
   if (a1 < a0) memStack.push(1);
   else memStack.push(0);
}
void Interpreter::compute_LEQUAL(instruction_t instruction)
{
   int a0 = memStack.top(); memStack.pop();
   int a1 = memStack.top(); memStack.pop();
   if (a1 <= a0) memStack.push(1);
   else memStack.push(0);
}
void Interpreter::compute_STDIN(instruction_t instruction)
{
   memStack.push(getInput());
}
void Interpreter::compute_STDOUT(instruction_t instruction)
{
   output(memStack.top());
   memStack.pop();
}


void Interpreter::jumpTo(std::string label)
{
   std::list<instruction_t>::iterator ip;
   for(ip = instructions.begin(); ip != instructions.end(); ip++ )
   {
      if (label.compare(ip->label) == 0)
         { instruction_pointer = ip; return; }
   }
}

int Interpreter::getInput()
{
    window->waitForInput();

    m_sleeping = true;
    while (m_sleeping)
    {
       sleepMS(50); //50 microseconds
    }
    return input;
}

void Interpreter::output(int value)
{
   window->println(intToString(value));
}



void Interpreter::Stackframe::incrementMemory(int bytes)
{
   for (int i=0; i<bytes; i++)
      memory.push_back(rand());
}

void Interpreter::Stackframe::setMemory(int level, int offset, int value)
{
   //std::cout << "level:" << level << " offset:" << offset << " value:" << value << " own_level:" << header.level << " memSize:" << memory.size() << std::endl;

   if (level > header.level)
      { std::cout << "TODO: level to high while setMemory" << std::endl;    sleepMS(1000);    return; }

   if (level < header.level)
      { header.upper->setMemory(level, offset, value); return; }

   // we have to set Memory
   if ( (offset < 0) || (memory.size() <= (unsigned int)offset) )  //TODO
      { std::cout << "TODO: wrong offset while setMemory" << std::endl;    sleepMS(1000);     return; }

   std::list<int>::iterator it;
   int i=0;
   for( it = memory.begin(); it != memory.end(); it++ )
   {
       if (i==offset) { *it = value; break; }
       i++;
   }
}

int Interpreter::Stackframe::getMemory(int level, int offset)
{
   //std::cout << "level:" << level << " offset:" << offset << " own level:" << header.level << " memSize:" << memory.size() << std::endl;

   if (level > header.level)
      { std::cout << "TODO: level to high while getMemory" << std::endl;    sleepMS(1000);    return rand(); }

   if (level < header.level)
      { return header.upper->getMemory(level, offset); }

   //we have to load memory
   if ( (offset < 0) || (memory.size() <= (unsigned int)offset) )  //TODO
      { std::cout << "TODO: wrong offset while getMemory" << std::endl;    sleepMS(1000);    return rand(); }

   std::list<int>::iterator it;
   int i=0;
   for( it = memory.begin(); it != memory.end(); it++ )
   {
       if (i==offset) return *it;
       i++;
   }

   std::cout << "TODO: state while getMemory" << std::endl;
   return rand();
}

void Interpreter::compute(std::string what)
{
   return;
   std::cout << what << std::endl;
}

int stringToInt(std::string str)
{
    std::istringstream buffer(str);
    int ret;
    buffer >> ret;
    return ret;
}

int get_level(std::string adr)
{
    return stringToInt(adr.substr(0,adr.find(",")));
}

int get_offset(std::string adr)
{
    return stringToInt(adr.substr(adr.find(",")+1));
}

std::string intToString(int i)
{
   std::stringstream out;
   out << i;
   return out.str();
}
