#ifndef PL0CONTROLLER_H
#define PL0CONTROLLER_H

#include <list>

#include "lexer/pl0lexer.h"
#include "gui/mainwindow.h"

#include "parser/parser.h"
#include "common.h"

#include "codegenerator/instruction.h"
#include "codegenerator/codegenerator.h"

class MainWindow;
class Parser;
class CodeGenerator;

class PL0Controller
{
   public:
     PL0Controller(): pl0Lexer(NULL), parser(NULL), codeGenerator(NULL) {}
     void oneTokenForward();
     void allTokenForward();
     void completeParsing();
     void parsingTillPause();
     void generateCodeComplete();
     void setMainWindow(MainWindow *mainWindow);
     void resetLexer();
     void showParseTree();
     void parse(bool with_pause);
     void afterParse();
     void abortParsing();
     void removeToken();

     void clearCodeGenerator();

   private:
     bool oneTokenForward(bool highlight);
     PL0Lexer *pl0Lexer;
     Parser *parser;
     CodeGenerator *codeGenerator;
     MainWindow *mainWindow;
};

#endif
