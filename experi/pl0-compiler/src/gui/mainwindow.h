 #ifndef MAINWINDOW_H
 #define MAINWINDOW_H

 #include <QMainWindow>
 #include <QTranslator>
 #include <QString>
 #include <QTableWidget>

 #include "ilanguagewidget.h"
 #include "languagemanager.h"
 #include "languageaction.h"

 #include "sourcewindow.h"
 #include "tokentablewindow.h"
 #include "parsetreewindow.h"
 #include "symboltablewindow.h"
 #include "grammarwindow.h"
 #include "codewindow.h"
 #include "interpreterwindow.h"
 #include "lexertoolbar.h"
 #include "parsertoolbar.h"
 #include "codetoolbar.h"
 #include "interpretertoolbar.h"

 #include "../pl0controller.h"

 #include "../common.h"

 #include "../parser/exceptions.h"

 #include "../codegenerator/instruction.h"

 #include <iostream>

 class PL0Controller;

 class QAction;
 class QMenu;
 class SourceWindow;
 class TokenTableWindow;
 class ParseTreeWindow;
 class SymbolTableWindow;
 class GrammarWindow;
 class InterpreterWindow;
 class CodeWindow;

 class QHeaderView;

 class LexerToolBar;
 class ParserToolBar;
 class CodeToolBar;
 class InterpreterToolBar;


 enum GuiState
 {
    LEXING_NEW,
    LEXING_START,
    LEXING_STOP,
    LEXING_ABORT,
    PARSING_START,
    PARSING_STOP,
    PARSING_FINISH,
    PARSING_ABORT
 };


 class MainWindow : public QMainWindow, ILanguageWidget
 {
     Q_OBJECT

 public:
     MainWindow(PL0Controller *pl0controller);

     void languageNotify();

     void initWindow();

     std::string getSourceCode();
     void changeGUI(enum GuiState state);

     void addToken(Token<Pl0TokenType> *token);
     void removeFirstToken();

     bool highlightSource(position_t position, unsigned int length);
     bool markSource(position_t start, position_t end);

     void goLexerStepForward();
     void goLexerStepEnd();
     void goParserStepForward();
     void goParserStepEnd();
     void goCodeGenerationStepEnd();

     void showParseTree(AST::ASTTree *astTree);
     void showException(ParseException *parseException);
     void showAssemblerCode(std::list<instruction_t> instructions);

     void grammarWindowWasSetVisible(bool visible);
     void finish_CodeGeneration();
     void showInterpreterWindow();


 private slots:
     void save();
     void saveAs();
     void about();
     void open();
     void showGrammar();

     void changeLanguage(QAction * action);

 private:
     void changeLanguage(std::string languageFile);

     void initLanguages();

     void createActions();
     void createMenus();
     void createToolBars();
     void createStatusBar();

     void initDockWindows();
     void initGrammarWindow();

     void stopLexing();
     void newLexing();
     void startLexing();

     void canStartParsing();
     void startParsing();
     void stopParsing();
     void finishParsing();
     void abortParsing();

     bool oneStepForward();


     void abortCodeGeneration();

     PL0Controller *pl0controller;
     QTranslator *translator;

     SourceWindow *sourceWindow;
     TokenTableWindow *tokenTableWindow;
     ParseTreeWindow *parseTreeWindow;
     SymbolTableWindow *symbolTableWindow;
     GrammarWindow *grammarWindow;
     CodeWindow *codeWindow;
     InterpreterWindow *interpreterWindow;

     QMenu *fileMenu;
     QMenu *viewMenu;
     QMenu *languageMenu;
     QMenu *helpMenu;

     QToolBar *fileToolBar;
     LexerToolBar *lexerToolBar;
     ParserToolBar *parserToolBar;
     CodeToolBar *codeToolBar;
     InterpreterToolBar *interpreterToolBar;

     /* filename of the source file */
     QString filename;

     QAction *openAction;
     QAction *saveAction;
     QAction *saveAsAction;
     QAction *quitAction;

     QAction *aboutAction;
     QAction *aboutQtAction;
     QAction *grammarAction;

 };

 #endif
