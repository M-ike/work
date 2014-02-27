%{
#include "pl0token.h"

int col=1;

%}

%%
CONST                { col += yyleng; return T_CONST;           }
VAR                  { col += yyleng;  return T_VAR;            }
PROCEDURE            { col += yyleng;  return T_PROCEDURE;      }
BEGIN                { col += yyleng;  return T_BEGIN;          }
END                  { col += yyleng;  return T_END;            }
CALL                 { col += yyleng;  return T_CALL;           }
IF                   { col += yyleng;  return T_IF;             }
THEN                 { col += yyleng;  return T_THEN;           }
WHILE                { col += yyleng;  return T_WHILE;          }
DO                   { col += yyleng;  return T_DO;             }
ODD                  { col += yyleng;  return T_ODD;            }
\?                   { col += yyleng;  return T_QUESTIONMARK;   }
\!                   { col += yyleng;  return T_EXCLAMATIOMARK; }
\*                   { col += yyleng;  return T_MULT;           }
\-                   { col += yyleng;  return T_MINUS;          }
\+                   { col += yyleng;  return T_PLUS;           }
\/                   { col += yyleng;  return T_DIV;            }
:=                   { col += yyleng;  return T_ASSIGN;         }
\=                   { col += yyleng;  return T_EQUAL;          }
\>                   { col += yyleng;  return T_GREATERTHEN;    }
\<                   { col += yyleng;  return T_LESSTHEN;       }
\<=                  { col += yyleng;  return T_LESSEQUAL;      }
\>=                  { col += yyleng;  return T_GREATEREQUAL;   }
\#                   { col += yyleng;  return T_NOTEQUAL;       }
\(                   { col += yyleng;  return T_OPENPARENTHESIS;      }
\)                   { col += yyleng;  return T_CLOSEPARENTHESIS;     }
\,                   { col += yyleng;  return T_COMMA;          }
\;                   { col += yyleng;  return T_SEMICOLON;      }
\.                   { col += yyleng;  return T_DOT;            }
[A-Za-z][A-Za-z0-9]* { col += yyleng;  return T_IDENT;          }
([1-9][0-9]*)|[0]    { col += yyleng;  return T_NUMBER;         }
\n                   { col = 1;       /* skip whitespace */     }
[ \t\r]              { col += yyleng; /* skip whitespace */     }
.                    { col += yyleng;  return T_UNKNOWN;        }
%%


void set_lexer_input(char *yy_str)
{
    yy_scan_string(yy_str);
}

void set_lexer_output(FILE *fout)
{
    yyset_out(fout);
}

int yyget_colno_toklen()
{
    return col;
}

void clear_lexer()
{
   col=1;
   yyset_lineno(1);
}

