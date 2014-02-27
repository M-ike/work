#ifndef ILANGUAGEWIDGET_H
#define ILANGUAGEWIDGET_H

#include <string>

class ILanguageWidget
{

 public:
     virtual void languageNotify() = 0;

 protected:
    ILanguageWidget() {}
    virtual ~ILanguageWidget() {}

};


#endif
