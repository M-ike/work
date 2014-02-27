#ifndef CODEWINDOW_H
#define CODEWINDOW_H

#include <QDockWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <list>
#include <QLabel>

#include "mainwindow.h"
#include "languagemanager.h"

#include "../codegenerator/instruction.h"

class QAction;
class QString;
class MainWindow;

class CodeWindow: public QDockWidget, ILanguageWidget
{
 Q_OBJECT

 public:
     CodeWindow(MainWindow *mainWindow);
     void languageNotify();
     void showAssemblerCode(std::list<instruction_t> instructions);
     void clearListWidget();


  private slots:
    void changeCodeScrollbarAction(int pos);
    void changeLabelScrollbarAction(int pos);
    void changeLabelWindowCurrentRow(int row);
    void changeCodeWindowCurrentRow(int row);

 private:
     MainWindow *parent;
     QListWidget *labelTextEdit;
     QListWidget *codeTextEdit;
     QLabel *leftLabel;
     QLabel *rightLabel;

     QAction *changeAction;

     QString convertInstructionToQString(instruction_t instruction);
};

#endif
