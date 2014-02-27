#ifndef LANGUAGEMANAGER_H
#define LANGUAGEMANAGER_H

#include <string>
#include <list>
#include "ilanguagewidget.h"

class LanguageManager
{

 public:
   static LanguageManager* getInstance();
   bool registerWidget(ILanguageWidget *widget);
   //TODO unregister widget
   void changeLanguage();

 private:
   LanguageManager() {}
   LanguageManager(const LanguageManager& cc) {}
   static LanguageManager* instance;

   std::list<ILanguageWidget*> observers;

};


#endif
