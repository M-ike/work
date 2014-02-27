#include <QScrollBar>
#include "tokentablewindow.h"

class QTableWidget;
class QHeaderView;
class QSizePolicy;
class QTableWidgetItem;

TokenTableWindow::TokenTableWindow(MainWindow *parent)
  : QDockWidget(parent)
{
   this->parent = parent;
   lexerTableWidget = new QTableWidget(this);
   lexerTableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
   setWidget(lexerTableWidget);

   LanguageManager::getInstance()->registerWidget(this);

   /* build table */
   /*Initialisation*/
   lexerTableWidget->setColumnCount(4);
   lexerTableWidget->resizeColumnsToContents();
   lexerTableWidget->setColumnWidth(3,100);
   lexerTableWidget->setColumnWidth(2,100);
   lexerTableWidget->setColumnWidth(1,80);
   lexerTableWidget->setColumnWidth(0,50);
   lexerTableWidget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

   languageNotify();
}

void TokenTableWindow::languageNotify()
{
    setWindowTitle( tr("Token Table") );
    setHeaderNames();
    translateTable();
}

void TokenTableWindow::translateTable()
{
    if (tokenList.empty()) return;

    QScrollBar *vsb = lexerTableWidget->verticalScrollBar();
    QScrollBar *hsb = lexerTableWidget->horizontalScrollBar();
    int vVal = vsb->value();
    int hVal = hsb->value();

    setRowCount(0);
    std::list<Token<enum Pl0TokenType> >::iterator it;
    for (it = tokenList.begin(); it != tokenList.end(); it++)
    {
      position_t p = it->position;
      enum Pl0TokenClass c = getTokenClass(it->type);
      std::string v = it->value;
      addToken(p, c, v);
    }

    vsb->setValue(vVal);
    hsb->setValue(hVal);
}

void TokenTableWindow::setColCount(int count)
{
  lexerTableWidget->setColumnCount(count);
}

void TokenTableWindow::setRowCount(int count)
{
  lexerTableWidget->setRowCount(count);
}

void TokenTableWindow::setHeaderNames()
{
   QTableWidgetItem *colItem = new QTableWidgetItem();
   colItem->setText(tr("Row"));
   lexerTableWidget->setHorizontalHeaderItem(0, colItem);

   colItem = new QTableWidgetItem();
   colItem->setText(tr("Column"));
   lexerTableWidget->setHorizontalHeaderItem(1, colItem);

   colItem = new QTableWidgetItem();
   colItem->setText(tr("Class"));
   lexerTableWidget->setHorizontalHeaderItem(2, colItem);

   colItem = new QTableWidgetItem();
   colItem->setText(tr("Value"));
   lexerTableWidget->setHorizontalHeaderItem(3, colItem);
}


void TokenTableWindow::addToken(Token<enum Pl0TokenType> *token)
{
   addToken(token->position,getTokenClass(token->type), token->value);
   tokenList.push_back(*token);
}


void TokenTableWindow::removeFirstToken()
{
    lexerTableWidget->removeRow(0);
    tokenList.pop_front();
}

void TokenTableWindow::addToken(position_t position, enum Pl0TokenClass tokenClass, std::string value)
{

  int currRowCount = lexerTableWidget->rowCount();
  lexerTableWidget->insertRow( currRowCount );

  QString str;
  str.setNum(position.row);
  setCellContent(currRowCount, 0, str);
  str.setNum(position.column);
  setCellContent(currRowCount, 1, str);
  setCellContent(currRowCount, 2, getTokenClassName(tokenClass));
  setCellContent(currRowCount, 3, value.c_str());

  lexerTableWidget->scrollToBottom ();
}


void TokenTableWindow::setCellContent(int row, int column, QString text)
{
  QTableWidgetItem *cellItem = new QTableWidgetItem();
  cellItem->setText(text);
  cellItem->setTextAlignment(Qt::AlignCenter);
  lexerTableWidget->setItem(row,column,cellItem);
}

void TokenTableWindow::clear()
{
   setRowCount(0);
   tokenList.clear();
}

QString TokenTableWindow::getTokenClassName(enum Pl0TokenClass tokenClass)
{
 switch (tokenClass) {
   case CLASS_KEYWORD:    return QString(tr("Keyword")); break;
   case CLASS_IDENTIFIER: return QString(tr("Identifier")); break;
   case CLASS_DELIMITER:  return QString(tr("Delimiter")); break;
   case CLASS_CONSTANTS:  return QString(tr("Constants")); break;
   case CLASS_OPERATORS:  return QString(tr("Operators")); break;
   case CLASS_UNKNOWN:    return QString(tr("Unknown")); break;
  }
  return QString("unknown token class");
};
