#ifndef PARSERTOOLBAR_H
#define PARSERTOOLBAR_H

#include <QToolBar>
#include <QAction>

#include "mainwindow.h"

#include "ilanguagewidget.h"
#include "languagemanager.h"

class ParserToolBar: public QToolBar, ILanguageWidget
{
 Q_OBJECT

 public:
     ParserToolBar(MainWindow *parent);
     void setEnableForward(bool enable);
     void setEnableStop(bool enable);

     void languageNotify();

 private slots:
     void goStepForward();
     void goStepEnd();
     void abortParsing();

 private:
     MainWindow *parent;
     QAction *stepForward;
     QAction *stepEnd;
     QAction *stopAction;
     void createActions();
};


#endif
