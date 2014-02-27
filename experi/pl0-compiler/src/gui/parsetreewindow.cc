#include "parsetreewindow.h"
#include "parsetreevisitor.h"

#include <iostream>

#include "treeitem.h"
#include "symboltablewindow.h"

ParseTreeWindow::ParseTreeWindow(MainWindow *mainWindow, SymbolTableWindow *symbolTableWindow)
  : QDockWidget(mainWindow)
{
   this->parent = mainWindow;
   this->symbolTableWindow = symbolTableWindow;

   LanguageManager::getInstance()->registerWidget(this);

   parseTreeWidget = new QTreeWidget(this);
   setWidget(parseTreeWidget);
   parseTreeWidget->setColumnCount(3);
   parseTreeWidget->setColumnWidth(0,380);
   parseTreeWidget->setColumnWidth(1,50);
   parseTreeWidget->setColumnWidth(2,50);

   connect(parseTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(treeItemSelected()));

   languageNotify();
}


void ParseTreeWindow::showTree(AST::ASTTree *astTree)
{
   parseTreeWidget->clear();
   symbolTableWindow->clearTablesView();

   ParseTreeVisitor *parseTreeVisitor = new ParseTreeVisitor(parseTreeWidget, symbolTableWindow);
   astTree->accept(parseTreeVisitor);
   parseTreeWidget->expandAll();
   delete parseTreeVisitor;
}

void ParseTreeWindow::languageNotify()
{
    setWindowTitle( tr("Parse Tree") );

    QStringList header;
    header.append(tr("Tree"));
    header.append(tr("Row"));
    header.append(tr("Column"));
    parseTreeWidget->setHeaderLabels( header );
}

void ParseTreeWindow::treeItemSelected()
{
   AST::ASTTree *treeItem = ((TreeItem<AST::ASTTree>*)parseTreeWidget->currentItem())->getReference();
   parent->markSource(treeItem->startPosition, treeItem->endPosition);
}


void ParseTreeWindow::clearTree()
{
   parseTreeWidget->clear();
}
