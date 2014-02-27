#include "pl0controller.h"
#include <iostream>
#include <pthread.h>
#include <list>

#include "codegenerator/instruction.h"

void PL0Controller::setMainWindow(MainWindow *mainWindow)
{
  this->mainWindow = mainWindow;
}

void PL0Controller::allTokenForward()
{
  while (oneTokenForward(false))
     ;
}

void PL0Controller::oneTokenForward()
{
   oneTokenForward(true);
}

bool PL0Controller::oneTokenForward(bool highlight)
{
  if (pl0Lexer == NULL)
  {
     /* new lexer */
     pl0Lexer = new PL0Lexer();
     pl0Lexer->setInput(mainWindow->getSourceCode());
     pl0Lexer->lex();

     mainWindow->changeGUI(LEXING_START);
  }
  Token<Pl0TokenType> token;
  if (pl0Lexer->nextToken(&token))
  {
     mainWindow->addToken(&token);
     if (highlight) mainWindow->highlightSource(token.position, token.value.length());
  }
  if (pl0Lexer->existsNextToken()) //if last token -> disable all buttons
  {
     return true;
  }
  else
  {
     mainWindow->changeGUI(LEXING_STOP);
     return false;
  }
  return false;
}

void PL0Controller::resetLexer()
{ pl0Lexer = NULL; }


void PL0Controller::parse(bool with_pause)
{
   if (parser == NULL)
   {
      parser = new Parser(this);
      std::list<Token<enum Pl0TokenType> > tokenList = pl0Lexer->getTokenList();
      parser->setTokenList(tokenList);
   }
   parser->pausingAfterEachRule(with_pause);
   parser->run();

   mainWindow->changeGUI(PARSING_START);
}

void PL0Controller::parsingTillPause()
{
   parse(true);
   while (parser->isRunning() && !parser->isSleeping())  { sleepMS(50); /* 50 microseconds */ }
   afterParse();
}

void PL0Controller::afterParse()
{
   AST::Program *programTree = parser->getProgramTree();
   if (programTree != NULL) mainWindow->showParseTree(programTree);
   if (parser->returnCode() == Parser::EXCEPTION)
      mainWindow->showException(parser->getCatchedException());
   if (parser->returnCode() == Parser::FINISH)
   {
      mainWindow->changeGUI(PARSING_FINISH);
   }
}

void PL0Controller::completeParsing()
{
   parse(false);
   while (parser->isRunning()) { sleepMS(50);  /* 50 microseconds */ }
   afterParse();
}

void PL0Controller::generateCodeComplete()
{
   if (codeGenerator == NULL)
   {
      codeGenerator = new CodeGenerator();
      AST::Program *program = parser->getProgramTree();
      codeGenerator->setProgram(program);
   }
   std::list<instruction_t> instructions = codeGenerator->generate();
   mainWindow->showAssemblerCode(instructions);
   mainWindow->finish_CodeGeneration();
}

void PL0Controller::abortParsing()
{
   if (parser == NULL) return;
   parser->abortParser();
   while (parser->isRunning()) { sleepMS(50);  /* 50 microseconds */ }
   delete parser;
   parser = NULL;
}


void PL0Controller::clearCodeGenerator()
{
     codeGenerator = NULL;
}


void PL0Controller::removeToken()
{ mainWindow->removeFirstToken(); }
