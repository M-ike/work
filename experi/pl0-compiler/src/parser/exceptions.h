#ifndef PARSEREXCEPTIONS_H
#define PARSEREXCEPTIONS_H

#include <exception>
#include "ast.h"

class ParseException: public std::exception
{
   protected:
      ParseException() {}
};

class UndefinedSymbolException: public ParseException
{
  public:
     UndefinedSymbolException(AST::Ident *ident): ident(ident) {}
     AST::Ident *ident;
};


class UndefinedVariableException: public UndefinedSymbolException
{
  public:
     UndefinedVariableException(AST::Ident *ident): UndefinedSymbolException(ident) {}
};


class UndefinedProcedureException: public UndefinedSymbolException
{
  public:
     UndefinedProcedureException(AST::Ident *ident, int nrParameters): UndefinedSymbolException(ident), nrParameters(nrParameters) {}
     int nrParameters;
};


class UnexpectedSymbolException: public ParseException
{
  public:
     UnexpectedSymbolException(Token<Pl0TokenType> *token): token(token) {}
     Token<Pl0TokenType> *token;
};

class MultipleDefinitionException: public ParseException
{
  public:
     MultipleDefinitionException(AST::Ident *ident, int nrParameters): ident(ident), nrParameters(nrParameters) {}
     MultipleDefinitionException(AST::Ident *ident): ident(ident), nrParameters(-1) {}
     AST::Ident *ident;
     int nrParameters;
};


class StopException: public ParseException
{
  public:
     StopException() {}
};

#endif
