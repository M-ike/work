#ifndef PARSETREEWINDOW_H
#define PARSETREEWINDOW_H

#include <QDockWidget>
#include <QTreeWidget>

#include "mainwindow.h"
#include "symboltablewindow.h"

#include "../parser/ast.h"

#include "ilanguagewidget.h"
#include "languagemanager.h"

class MainWindow;
class SymbolTableWindow;

class ParseTreeWindow: public QDockWidget, ILanguageWidget
{
 Q_OBJECT

 public:
     ParseTreeWindow(MainWindow *mainWindow, SymbolTableWindow *symbolTableWindow);
     void showTree(AST::ASTTree *astTree);
     void languageNotify();
     void clearTree();

 private:
     MainWindow *parent;
     SymbolTableWindow *symbolTableWindow;
     QTreeWidget *parseTreeWidget;

 private slots:
     void treeItemSelected();
};

#endif
