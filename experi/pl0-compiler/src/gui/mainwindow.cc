#include <QtGui>
#include <QIcon>
#include <QFileDialog>

#include <iostream>

#include "mainwindow.h"

#include "../lexer/token.h"
#include "../lexer/pl0token.h"


MainWindow::MainWindow(PL0Controller *pl0controller)
   : pl0controller(pl0controller), translator(NULL)
{}

void MainWindow::languageNotify()
{
    setWindowTitle(tr("PL/0"));
    languageMenu->setTitle(tr("Language"));
    fileMenu->setTitle(tr("&File"));
    viewMenu->setTitle(tr("&View"));
    helpMenu->setTitle(tr("&Help"));

    fileToolBar->setWindowTitle(tr("File Toolbar"));

    openAction->setText(tr("&Open"));
    openAction->setShortcut(tr("Ctrl+O"));
    openAction->setStatusTip(tr("Open a PL/0 source file"));

    saveAction->setText(tr("&Save..."));
    saveAction->setShortcut(tr("Ctrl+S"));
    saveAction->setStatusTip(tr("Save the current code"));

    saveAsAction->setText(tr("Save as..."));
    saveAsAction->setStatusTip(tr("Save the current code to another file"));

    quitAction->setText(tr("&Quit"));
    quitAction->setShortcut(tr("Ctrl+Q"));
    quitAction->setStatusTip(tr("Quit the application"));

    aboutAction->setText(tr("&About"));
    aboutAction->setStatusTip(tr("Show the application's About box"));

    aboutQtAction->setText(tr("About &Qt"));
    aboutQtAction->setStatusTip(tr("Show the Qt library's About box"));

    grammarAction->setText(tr("show PL/O 2.0 Grammar"));
    grammarAction->setStatusTip(tr("show the PL/O 2.0 Grammar"));
}

void MainWindow::initWindow()
{
     initLanguages();
     changeLanguage("de.qm");

     LanguageManager::getInstance()->registerWidget(this);


     createActions();
     createMenus();
     createToolBars();

     createStatusBar();

     initDockWindows();
     initGrammarWindow();

     interpreterWindow = new InterpreterWindow(this);

     setWindowIcon(QIcon(QString::fromUtf8(":/icon/images/donkey-48x48.png")));

     languageNotify();
}


void MainWindow::initDockWindows()
{
     setDockNestingEnabled(true);

     sourceWindow = new SourceWindow(this);
     sourceWindow->setAllowedAreas(Qt::AllDockWidgetAreas);
     addDockWidget(Qt::LeftDockWidgetArea, sourceWindow);
     viewMenu->addAction(sourceWindow->toggleViewAction());

     tokenTableWindow = new TokenTableWindow(this);
     tokenTableWindow->setAllowedAreas(Qt::AllDockWidgetAreas);
     addDockWidget(Qt::LeftDockWidgetArea, tokenTableWindow);
     viewMenu->addAction(tokenTableWindow->toggleViewAction());

     symbolTableWindow = new SymbolTableWindow(this);
     symbolTableWindow->setAllowedAreas(Qt::AllDockWidgetAreas);
     addDockWidget(Qt::RightDockWidgetArea, symbolTableWindow);
     viewMenu->addAction(symbolTableWindow->toggleViewAction());

     parseTreeWindow = new ParseTreeWindow(this, symbolTableWindow);
     parseTreeWindow->setAllowedAreas(Qt::AllDockWidgetAreas);
     addDockWidget(Qt::RightDockWidgetArea, parseTreeWindow);
     viewMenu->addAction(parseTreeWindow->toggleViewAction());

     codeWindow = new CodeWindow(this);
     codeWindow->setAllowedAreas(Qt::AllDockWidgetAreas);
     addDockWidget(Qt::RightDockWidgetArea, codeWindow);
     viewMenu->addAction(codeWindow->toggleViewAction());
}

void MainWindow::showParseTree(AST::ASTTree *astTree)
{
   parseTreeWindow->showTree(astTree);
}


void MainWindow::showException(ParseException *parseException)
{
   QString msg;
   QString space = QString(" ");
   QString at = space + tr("at (row;column):") + space;

   unsigned int line;

   if (UndefinedVariableException* uve = dynamic_cast<UndefinedVariableException*>(parseException))
   {
      msg  = tr("undefined variable") + space;
      msg += QString("\"") + QString::fromStdString(uve->ident->value )+ QString("\"");
      msg += at + QString::number(uve->ident->startPosition.row) + QString(";") + QString::number(uve->ident->startPosition.column);
      line = uve->ident->startPosition.row;
   }
   else if (UndefinedProcedureException* upe = dynamic_cast<UndefinedProcedureException*>(parseException))
   {
      msg  = tr("undefined procedure") + space;
      msg += QString("\"") + QString::fromStdString(upe->ident->value) + QString("(") + QString::number(upe->nrParameters) + QString(")") + QString("\"");
      msg += at + QString::number(upe->ident->startPosition.row) + QString(";") + QString::number(upe->ident->startPosition.column);
      line = upe->ident->startPosition.row;
   }
   else if (UndefinedSymbolException* use = dynamic_cast<UndefinedSymbolException*>(parseException))
   {
      msg  = tr("undefined symbol") + space;
      msg += QString("\"") + QString::fromStdString(use->ident->value) + QString("\"");
      msg += at + QString::number(use->ident->startPosition.row) + QString(";") + QString::number(use->ident->startPosition.column);
      line = use->ident->startPosition.row;
   }
   else if (UnexpectedSymbolException* use = dynamic_cast<UnexpectedSymbolException*>(parseException))
   {
      msg  = tr("unexpected symbol") + space;
      msg += QString("\"") + QString::fromStdString(use->token->value) + QString("\"");
      msg += at + QString::number(use->token->position.row) + QString(";") + QString::number(use->token->position.column);
      line = use->token->position.row;
   }
   else if (MultipleDefinitionException* mde = dynamic_cast<MultipleDefinitionException*>(parseException))
   {
      msg  = tr("multiple definition of symbol") + space;
      msg += QString("\"") + QString::fromStdString(mde->ident->value);
      if (mde->nrParameters >= 0) msg += QString("(") + QString::number(mde->nrParameters) + QString(")");
      msg += QString("\"");
      msg += at + QString::number(mde->ident->startPosition.row) + QString(";") + QString::number(mde->ident->startPosition.column);
      line = mde->ident->startPosition.row;
   }
   else
   {
      msg = tr("unknown exception");
      line = 0;
   }

   QMessageBox::warning(this, tr("Parse Exception") ,msg, QMessageBox::Ok);

   if (line!=0) sourceWindow->mark_line(line);
   stopParsing();

}


void MainWindow::showAssemblerCode(std::list<instruction_t> instructions)
{
    codeWindow->showAssemblerCode(instructions);
    interpreterWindow->setInstructions(instructions);
}


void MainWindow::changeLanguage(QAction *action)
{
    changeLanguage(((LanguageAction*)action)->getLanguageFile());
}

void MainWindow::changeLanguage(std::string languageFile)
{
   if (translator != NULL) QCoreApplication::removeTranslator(translator);

   if (!languageFile.empty())
   {
       translator = new QTranslator();
       translator->load(QString::fromStdString(languageFile));
       QCoreApplication::installTranslator(translator);
   }

   LanguageManager::getInstance()->changeLanguage();
}

void MainWindow::initLanguages()
{
    languageMenu = new QMenu();

    QAction *languageAction = new LanguageAction(QIcon(":/icon/images/flags/gb.png"), "English", std::string(), this );
    languageAction->setStatusTip("switch to English");
    languageMenu->addAction(languageAction);

    languageAction = new LanguageAction(QIcon(":/icon/images/flags/de.png"), "Deutsch", "de.qm", this );
    languageAction->setStatusTip("auf Deutsch umstellen");
    languageMenu->addAction(languageAction);


    connect(languageMenu, SIGNAL(triggered(QAction *)), this, SLOT(changeLanguage(QAction *)));
}



std::string MainWindow::getSourceCode()
{
    return sourceWindow->getSourceCode();
}


void MainWindow::changeGUI(enum GuiState state)
{
   switch (state)
   {
       case LEXING_NEW:   { abortCodeGeneration(),abortParsing(); stopParsing(); stopLexing(); newLexing();   break; }
       case LEXING_START: { startLexing(); break; }
       case LEXING_STOP:  { stopLexing(); canStartParsing();  break; }
       case LEXING_ABORT: { stopLexing(); newLexing(); break; }
       case PARSING_START: { startParsing(); break; }
       case PARSING_STOP:  { stopParsing(); break; }
       case PARSING_FINISH:  { finishParsing(); break; }
       case PARSING_ABORT: { abortParsing(); break; }
       default: break;
   }
}


void MainWindow::addToken(Token<Pl0TokenType> *token)
{
     tokenTableWindow->addToken(token);
}


void MainWindow::removeFirstToken()
{
     tokenTableWindow->removeFirstToken();
}


bool MainWindow::highlightSource(position_t position, unsigned int length)
{
     return sourceWindow->highlight(position, length);
}

bool MainWindow::markSource(position_t start, position_t end)
{
     return sourceWindow->mark(start, end);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About PL/0 Compiler"),
             tr("The PL/0 Compiler itself\n\nDeveloper: Daniel Bohn, Sebastian Patschorke\n\nhttp://sourceforge.net/projects/pl0-compiler/"));
}

void MainWindow::createActions()
{
     openAction = new QAction(QIcon(":/icon/images/folder.png"), QString(), this);
     connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

     saveAction = new QAction(QIcon(":/icon/images/filesave.png"), QString(), this);
     connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

     saveAsAction = new QAction(QIcon(":/icon/images/filesaveas.png"), QString(), this);
     connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

     quitAction = new QAction(QIcon(":/icon/images/button_cancel.png"), QString(), this);
     connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

     aboutAction = new QAction(QIcon(":/icon/images/donkey-48x48.png"), QString(), this);
     connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

     aboutQtAction = new QAction(QIcon(":/icon/images/qt_logo.png"), QString(), this);
     connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

     grammarAction = new QAction(QString(), this);
     grammarAction->setCheckable(true);
     connect(grammarAction, SIGNAL(triggered()), this, SLOT(showGrammar()));
}

void MainWindow::createMenus()
{
     fileMenu = menuBar()->addMenu(QString());

     fileMenu->addAction(openAction);
     fileMenu->addAction(saveAction);
     fileMenu->addAction(saveAsAction);
     fileMenu->addSeparator();
     fileMenu->addAction(quitAction);

     viewMenu = menuBar()->addMenu(QString());
     viewMenu->addAction(grammarAction);
     menuBar()->addMenu(languageMenu);

     menuBar()->addSeparator();

     helpMenu = menuBar()->addMenu(QString());
     helpMenu->addAction(aboutAction);
     helpMenu->addAction(aboutQtAction);
}

void MainWindow::createToolBars()
{
     fileToolBar = addToolBar(QString());
     fileToolBar->addAction(openAction);
     fileToolBar->addAction(saveAction);

     QAction *emptyAction = new QAction(QIcon(":/icon/images/empty.png"), QString(), this);
     emptyAction->setEnabled(false);
     fileToolBar->addAction(emptyAction);

     lexerToolBar =  new LexerToolBar(this);
     addToolBar(lexerToolBar);

     parserToolBar =  new ParserToolBar(this);
     addToolBar(parserToolBar);

     codeToolBar =  new CodeToolBar(this);
     addToolBar(codeToolBar);

     interpreterToolBar = new InterpreterToolBar(this);
     addToolBar(interpreterToolBar);
}

void MainWindow::createStatusBar()
{
     statusBar()->showMessage(tr("Ready"));
}

void MainWindow::open()
{
   filename = QFileDialog::getOpenFileName( this, QString(tr("Open a PL/0 source file")), QString(), QString(), NULL, NULL ) ;

    if ( !filename.isEmpty() ) {
       sourceWindow->loadSourceFile(filename);
       changeGUI(LEXING_NEW);
    }
    else
       statusBar()->showMessage(tr("Loading aborted"));
}

void MainWindow::save()
{
   if ( !filename.isEmpty() ) {
       sourceWindow->saveFile(filename);
   }
    else {
       filename = QFileDialog::getSaveFileName( this, QString(tr("Save PL/0 source file")), QString(), QString(), NULL, NULL ) ;
       sourceWindow->saveFile(filename);
   }
}

void MainWindow::saveAs()
{
   filename = QFileDialog::getSaveFileName( this, QString(tr("Save PL/0 source file as")), QString(), QString(), NULL, NULL ) ;
   save();
}

void MainWindow::goLexerStepForward()
{ pl0controller->oneTokenForward(); }

void MainWindow::goLexerStepEnd()
{ pl0controller->allTokenForward(); }

void MainWindow::goParserStepForward()
{ pl0controller->parsingTillPause(); }

void MainWindow::goParserStepEnd()
{ pl0controller->completeParsing(); }

void MainWindow::goCodeGenerationStepEnd()
{ pl0controller->generateCodeComplete(); }


void MainWindow::stopLexing()
{
     /* disable stop button */
     lexerToolBar->setEnableStop(false);
     /* enable new button */
     lexerToolBar->setEnableNew(true);
     /* disable forward buttons */
     lexerToolBar->setEnableForward(false);
}

void MainWindow::newLexing()
{
     /* reset lexer */
     pl0controller->resetLexer();
     /* set source read only */
     sourceWindow->setReadOnly(false);
     /* disable new button */
     lexerToolBar->setEnableNew(false);
     /* enable forward buttons */
     lexerToolBar->setEnableForward(true);
     /* remove old rows */
     tokenTableWindow->clear();


     sourceWindow->reset();
}

void MainWindow::startLexing()
{
     /* set source read only */
     sourceWindow->setReadOnly(true);
     /* enable stop button */
     lexerToolBar->setEnableStop(true);
}

void MainWindow::canStartParsing()
{
   /* enable the Parser start buttons */
   parserToolBar->setEnableForward(true);
}



void MainWindow::startParsing()
{
    /* enable stop button */
    parserToolBar->setEnableStop(true);
}

void MainWindow::stopParsing()
{
    parserToolBar->setEnableForward(false);
    parserToolBar->setEnableStop(false);
}

void MainWindow::finishParsing()
{
    parserToolBar->setEnableForward(false);
    parserToolBar->setEnableStop(false);

    codeToolBar->setEnableForward(true);

}

void MainWindow::abortParsing()
{
      pl0controller->abortParsing();
      parseTreeWindow->clearTree();
      symbolTableWindow->clearTablesView();
}

void MainWindow::initGrammarWindow()
{
     grammarWindow = new GrammarWindow(this);
}

void MainWindow::showGrammar()
{
   grammarWindow->showGrammar(grammarAction->isChecked());
}

void MainWindow::grammarWindowWasSetVisible(bool visible)
{
   grammarAction->setChecked(visible);
}

void MainWindow::finish_CodeGeneration()
{
   codeToolBar->setEnableForward(false);
   interpreterToolBar->setEnabled(true);
}

void MainWindow::abortCodeGeneration()
{
   codeWindow->clearListWidget();
   pl0controller->clearCodeGenerator();
   finish_CodeGeneration();
   interpreterToolBar->setEnabled(false);
   interpreterWindow->setVisible(false);

}

void MainWindow::showInterpreterWindow()
{
   interpreterWindow->reset();
   interpreterWindow->showInterpreterWindow();
}
