#include "pl0lexer.h"
#include "../common.h"

extern "C"
{
   #include "pl0lexer_c.h"
   void set_lexer_input(char *yy_str);
   void set_lexer_output(FILE *fout);
   char *yyget_text(void);
   int yyget_lineno(void);
   int yyget_colno_toklen();
   void clear_lexer();
   int yyget_leng  (void);
}


void PL0Lexer::setInput(std::string input)
{
   this->input = input;
}

void PL0Lexer::lex()
{
   tokenlist.clear();
   clear_lexer();

   set_lexer_input((char *)input.c_str());

   FILE *lexer_output = fopen("lex_file", "w+");
   set_lexer_output(lexer_output);

   int t = yylex();
   while (t != 0)
   {
      Token<Pl0TokenType> token;

      token.type = (Pl0TokenType)t;
      token.position.row = yyget_lineno();
      token.position.column = yyget_colno_toklen() - yyget_leng();
      token.value = std::string(yyget_text());

      tokenlist.push_back(token);

      t = yylex();
   }

   //EOF Token
   Token<enum Pl0TokenType> eofToken;
   eofToken.type = T_EOF;
   eofToken.position.row = yyget_lineno();
   eofToken.position.column = yyget_colno_toklen();
   eofToken.value = std::string("<EOF>");

   tokenlist.push_back(eofToken);


   tokenlistIterator = tokenlist.begin();

   fclose(lexer_output);
}

void PL0Lexer::lex(std::string input)
{
   setInput(input);
   lex();
}

bool PL0Lexer::nextToken(Token<Pl0TokenType> *token)
{
  if (tokenlistIterator == tokenlist.end()) return false;
  *token = *tokenlistIterator;
  tokenlistIterator++;
  return true;
}


std::list<Token<enum Pl0TokenType> > PL0Lexer::getTokenList()
{
   return tokenlist;
}


bool PL0Lexer::existsNextToken()
{
   return (tokenlistIterator != tokenlist.end());
}
