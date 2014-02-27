#ifndef INTERPRETERWINDOW_H
#define INTERPRETERWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QLineEdit>
#include "ilanguagewidget.h"
#include "languagemanager.h"

#include "mainwindow.h"

#include "../interpreter/interpreter.h"
#include "../codegenerator/instruction.h"

#include "../common.h"

class Interpreter;

class InterpreterWindow: public QWidget, ILanguageWidget
{
  Q_OBJECT
     public:
       InterpreterWindow(MainWindow *mainWindow);
       void languageNotify();
       void showInterpreterWindow();
       void reset();
       void setInstructions(std::list<instruction_t> instructions);
       void println(std::string str);
       void waitForInput();
       void finishedInterpretaton();

    private:
      //QWidget *interpreterWindow;
      MainWindow *parent;
      void init();
      QListWidget *outputWindow;
      QLineEdit *inputWindow;
      Interpreter *interpreter;
      //bool returnWasPressed;
      bool interpreting;
      bool getValueFromInput(int *value);
      std::list<instruction_t> instructions;

    private slots:
       void returnPressed();
};

#endif
