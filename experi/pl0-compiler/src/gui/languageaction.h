#ifndef LANGUAGEACTION_H
#define LANGUAGEACTION_H

#include <QAction>
#include <string>

class LanguageAction: public QAction
{

 public:
     LanguageAction(const QIcon &icon, const QString &text, std::string languageFile, QObject *parent)
        : QAction(icon, text, parent),
          languageFile(languageFile)
     {}

     std::string getLanguageFile()
     {
         return languageFile;
     }

 private:
     std::string languageFile;
};


#endif
