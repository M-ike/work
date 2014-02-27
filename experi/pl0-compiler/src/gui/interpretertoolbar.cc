#include <QIcon>
#include <iostream>

#include "interpretertoolbar.h"

InterpreterToolBar::InterpreterToolBar(MainWindow *parent)
  : QToolBar(parent)
{
   this->parent = parent;

   LanguageManager::getInstance()->registerWidget(this);


   createActions();

   languageNotify();
}

void InterpreterToolBar::languageNotify()
{
    setWindowTitle( tr("Interpreter Toolbar") );

    interpret->setText(tr("interpretation"));
    interpret->setStatusTip(tr("interpret the assembler code"));
}

void InterpreterToolBar::createActions()
{
     interpret = new QAction(QIcon(":/icon/images/interpreter.png"), QString(), this);
     interpret->setEnabled(false);
     connect(interpret, SIGNAL(triggered()), this, SLOT(interpretation()));
     addAction(interpret);

     QAction *emptyAction = new QAction(QIcon(":/icon/images/empty.png"), QString(), this);
     emptyAction->setEnabled(false);
     addAction(emptyAction);
}

void InterpreterToolBar::interpretation()
{
   parent->showInterpreterWindow();
}

void InterpreterToolBar::setEnabled(bool enable)
{
    interpret->setEnabled(enable);
}


