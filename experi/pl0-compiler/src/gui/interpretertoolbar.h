#ifndef INTERPRETERTOOLBAR_H
#define INTERPRETERTOOLBAR_H

#include <QToolBar>
#include <QAction>

#include "mainwindow.h"

#include "ilanguagewidget.h"
#include "languagemanager.h"

class InterpreterToolBar: public QToolBar, ILanguageWidget
{
 Q_OBJECT
    public:
      InterpreterToolBar(MainWindow *parent);

      void languageNotify();
      void setEnabled(bool enable);

   private slots:
    void interpretation();


    private:
     MainWindow *parent;
     QAction *interpret;
     void createActions();
};


#endif
