#include "pl0token.h"

enum Pl0TokenClass getTokenClass(int tokenType)
{
   if (
   (tokenType == T_CONST) ||
   (tokenType == T_VAR) ||
   (tokenType == T_PROCEDURE) ||
   (tokenType == T_BEGIN) ||
   (tokenType == T_END) ||
   (tokenType == T_CALL) ||
   (tokenType == T_IF) ||
   (tokenType == T_THEN) ||
   (tokenType == T_WHILE) ||
   (tokenType == T_DO) ||
   (tokenType == T_QUESTIONMARK) ||
   (tokenType == T_EXCLAMATIOMARK)
   ) return CLASS_KEYWORD;

   if ( tokenType == T_IDENT) return CLASS_IDENTIFIER;

   if (
   (tokenType == T_OPENPARENTHESIS) ||
   (tokenType == T_CLOSEPARENTHESIS) ||
   (tokenType == T_COMMA) ||
   (tokenType == T_SEMICOLON) ||
   (tokenType == T_DOT) ||
   (tokenType == T_EOF)
   ) return CLASS_DELIMITER;

   if (tokenType == T_NUMBER) return CLASS_CONSTANTS;

   if (
   (tokenType == T_ODD) ||
   (tokenType == T_MULT) ||
   (tokenType == T_MINUS) ||
   (tokenType == T_PLUS) ||
   (tokenType == T_DIV) ||
   (tokenType == T_ASSIGN) ||
   (tokenType == T_EQUAL) ||
   (tokenType == T_GREATERTHEN) ||
   (tokenType == T_LESSTHEN) ||
   (tokenType == T_LESSEQUAL) ||
   (tokenType == T_GREATEREQUAL) ||
   (tokenType == T_NOTEQUAL)
   ) return CLASS_OPERATORS;

   if (
   (tokenType == T_NULL) ||
   (tokenType == T_UNKNOWN)
   ) return CLASS_UNKNOWN;

   return CLASS_UNKNOWN;
}
