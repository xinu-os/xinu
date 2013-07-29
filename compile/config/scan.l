%{

/* Get the token type and semantic value definitions generated from the parser
 * source.  */
#include "y.tab.h"
#include <stdlib.h>

extern int linectr;
extern int brkcount;

extern void yyerror(const char *s);
extern const char *literal_string;

%}
OCT 0[0-7]*
DEC [1-9][0-9]*
HEX 0x[0-9a-fA-F]+
ID [a-z_A-Z][a-z_A-Z0-9]*

/* Specify the 'noyywrap' option to avoid having to unnecessarily link with
 * -lfl. */
%option noyywrap

/* Exclusive start condition (within a comment--- most parsing rules don't
 * apply) */
%x COMMENT

/* Exclusive start condition (within the literal section at the end of the
 * configuration file --- copied directly to the output conf.h) */
%x LITERAL_SECTION

%x END

%%
<LITERAL_SECTION>(.|\n)* { literal_string = yytext; return 0; }
<COMMENT>"*/"  { BEGIN(INITIAL);    }
<COMMENT>\n    { linectr++;         }
<COMMENT>.     { ;                  }

"/*"           { BEGIN(COMMENT);    }

"%%"      { if (++brkcount == 2) {
                BEGIN(LITERAL_SECTION);
            } else {
                return DEFBRK;
            }                       }
":"       { return COLON;     }
{OCT}     { yylval.intval = strtol(yytext, NULL, 8); return INTEGER;   }
{DEC}     { yylval.intval = strtol(yytext, NULL, 10); return INTEGER;   }
{HEX}     { yylval.intval = strtol(yytext, NULL, 16); return INTEGER;   }
is        { return IS;        }
on        { return ON;        }
"="       { ;                 }
-?intr    { return INTR;      }
-?csr     { return CSR;       }
-?irq     { return IRQ;       }
-?i       { return INIT;      }
-?o       { return OPEN;      }
-?c       { return CLOSE;     }
-?r       { return READ;      }
-?g       { return GETC;      }
-?p       { return PUTC;      }
-?w       { return WRITE;     }
-?s       { return SEEK;      }
-?n       { return CONTROL;   }
[ \t]+    { ;                 }
\n        { linectr++;        }
{ID}      { yylval.strval = yytext; return IDENT; }
.         { yyerror("syntax error");  }
