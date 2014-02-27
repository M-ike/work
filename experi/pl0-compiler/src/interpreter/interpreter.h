#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <list>
#include <stack>
#include <pthread.h>

#include "../codegenerator/instruction.h"

#include "../gui/interpreterwindow.h"

class InterpreterWindow;

class Interpreter
{
 public:
     Interpreter(InterpreterWindow *window)
                : m_running(false), m_sleeping(false), m_stop(true),
                  window(window)
                {}
     ~Interpreter()
     {
         pthread_cancel(threadID);
         pthread_detach(threadID);
     }

     void setInstructions(std::list<instruction_t> instructions)
          { this->instructions = instructions; }

     void run()
     {
         if(!m_running) //start thread
         {
           pthread_create(&threadID, 0, &Interpreter::start_thread, this);
           m_running = true;
           m_stop = false;
         }
         else
         {
           m_sleeping = false;
         }
      }

      enum InterpreterReturn
      {
          FINISH,
          ABORTED,
      };

      bool isRunning()  { return m_running;  }
      bool isSleeping() { return m_sleeping; }

      void setInput(int input) { this->input = input; }

 private:
     volatile bool m_running;
     volatile bool m_sleeping;
     volatile bool m_stop;
     volatile int input;

     InterpreterWindow *window;

     void interprete();

     class Stackframe
     {
        public:
           typedef struct header
           {
               Stackframe *upper;
               int level;
               std::list<instruction_t>::iterator returnIP;
           } header_t;

           header_t header;

           Stackframe(Stackframe *upper, int level, std::list<instruction_t>::iterator returnIP)
                     { header.upper = upper; header.level = level; header.returnIP = returnIP; }

           void incrementMemory(int bytes);
           void setMemory(int level, int offset, int value);
           int getMemory(int level, int offset);

        private:
           std::list<int> memory;
     };

     pthread_t threadID;
     static void* start_thread(void *obj)
     {
        reinterpret_cast<Interpreter*>(obj)->interprete();
        reinterpret_cast<Interpreter*>(obj)->m_running = false;
        reinterpret_cast<Interpreter*>(obj)->m_sleeping = false;
        return 0;
     }

     std::list<instruction_t> instructions;
     std::list<instruction_t>::iterator instruction_pointer;
     std::stack<int> memStack;
     Stackframe *stackframe;

     void compute_LIT(instruction_t instruction);
     void compute_LOD(instruction_t instruction);
     void compute_STO(instruction_t instruction);
     void compute_CAL(instruction_t instruction);
     void compute_INT(instruction_t instruction);
     void compute_JMP(instruction_t instruction);
     void compute_JPC(instruction_t instruction);

     void compute_ADD(instruction_t instruction);
     void compute_SUB(instruction_t instruction);
     void compute_MULT(instruction_t instruction);
     void compute_DIV(instruction_t instruction);
     void compute_RET(instruction_t instruction);
     void compute_NOT(instruction_t instruction);
     void compute_ODD(instruction_t instruction);
     void compute_EQUAL(instruction_t instruction);
     void compute_NEQUAL(instruction_t instruction);
     void compute_GREATER(instruction_t instruction);
     void compute_GEQUAL(instruction_t instruction);
     void compute_LESS(instruction_t instruction);
     void compute_LEQUAL(instruction_t instruction);
     void compute_STDIN(instruction_t instruction);
     void compute_STDOUT(instruction_t instruction);

     void incrementInstructionPointer();
     void jumpTo(std::string label);

     int getInput();
     void output(int value);

     void compute(std::string what);
};


#endif
