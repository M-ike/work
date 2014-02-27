#ifndef SYMBOLTABLEWINDOW_H
#define SYMBOLTABLEWINDOW_H

#include <stack>
#include <list>

#include <QDockWidget>
#include <QTreeWidget>
#include <QTableWidget>

#include "mainwindow.h"
#include "treeitem.h"

#include "ilanguagewidget.h"
#include "languagemanager.h"

#include "../parser/symboltable.h"

class MainWindow;
class SymbolTable;

class SymbolTableWindow: public QDockWidget, ILanguageWidget
{
 Q_OBJECT

 public:
     SymbolTableWindow(MainWindow *mainWindow);
     void languageNotify();
     void clearTablesView();
     void addSymbolTable(SymbolTable *symbolTable);

 private:
     MainWindow *parent;
     QTreeWidget *symbolTableTreeWidget;
     QTableWidget *symbolTableWidget;
     std::list<AST::ASTTree*> symbolReferences;

     std::stack<TreeItem<SymbolTable>*> treeWitgetItemStack;
     std::list<TreeItem<SymbolTable>*> treeItems;

     TreeItem<SymbolTable> *searchTreeItem(SymbolTable *symbolTable);
     TreeItem<SymbolTable> *addTreeChild(TreeItem<SymbolTable> *parent, QStringList line);
     void setTableHeaders();
     QString getSymbolType(enum SymbolTable::SymbolType symbolType, int nrParameters);
     void translateSymboltable();

     void insertSymbols(std::list<SymbolTable::symbol_t> symbols);

 private slots:
     void setCellContent(int row, int column, QString text);
     void treeItemSelected();
     void tableItemSelected(int row, int column);
};

#endif
