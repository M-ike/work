#ifndef GRAMMARWINDOW_H
#define GRAMMARWINDOW_H

#include <QWidget>
#include "ilanguagewidget.h"
#include "languagemanager.h"

#include "mainwindow.h"

class QWidget;
class QTextEdit;
class MainWindow;

class GrammarWindow: public QWidget, ILanguageWidget
{
 Q_OBJECT
   public:
      GrammarWindow(MainWindow *mainWindow);
      void languageNotify();
      void showGrammar(bool showIt);
      void setVisible(bool visible);

   private:
      void init();
      QWidget *grammarWindow;
      QTextEdit *grammarTextEdit;
      MainWindow *parent;
};

#endif
