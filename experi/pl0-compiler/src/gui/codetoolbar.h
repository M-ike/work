#ifndef CODETOOLBAR_H
#define CODETOOLBAR_H

#include <QToolBar>
#include <QAction>

#include "mainwindow.h"

#include "ilanguagewidget.h"
#include "languagemanager.h"

class CodeToolBar: public QToolBar, ILanguageWidget
{
 Q_OBJECT

 public:
     CodeToolBar(MainWindow *parent);

     void setEnableForward(bool enable);

     void languageNotify();

 private slots:
     void goStepEnd();

 private:
     MainWindow *parent;
     QAction *stepEnd;
     void createActions();
};


#endif
