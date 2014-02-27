#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QListWidgetItem>

#include "interpreterwindow.h"

#include "mainwindow.h"

InterpreterWindow::InterpreterWindow(MainWindow *parent)
   :QWidget()
{
    this->parent = parent;
    //returnWasPressed = false;
    interpreting = false;
    interpreter = NULL;
    //interpreterWindow = new QWidget(this);
    init();
    languageNotify();
}


void InterpreterWindow::languageNotify()
{
    setWindowTitle(tr("Interpretation Window") );
}

void InterpreterWindow::init()
{
   setBaseSize(600,800);

   outputWindow = new QListWidget();

   QVBoxLayout *layout = new QVBoxLayout();
   layout->setContentsMargins(1,8,1,1);
   layout->addWidget(outputWindow);

   outputWindow->setStyleSheet("QListWidget {background-color: black; color : white}");

   inputWindow = new QLineEdit();
   layout->addWidget(inputWindow);
   setLayout(layout);

   connect(inputWindow, SIGNAL(returnPressed()), this, SLOT(returnPressed()));
}

void InterpreterWindow::showInterpreterWindow()
{
   interpreter = new Interpreter(this);
   interpreter->setInstructions(instructions);
   show();
}

void InterpreterWindow::reset()
{
   outputWindow->clear();
   inputWindow->clear();
   outputWindow->addItem(tr("Press Enter to start..."));
   //returnWasPressed = false;
   interpreting = false;
   if (interpreter!=NULL)
   {
     delete interpreter;
     interpreter=NULL;
   }
}

void InterpreterWindow::setInstructions(std::list<instruction_t> instructions)
{
   this->instructions = instructions;
}

void InterpreterWindow::returnPressed()
{
   if (interpreter == NULL) return;

   if (! interpreting)
      {
          interpreting = true; inputWindow->clear(); interpreter->run();
      }
   else
   {
       bool ok; int inputValue;
       ok = getValueFromInput(&inputValue);
       if (ok)
       {
          inputWindow->clear();
          interpreter->setInput(inputValue);
          interpreter->run();
       }
       else
       {
          QString msg = "\""; msg.append(inputWindow->text()); msg.append("\" "); msg.append(tr("isn't a valid int"));
          println(msg.toStdString());
          inputWindow->clear();
       }
   }
}

void InterpreterWindow::println(std::string str)
{
   outputWindow->addItem(QString::fromStdString(str));
}

void InterpreterWindow::waitForInput()
{
       println((tr("Enter input:")).toStdString());
}

void InterpreterWindow::finishedInterpretaton()
{
       println((tr("End")).toStdString());
       delete interpreter;
       interpreter = NULL;
}

bool InterpreterWindow::getValueFromInput(int *value)
{
   QString str = inputWindow->text();
   bool ok;
   *value = str.toInt(&ok, 10);
   return ok;
}
