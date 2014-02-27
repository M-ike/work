#ifndef LEXERTOOLBAR_H
#define LEXERTOOLBAR_H

#include <QToolBar>
#include <QAction>

#include "mainwindow.h"

#include "ilanguagewidget.h"
#include "languagemanager.h"

class LexerToolBar: public QToolBar, ILanguageWidget
{
 Q_OBJECT

 public:
     LexerToolBar(MainWindow *parent);
     void setEnableForward(bool enable);
     void setEnableNew(bool enable);
     void setEnableStop(bool enable);

     void languageNotify();

 private slots:
     void goStepForward();
     void goStepEnd();
     void abortLexing();
     void newLexing();

 private:
     MainWindow *parent;
     QAction *newAction;
     QAction *stepForward;
     QAction *stepEnd;
     QAction *stopAction;
     void createActions();
};


#endif
