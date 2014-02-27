#include "sourcewindow.h"
#include <QFile>
#include <QTextStream>
#include <QStatusBar>
#include <QScrollBar>

#include <iostream>

class QTextEdit;

SourceWindow::SourceWindow(MainWindow *parent)
  : QDockWidget(parent)
{
   this->parent = parent;
   LanguageManager::getInstance()->registerWidget(this);

   sourceTextEdit = new QTextEdit(this);
   setWidget(sourceTextEdit);

   languageNotify();
}

bool SourceWindow::highlight(position_t position, unsigned int length)
{
   position_t end = position;
   end.column += length-1;
   bool result = mark(position, end, QString(), "<b><font size=+2 color = red>", "</font></b>");
   if (result) scrollToRow(position.row);
   return result;
}


bool SourceWindow::mark(position_t start, position_t end)
{
   QString header = "<style type=\"text/css\"> .mark { background-color:#FFFF66; } </style>";
   bool result = mark(start, end, header, "<span class=\"mark\">", "</span>");
   if (result) scrollToRow(start.row);
   return result;
}

bool SourceWindow::mark_line(unsigned int line)
{
   if ( (line <= 0) || (line > numberOfRows()) ) return false;
   QString lineText = getLine(line);

   position_t start, end;
   start.row = line; end.row = line;
   start.column = 1; end.column = lineText.length();

   QString header = "<style type=\"text/css\"> .mark { background-color:#FF0000; } </style>";
   bool result = mark(start, end, header, "<span class=\"mark\">", "</span>");
   if (result) scrollToRow(start.row);
   return result;
}

bool SourceWindow::mark(position_t start, position_t end, QString htmlHeader, QString htmlStartTag, QString htmlEndTag)
/* row  and  column start with 1
   [start;end] will be marked */
{
   if ( (start.row < 1) || (start.column < 1) ) return false;
   if ( (end.row < 1)   || (end.column < 1)   ) return false;

   if ( start.row > end.row ) return false;
   if ( start.row == end.row && start.column > end.column ) return false;

   /*search start row */
   int startRowPos=0;
   for (int i=start.row; i>1; i--)
   {
      startRowPos = text.indexOf('\n', startRowPos) + 1 ;
      if (startRowPos == 0) return false; /* text doesn't have enough rows */
   }
   /*search end row */
   int endRowPos=startRowPos;
   for (int i=end.row-start.row+1; i>1; i--)
   {
      endRowPos = text.indexOf('\n', endRowPos) + 1 ;
      if (endRowPos == 0) return false; /* text doesn't have enough rows */
   }

   int startRowLength = text.indexOf('\n', startRowPos) - startRowPos;
   if (startRowLength < 0) /* last row */ startRowLength = text.length() - startRowPos;
   int endRowLength   = text.indexOf('\n', endRowPos)   - endRowPos;
   if (endRowLength   < 0) /* last row */ endRowLength   = text.length() - endRowPos;

   if ((int)(start.column-1) > startRowLength) return false; /* column row */
   if ((int)(end.column-1)   > endRowLength)   return false; /* column row */


   /* build temp String with highlightning to show */
   QString leftText = text.left(startRowPos+start.column-1);
   QString midText = text.mid(startRowPos+start.column-1, (endRowPos+end.column)-(startRowPos+start.column)+1);
   QString rightText = text.mid(endRowPos+end.column);

   makeHTMLconform(&leftText);
   makeHTMLconform(&midText);
   makeHTMLconform(&rightText);

   QString tmpText = htmlHeader + leftText + htmlStartTag + midText + htmlEndTag + rightText;

   sourceTextEdit->setHtml( tmpText );

   return true;
}


void SourceWindow::languageNotify()
{
    setWindowTitle( tr("Source Code") );
}

void SourceWindow::saveFile( const QString &fileName )
{
   QFile data(fileName);

   if (data.open(QFile::WriteOnly | QFile::Truncate)) {
     QTextStream out(&data);
     text = sourceTextEdit->toPlainText();
     out << text << endl;
  }
   data.close();
}


void SourceWindow::loadSourceFile( const QString &fileName )
{
    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ) )
       return;

    QTextStream textStream( &file );
    sourceTextEdit->setPlainText( textStream.readAll() );
    text = sourceTextEdit->toPlainText();
    sourceTextEdit->document()->setModified( FALSE );

    parent->statusBar()->showMessage(tr("Loaded document") + " " + fileName);
}

std::string SourceWindow::getSourceCode()
{
    text = sourceTextEdit->toPlainText();
    return text.toStdString();
}


void  SourceWindow::setReadOnly(bool ro)
{
    if (!ro) sourceTextEdit->setPlainText(text); /* remove highlightnings if writable */
    sourceTextEdit->setReadOnly(ro);
}

void  SourceWindow::scrollToRow(unsigned int row)
{
      unsigned int rows = numberOfRows();
      if ( (row<1) || (row>rows) ) return;
      QScrollBar *sb = sourceTextEdit->verticalScrollBar();

      double oneStep = ((double)1) / (rows-1);
      double factor = oneStep * (row-1);

      sb->setValue( (int) (factor * sb->maximum()) );
}

unsigned int SourceWindow::numberOfRows()
{
   unsigned int nr = 1; /* one more row than newlines */
   int pos=0;
   while (true)
   {
      pos = text.indexOf('\n', pos) + 1 ;
      if (pos == 0) return nr;
      nr++;
   }
}

QString SourceWindow::getLine(unsigned int nr)
{
   int pos0=0;
   int pos1=0;
   while (true)
   {
      nr--;
      pos1 = text.indexOf('\n', pos1) + 1 ;
      if (nr == 0) return text.mid(pos0, pos1-pos0-1);
      pos0 = pos1;
   }
   return QString();
}

void SourceWindow::makeHTMLconform(QString *qstr)
{
   qstr->replace(' ', "&nbsp;");
   qstr->replace('<', "&lt;");
   qstr->replace('>', "&gt;");
   qstr->replace('\n', "<br>");
}

void SourceWindow::reset()
{
   sourceTextEdit->clear();
   sourceTextEdit->setPlainText(text);
}

