#include <QIcon>
#include <iostream>
#include "lexertoolbar.h"

LexerToolBar::LexerToolBar(MainWindow *parent)
  : QToolBar(parent)
{
   this->parent = parent;

   LanguageManager::getInstance()->registerWidget(this);

   createActions();

   languageNotify();
}

void LexerToolBar::languageNotify()
{
    setWindowTitle( tr("Lexer Toolbar") );

    stepForward->setText(tr("one lexer step forward"));
    stepForward->setStatusTip(tr("go one lexer step forward"));

    stepEnd->setText(tr("lex till end"));
    stepEnd->setStatusTip(tr("lex till end"));

    stopAction->setText(tr("stop"));
    stopAction->setStatusTip(tr("stop lexing process"));

    newAction->setText(tr("new"));
    newAction->setStatusTip(tr("start new lexing process"));
}

void LexerToolBar::createActions()
{
     newAction = new QAction(QIcon(":/icon/images/new.png"), QString(), this);
     newAction->setEnabled(false);
     connect(newAction, SIGNAL(triggered()), this, SLOT(newLexing()));
     addAction(newAction);

     stepForward = new QAction(QIcon(":/icon/images/forward_green.png"), QString(), this);
     connect(stepForward, SIGNAL(triggered()), this, SLOT(goStepForward()));
     addAction(stepForward);

     stepEnd = new QAction(QIcon(":/icon/images/finish_green.png"), QString(), this);
     connect(stepEnd, SIGNAL(triggered()), this, SLOT(goStepEnd()));
     addAction(stepEnd);

     stopAction = new QAction(QIcon(":/icon/images/stop.png"), QString(), this);
     stopAction->setEnabled(false);
     connect(stopAction, SIGNAL(triggered()), this, SLOT(abortLexing()));
     addAction(stopAction);

     QAction *emptyAction = new QAction(QIcon(":/icon/images/empty.png"), QString(), this);
     emptyAction->setEnabled(false);
     addAction(emptyAction);
}

void LexerToolBar::goStepForward()
{
    parent->goLexerStepForward();
}

void LexerToolBar::goStepEnd()
{
    parent->goLexerStepEnd();
}

void LexerToolBar::abortLexing()
{
    parent->changeGUI(LEXING_ABORT);
}

void LexerToolBar::newLexing()
{
    parent->changeGUI(LEXING_NEW);
}

void LexerToolBar::setEnableForward(bool enable)
{
     stepForward->setEnabled(enable);
     stepEnd->setEnabled(enable);
}

void LexerToolBar::setEnableNew(bool enable)
{
     newAction->setEnabled(enable);
}

void LexerToolBar::setEnableStop(bool enable)
{
     stopAction->setEnabled(enable);
}
