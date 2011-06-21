%{
extern int linectr;
extern int brkcount;
int skipping;
%}
OCT 0[0-7]*
DEC [1-9][0-9]*
HEX 0x[0-9a-fA-F]+
ID [a-z_A-Z][a-z_A-Z0-9]*
%%
"/*"      { skipping = 1;                     }
"*/"      { skipping = 0;                     }
"%%"      { if (! skipping)
                if (++brkcount == 2)
                { return 0; /* EOF */ }
                else
                { return DEFBRK; }            }
":"       { if (! skipping) return COLON;     }
{OCT}     { if (! skipping) return INTEGER;   }
{DEC}     { if (! skipping) return INTEGER;   }
{HEX}     { if (! skipping) return INTEGER;   }
is        { if (! skipping) return IS;        }
on        { if (! skipping) return ON;        }
"="       ;
-?intr    { if (! skipping) return INTR;      }
-?csr     { if (! skipping) return CSR;       }
-?irq     { if (! skipping) return IRQ;       }
-?i       { if (! skipping) return INIT;      }
-?o       { if (! skipping) return OPEN;      }
-?c       { if (! skipping) return CLOSE;     }
-?r       { if (! skipping) return READ;      }
-?g       { if (! skipping) return GETC;      }
-?p       { if (! skipping) return PUTC;      }
-?w       { if (! skipping) return WRITE;     }
-?s       { if (! skipping) return SEEK;      }
-?n       { if (! skipping) return CONTROL;   }
[ \t]+    ;
"\n"      { linectr++;                        }
{ID}      { if (! skipping) return IDENT;     }
.         { if (! skipping) return yytext[0]; }
