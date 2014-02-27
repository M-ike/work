#ifndef TOKENTABLEWINDOW_H
#define TOKENTABLEWINDOW_H

#include <QDockWidget>
#include <QTableWidget>

#include <list>

#include "mainwindow.h"

#include "../lexer/token.h"
#include "../lexer/pl0token.h"

#include "ilanguagewidget.h"
#include "languagemanager.h"

#include "../common.h"
#include "../lexer/token.h"
#include "../lexer/pl0token.h"

class QWidget;
class QTableWidget;
class MainWindow;



class TokenTableWindow: public QDockWidget, ILanguageWidget
{
 Q_OBJECT

 public:
     TokenTableWindow(MainWindow *mainWindow);
     void clear();
     void addToken(Token<enum Pl0TokenType> *token);
     void removeFirstToken();
     void languageNotify();

 private:
     MainWindow *parent;
     QTableWidget *lexerTableWidget;
     std::list<Token<enum Pl0TokenType> > tokenList;

     QString getTokenClassName(enum Pl0TokenClass tokenClass);
     void setHeaderNames();
     void addToken(position_t position, enum Pl0TokenClass tokenClass, std::string value);
     void setCellContent(int row, int column, QString text);

     void translateTable();

     void setColCount(int count);
     void setRowCount(int count);
};

#endif
