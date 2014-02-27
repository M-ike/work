#include <QIcon>
#include <iostream>

#include "parsertoolbar.h"

ParserToolBar::ParserToolBar(MainWindow *parent)
  : QToolBar(parent)
{
   this->parent = parent;

   LanguageManager::getInstance()->registerWidget(this);

   createActions();

   languageNotify();
}

void ParserToolBar::languageNotify()
{
    setWindowTitle( tr("Parser Toolbar") );

    stepForward->setText(tr("parse next rule"));
    stepForward->setStatusTip(tr("parse next rule"));

    stepEnd->setText(tr("parse all"));
    stepEnd->setStatusTip(tr("parse all"));
}

void ParserToolBar::createActions()
{
     stepForward = new QAction(QIcon(":/icon/images/forward_blue.png"), QString(), this);
     stepForward->setEnabled(false);
     connect(stepForward, SIGNAL(triggered()), this, SLOT(goStepForward()));
     addAction(stepForward);

     stepEnd = new QAction(QIcon(":/icon/images/finish_blue.png"), QString(), this);
     stepEnd->setEnabled(false);
     connect(stepEnd, SIGNAL(triggered()), this, SLOT(goStepEnd()));
     addAction(stepEnd);

     stopAction = new QAction(QIcon(":/icon/images/stop.png"), QString(), this);
     stopAction->setEnabled(false);
     connect(stopAction, SIGNAL(triggered()), this, SLOT(abortParsing()));
     addAction(stopAction);

     QAction *emptyAction = new QAction(QIcon(":/icon/images/empty.png"), QString(), this);
     emptyAction->setEnabled(false);
     addAction(emptyAction);
}


void ParserToolBar::goStepForward()
{
    parent->goParserStepForward();
}


void ParserToolBar::goStepEnd()
{
    parent->goParserStepEnd();
}


void ParserToolBar::abortParsing()
{
   parent->changeGUI(PARSING_ABORT);
}

void ParserToolBar::setEnableForward(bool enable)
{
     stepForward->setEnabled(enable);
     stepEnd->setEnabled(enable);
}


void ParserToolBar::setEnableStop(bool enable)
{
     stopAction->setEnabled(enable);
}

