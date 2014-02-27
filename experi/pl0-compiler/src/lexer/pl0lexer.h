#ifndef PL0LEXER_H
#define PL0LEXER_H

#include <string>
#include <list>
#include "token.h"
#include "pl0token.h"

class PL0Lexer
{
   public:
      PL0Lexer() {}
      void setInput(std::string input);
      void lex();
      void lex(std::string input);
      bool nextToken(Token<Pl0TokenType> *token);
      std::list<Token<enum Pl0TokenType> > getTokenList();
      bool existsNextToken();

   private:
      std::string input;
      std::list<Token<enum Pl0TokenType> > tokenlist;
      std::list<Token<enum Pl0TokenType> >::iterator tokenlistIterator;
};

#endif
