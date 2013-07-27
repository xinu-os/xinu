%{
extern char *doing;
extern int   currname;
extern int   currtname;
extern int   currdname;
%}

/* Token semantic values */
%union {
	int intval;
	char *strval;
}

/* Types of each grammar symbol whose semantic value is used */
%token <strval> IDENT
%token <intval> INTEGER
%type <intval> id
%type <intval> tname
%type <intval> dname
%type <intval> optional_on

/* Additional token types */
%token DEFBRK COLON OCTAL CSR IRQ INTR INIT OPEN CLOSE READ
       WRITE SEEK CONTROL IS ON GETC PUTC


%%
configuration:  devtypes devdescriptors
;

devtypes:       ftypes DEFBRK { doing = "device definitions"; }
;

ftypes:         /* nothing */
              | ftypes ftype
;

ftype:          tname dev_list
;

dev_list:       devheader attr_list
              | dev_list devheader attr_list
;

devheader:      ON id { mktype($2); }
;

tname:          id COLON {$$ = currtname = cktname($1); }
;

id:             IDENT { $$ = currname = lookup($1); }
;

attr_list:      /* nothing */
              | attr_list attr
;

attr:           CSR INTEGER    { newattr(CSR, $2);     }
              | IRQ INTEGER    { newattr(IRQ, $2);     }
              | INTR id        { newattr(INTR, $2);    }
              | OPEN id        { newattr(OPEN, $2);    }
              | CLOSE id       { newattr(CLOSE, $2);   }
              | INIT id        { newattr(INIT, $2);    }
              | GETC id        { newattr(GETC, $2);    }
              | PUTC id        { newattr(PUTC, $2);    }
              | READ id        { newattr(READ, $2);    }
              | WRITE id       { newattr(WRITE, $2);   }
              | SEEK id        { newattr(SEEK, $2);    }
              | CONTROL id     { newattr(CONTROL, $2); }
;

devdescriptors: /* nothing */
              | devdescriptors descriptor
;

descriptor:     fspec attr_list
;

fspec:          dname IS id optional_on { mkdev($1, $3, $4); }
;

dname:          id { $$ = currdname = ckdname($1); }
;

optional_on:    /* nothing */  { $$ = 0;  }
              | ON id          { $$ = $2; }
;

%%
