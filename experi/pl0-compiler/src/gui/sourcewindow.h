#ifndef SOURCEWINDOW_H
#define SOURCEWINDOW_H

#include <QDockWidget>
#include <QTextEdit>

#include "mainwindow.h"
#include "languagemanager.h"
#include "../common.h"

class QString;
class QWidget;
class QTextEdit;
class MainWindow;

class SourceWindow: public QDockWidget, ILanguageWidget
{
 Q_OBJECT

 public:
     SourceWindow(MainWindow *mainWindow);
     void loadSourceFile(const QString &fileName);
     void saveFile( const QString &fileName );
     std::string getSourceCode();
     void setReadOnly(bool ro);
     bool highlight(position_t position, unsigned int length);
     bool mark_line(unsigned int line);
     bool mark(position_t start, position_t end);
     void languageNotify();
     void reset();

 private:
     void scrollToRow(unsigned int row);
     bool mark(position_t start, position_t end, QString htmlHeader, QString htmlStartTag, QString htmlEndTag);
     unsigned int numberOfRows();
     QString getLine(unsigned int nr);
     void makeHTMLconform(QString *qstr);
     MainWindow *parent;
     QString text;
     QTextEdit *sourceTextEdit;
};

#endif
