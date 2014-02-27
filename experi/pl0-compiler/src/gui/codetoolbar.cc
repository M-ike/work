#include <QIcon>
#include <iostream>

#include "codetoolbar.h"

CodeToolBar::CodeToolBar(MainWindow *parent)
  : QToolBar(parent)
{
   this->parent = parent;

   LanguageManager::getInstance()->registerWidget(this);

   createActions();

   

   languageNotify();
}

void CodeToolBar::languageNotify()
{
    setWindowTitle( tr("Codegenerator Toolbar") );

    stepEnd->setText(tr("generate all"));
    stepEnd->setStatusTip(tr("generate all"));
}

void CodeToolBar::createActions()
{
     stepEnd = new QAction(QIcon(":/icon/images/finish_red.png"), QString(), this);
     stepEnd->setEnabled(false);
     connect(stepEnd, SIGNAL(triggered()), this, SLOT(goStepEnd()));
     addAction(stepEnd);

     QAction *emptyAction = new QAction(QIcon(":/icon/images/empty.png"), QString(), this);
     emptyAction->setEnabled(false);
     addAction(emptyAction);
}



void CodeToolBar::goStepEnd()
{
    parent->goCodeGenerationStepEnd();
}


void CodeToolBar::setEnableForward(bool enable)
{
     stepEnd->setEnabled(enable);
}
