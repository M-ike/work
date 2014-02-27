#include "symboltablewindow.h"

#include <iostream>
#include <QSplitter>
#include <QStringList>
#include <QTreeWidgetItem>
#include <QScrollBar>


SymbolTableWindow::SymbolTableWindow(MainWindow *parent)
  : QDockWidget(parent)
{
   this->parent = parent;

   LanguageManager::getInstance()->registerWidget(this);

   symbolTableTreeWidget = new QTreeWidget(this);;
   symbolTableTreeWidget->setColumnWidth(0,100);
   connect(symbolTableTreeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(treeItemSelected()));

   symbolTableWidget = new QTableWidget(this);
   connect(symbolTableWidget, SIGNAL(cellClicked(int,int)), this, SLOT(tableItemSelected(int,int)));
   symbolTableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);

   symbolTableWidget->setColumnCount(4);
   symbolTableWidget->resizeColumnsToContents();
   symbolTableWidget->setColumnWidth(0,90);
   symbolTableWidget->setColumnWidth(1,130);
   symbolTableWidget->setColumnWidth(2,50);
   symbolTableWidget->setColumnWidth(3,60);

   QSplitter *splitter = new QSplitter;
   splitter->addWidget(symbolTableTreeWidget);
   splitter->addWidget(symbolTableWidget);
   splitter->setStretchFactor(0, 1);
   splitter->setStretchFactor(1, 2);
   splitter->setCollapsible (0, false);
   splitter->setCollapsible (1, false);

   setWidget(splitter);

   languageNotify();
}



void SymbolTableWindow::languageNotify()
{
    setWindowTitle( tr("Symbol Tables") );

    QStringList header;
    header.append(tr("Symbol Tables"));
    symbolTableTreeWidget->setHeaderLabels( header );

    setTableHeaders();

    translateSymboltable();
}


void SymbolTableWindow::translateSymboltable()
{
    QScrollBar *vsb = symbolTableWidget->verticalScrollBar();
    QScrollBar *hsb = symbolTableWidget->horizontalScrollBar();
    int vVal = vsb->value();
    int hVal = hsb->value();

    treeItemSelected();

    vsb->setValue(vVal);
    hsb->setValue(hVal);
}


void SymbolTableWindow::treeItemSelected()
{
   TreeItem<SymbolTable> *currentItem = (TreeItem<SymbolTable>*)(symbolTableTreeWidget->currentItem());
   if (currentItem == NULL) return;

   //collect all symbols
   SymbolTable *treeItem = currentItem->getReference();
   std::list<SymbolTable::symbol_t>  symbols = treeItem->getDirectOwnedSymbols();
   std::list<SymbolTable::symbol_t>  indirectSymbols = treeItem->getIndirectKnownSymbols();
   if (!indirectSymbols.empty())
   {
      std::list<SymbolTable::symbol_t>::iterator it;
      for( it = indirectSymbols.begin(); it != indirectSymbols.end(); it++ )
         symbols.push_back(*it);
   }

   setTableHeaders();

   symbolReferences.clear();

   symbolTableWidget->clear();
   symbolTableWidget->setRowCount(0);

   insertSymbols(symbols);
}


void SymbolTableWindow::insertSymbols(std::list<SymbolTable::symbol_t> symbols)
{
   std::list<SymbolTable::symbol_t>::iterator it;
   for (it = symbols.begin(); it != symbols.end(); it++)
   {
      int currRowCount = symbolTableWidget->rowCount();
      symbolTableWidget->insertRow(currRowCount);

      QString str;

      str = QString::fromStdString(it->name);
      if (it->type == SymbolTable::PROCEDURE)
      {
        str.append(" (");
        str.append( QString::number(it->nrParameters) );
        str.append(")");
      }

      setCellContent(currRowCount, 0, str);

      str = getSymbolType(it->type, it->nrParameters);
      setCellContent(currRowCount, 1, str);

      str = QString::number(it->reference->startPosition.row);
      setCellContent(currRowCount, 2, str);

      str = QString::number(it->reference->startPosition.column);
      setCellContent(currRowCount, 3, str);

      symbolReferences.push_back(it->reference);
   }

   setTableHeaders();
}


void SymbolTableWindow::setCellContent(int row, int column, QString text)
{
  QTableWidgetItem *cellItem = new QTableWidgetItem();
  cellItem->setText(text);
  cellItem->setTextAlignment(Qt::AlignCenter);
  symbolTableWidget->setItem(row,column,cellItem);
}


void SymbolTableWindow::tableItemSelected(int row, int column)
{
   std::list<AST::ASTTree*> tmp = symbolReferences;
   for (int i = 0; i<row; i++)
       tmp.pop_front();
   AST::ASTTree *tree = tmp.front();
   parent->markSource(tree->startPosition, tree->endPosition);
}


void SymbolTableWindow::clearTablesView()
{
    symbolTableTreeWidget->clear();
    symbolTableWidget->clear();
    symbolTableWidget->setRowCount(0);
    treeItems.clear();
    setTableHeaders();
}


void SymbolTableWindow::addSymbolTable(SymbolTable *symbolTable)
{
   QStringList line;
   QString help="SYM::";
   help.append(QString::fromStdString(symbolTable->name()));
   line.append(help);
   TreeItem<SymbolTable> *parentItem = searchTreeItem(symbolTable->getParentSymbolTable());

   TreeItem<SymbolTable> *newItem = addTreeChild(parentItem, line);

   newItem->setReference(symbolTable);
   treeItems.push_back(newItem);
   symbolTableTreeWidget->expandAll();
}


TreeItem<SymbolTable> *SymbolTableWindow::addTreeChild(TreeItem<SymbolTable> *parent, QStringList line)
{
   TreeItem<SymbolTable> *child = new TreeItem<SymbolTable>(parent, line);
   if (parent == NULL)
      symbolTableTreeWidget->addTopLevelItem(child);
   else
      parent->addChild(child);
   return child;
}


TreeItem<SymbolTable> *SymbolTableWindow::searchTreeItem(SymbolTable *symbolTable)
{
   std::list<TreeItem<SymbolTable>*>::iterator it;
   for (it = treeItems.begin(); it != treeItems.end(); it++)
   {
      if ((*it)->getReference() == symbolTable) return *it;
   }
   return NULL;
}


QString SymbolTableWindow::getSymbolType(enum SymbolTable::SymbolType symbolType, int nrParameters)
{
 switch (symbolType) {
   case SymbolTable::CONSTANT:  return tr("CONSTANT"); break;
   case SymbolTable::VARIABLE:  return tr("VARIABLE"); break;
   case SymbolTable::PROCEDURE: { return tr("PROCEDURE"); break;}
  }
  return QString("unknown symbol type");
};


void SymbolTableWindow::setTableHeaders()
{
    QTableWidgetItem *colItem = new QTableWidgetItem();
    colItem->setText(tr("Name"));
    symbolTableWidget->setHorizontalHeaderItem(0, colItem);
    colItem = new QTableWidgetItem();
    colItem->setText(tr("Type"));
    symbolTableWidget->setHorizontalHeaderItem(1, colItem);
    colItem = new QTableWidgetItem();
    colItem->setText(tr("Row"));
    symbolTableWidget->setHorizontalHeaderItem(2, colItem);
    colItem = new QTableWidgetItem();
    colItem->setText(tr("Column"));
    symbolTableWidget->setHorizontalHeaderItem(3, colItem);
}
