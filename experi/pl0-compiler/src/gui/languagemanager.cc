#include "languagemanager.h"

LanguageManager* LanguageManager::instance = 0;

LanguageManager* LanguageManager::getInstance()
{
    if( !instance )
      instance = new LanguageManager();
    return instance;
}

bool LanguageManager::registerWidget(ILanguageWidget *widget)
{
   if (widget == NULL) return false;
   observers.push_back(widget);
   return true;
}

void LanguageManager::changeLanguage()
{
   std::list<ILanguageWidget*>::iterator observer_iterator;
   for( observer_iterator = observers.begin(); observer_iterator != observers.end(); observer_iterator++ )
       (*observer_iterator)->languageNotify();
}
