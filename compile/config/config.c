#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "y.tab.h"

#define NIL (struct dev_ent *)0x00

extern int yylex(void);

#define CONFIGC   "../system/conf.c"  /* name of .c output     */
#define CONFIGH   "../include/conf.h" /* name of .h output     */
#define CONFHREF  "<conf.h>"          /* how conf.h referenced */
#define CONFIGIN  "xinu.config"       /* name of input file    */
#define CONFMAXNM 20                  /* max length of strings */

#ifdef __MACH__
size_t strnlen(const char *s, size_t n)
{
    const char *p = (const char *)memchr(s, 0, n);
    return(p ? p-s : n);
}
#endif

FILE *confc;
FILE *confh;

char *dbstr;
int   ndevs = 0;
int   currname = -1;
int   currtname = -1;
int   currdname = -1;
int   brkcount = 0;

const char *literal_string = "";

struct sym_ent { /* symbol table */
	char *name;
	int   occurs;
} symtab[250];

int nsym = 0;
int linectr = 1;

char *doing = "device type declaration";
char *s;

struct dev_ent
{
	char *name;               /* device name (not used in types) */
	char *tname;              /* type name                       */
	int   tnum;               /* symbol table index of type      */
	char *device;             /* device name                     */
	int   csr;                /* Control Status Register addr    */
	int   irq;                /* interrupt request               */
	char  intr[CONFMAXNM];    /* interrupt routine               */
	char  init[CONFMAXNM];    /* init routine name               */
	char  open[CONFMAXNM];    /* open routine name               */
	char  close[CONFMAXNM];   /* close routine name              */
	char  read[CONFMAXNM];    /* read routine name               */
	char  write[CONFMAXNM];   /* write routine name              */
	char  control[CONFMAXNM]; /* control routine name            */
	char  seek[CONFMAXNM];    /* seek routine name               */
	char  getc[CONFMAXNM];    /* getc routine name               */
	char  putc[CONFMAXNM];    /* putc routine name               */
	int   minor;              /* device number 0,1,...           */
	struct dev_ent *next;     /* next node on the list           */
};

struct dev_ent *ftypes = NIL; /* linked list of device types  */
struct dev_ent *devs = NIL;   /* linked list of device decls. */
struct dev_ent *lastdv = NIL;
struct dev_ent *currtype = NIL;

char *devstab[] =
{
	"/* Device table entry */",
	"typedef struct dentry",
	"{",
	"\tint     num;",
	"\tint     minor;",
	"\tchar    *name;",
	"\tdevcall (*init)(struct dentry *);",
	"\tdevcall (*open)(struct dentry *, ...);",
	"\tdevcall (*close)(struct dentry *);",
	"\tdevcall (*read)(struct dentry *, void *, uint);",
	"\tdevcall (*write)(struct dentry *, const void *, uint);",
	"\tdevcall (*seek)(struct dentry *, long);",
	"\tdevcall (*getc)(struct dentry *);",
	"\tdevcall (*putc)(struct dentry *, char);",
	"\tdevcall (*control)(struct dentry *, int, long, long);",
	"\tvoid    *csr;",
	"\tvoid    (*intr)(void);",
	"\tuchar   irq;",
	"} device;\n",
	"extern const device devtab[]; /* one entry per device */",
	NULL
};

/* Prototypes */
void yyerror(const char *s);

int  lookup(const char *str);
void newattr(int tok, int val);
int  cktname(int symid);
void mktype(int deviceid);
void initattr(struct dev_ent *fstr, int tnum, int deviceid);
void mkdev(int nameid, int typid, int deviceid);
int  ckdname(int devid);

int main(int argc, char **argv)
{
	int n, i, fcount;
	struct dev_ent *s;
	int   verbose = 0;
	char *p;

	if ( argc > 1 && (strncmp("-v", argv[1], 2) == 0) )
	{
		argc--;
		argv++;
		verbose++;
	}

	if ( argc > 2 )
	{
		fprintf(stderr, "use: config [-v] [file]\n");
		exit(1);
	}

	if (verbose) { printf("Opening input file...\n"); }

	if (argc == 2)
	{
		if (freopen(argv[1], "r", stdin) == NULL)
		{
			fprintf(stderr, "Can't open %s\n", argv[1]);
			exit(1);
		}
	}
	else /* try to open Configuration file */
	{
		if (freopen(CONFIGIN, "r", stdin) == NULL)
		{
			fprintf(stderr, "Can't open %s\n", CONFIGIN);
			exit(1);
		}
	}

	/* Parse the Configuration file */
	if (verbose) { printf("Parsing configuration specs...\n"); }

	if ( (n = yyparse()) != 0 ) { exit(n); }

	/* write config.h and config.c */
	if (verbose) { printf("Opening output files...\n"); }

	if ( (confc = fopen(CONFIGC,"w") ) == NULL)
	{
		fprintf(stderr, "Can't write on %s\n", CONFIGC);
		exit(1);
	}

	if ( (confh = fopen(CONFIGH,"w") ) == NULL)
	{
		fprintf(stderr, "Can't write on %s\n", CONFIGH);
		exit(1);
	}


	/** produce conf.h **/

	fprintf(confh, "/* conf.h (GENERATED FILE; DO NOT EDIT) */\n\n");
	fprintf(confh, "#ifndef _CONF_H_\n");
	fprintf(confh, "#define _CONF_H_\n\n");

	fprintf(confh, "#include <stddef.h>\n\n");

	if (verbose) { printf("Writing output...\n"); }

	fprintf(confh, "/* Device table declarations */\n\n");

	for (i = 0; (p = devstab[i]) != NULL; i++)
	{
		fprintf(confh, "%s\n", p);
	}

	fprintf(confh, "\n");

	/* write device declarations and definitions; count type refs. */
	fprintf(confh, "/* Device name definitions */\n\n");
	for (i = 0, s = devs; s != NIL; i++, s = s->next)
	{
		fprintf(confh, "#define %-12s%d       /* type %-8s */\n",
		        s->name, i, s->tname);
		s->minor = symtab[s->tnum].occurs++;
	}

	fprintf(confh, "\n");

	/* write count of device types */
	fprintf(confh, "/* Control block sizes */\n\n");
	for (i = 0; i < nsym; i++)
	{
		if (symtab[i].occurs > 0)
		{
			int j;
			char name[CONFMAXNM];
			strncpy(name, symtab[i].name, CONFMAXNM);

			for (j = 0; j < CONFMAXNM && name[j] != '\0'; j++)
			{ name[j] = toupper( name[j] ); }
			name[19] = '\0';

			fprintf(confh, "#define N%s %d\n",
			        name, symtab[i].occurs);
		}
	}

	fprintf(confh, "\n");

	fprintf(confh, "#define DEVMAXNAME %d\n", CONFMAXNM);

	if (ndevs > 0) { fprintf(confh, "#define NDEVS %d\n", ndevs); }

	/* Copy definitions to output */
	if (brkcount == 2 && verbose)
	{ printf("Copying definitions to %s...\n", CONFIGH); }

	if (brkcount == 2 )
	{
		fputs(literal_string, confh);
	}

	fprintf(confh, "\n#endif /* _CONF_H_ */\n");

	fclose(confh);


	/** produce conf.c **/

	fprintf(confc, "/* conf.c (GENERATED FILE; DO NOT EDIT) */\n\n");
	fprintf(confc, "#include %s\n", CONFHREF);
	fprintf(confc, "#include <device.h>\n\n");

	for (i = 0; i < nsym; i++)
	{
		if (symtab[i].occurs > 0)
		{ fprintf(confc, "#include <%s.h>\n", symtab[i].name); }
	}
	fprintf(confc, "\n");

	fprintf(confc, "extern devcall ioerr(void);\n");
	fprintf(confc, "extern devcall ionull(void);\n\n");

	/* produce devtab (giant I/O switch table) */
	fprintf(confc, "/* device independent I/O switch */\n\n");
	if (ndevs > 0)
	{
		fprintf(confc, "const device devtab[NDEVS] =\n{\n");
		fprintf(confc, "%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n\n",
			"/**",
			" * Format of entries is:",
			" * dev-number, minor-number, dev-name,",
			" * init, open, close,",
			" * read, write, seek,",
			" * getc, putc, control,",
			" * dev-csr-address, intr-handler, irq",
			" */");
	}

	for (fcount = 0, s = devs; s != NIL; fcount++, s = s->next)
	{
		fprintf(confc, "/* %s is %s */\n", s->name, s->tname);
		fprintf(confc, "\t{ %d, %d, \"%s\",\n", fcount, s->minor, s->name);
		fprintf(confc, "\t  (void *)%s, (void *)%s, (void *)%s,\n",
		        s->init, s->open, s->close);
		fprintf(confc, "\t  (void *)%s, (void *)%s, (void *)%s,\n",
		        s->read, s->write, s->seek);
		fprintf(confc, "\t  (void *)%s, (void *)%s, (void *)%s,\n",
		        s->getc, s->putc, s->control);
		fprintf(confc, "\t  (void *)0x%x, (void *)%s, %d }",
		        s->csr, s->intr, s->irq);

		if ( s->next != NIL ) { fprintf(confc, ",\n\n"); }
		else                  { fprintf(confc, "\n};");  }
	}

	/* guarantee conf.c written later than conf.c for make */
	fprintf(confc, "\n");
	fclose(confc);

	/* finish up and write report for user if requested */
	if (verbose)
	{
		printf("Configuration complete. Number of devs=%d:\n", ndevs);
		for (s = devs; s != NIL ; s = s->next)
		{
			printf("Device %s (on %s)\n", s->name, s->device);
			printf("    csr=0x%04x, irq=0x%04x, minor=%d\n",
			       s->csr, s->irq, s->minor);
		}
	}
	return 0;
}

void yyerror(const char *s)
{
	fprintf(stderr, "Syntax error in %s on line %d\n", doing, linectr);
}

/* lookup  --  lookup a name in the symbol table; return position */
int lookup(const char *str)
{
	int   i;
	char *s;
	int len = strlen(str);

	if (len >= CONFMAXNM)
	{
		len = CONFMAXNM-1;
		fprintf(stderr, "warning: name %s truncated\n", str);
	}

	s = (char *)malloc(len + 1);
	strncpy(s, str, len);
	s[len] = '\0';

	for (i = 0; i < nsym; i++)
	{
		if ( strncmp(s, symtab[i].name, CONFMAXNM) == 0 )
		{ return(i); }
	}

	symtab[nsym].name = s;
	symtab[nsym].occurs = 0;

	return nsym++;
}

/* newattr -- add a new attribute spec to current type/device description */
/* tok: token type (attribute type) */
/* val: symbol number of value      */
void newattr(int tok, int val)
{
	struct dev_ent *s;
	char *c;

	/* doing types */
	if (devs == NIL) { s = currtype; }
	else             { s = lastdv; }

	if (val>=0 && val<nsym)
	{
		c = symtab[val].name;
		if (strnlen(c, CONFMAXNM) == CONFMAXNM )
		{
			fprintf(stderr, "Internal overflow\n");
			exit(1);
		}
	}
	else
	{ c = NULL; }

	switch (tok)
	{
	case CSR:     s->csr = val;                      break;
	case IRQ:     s->irq = val;                      break;
	case INTR:    strncpy(s->intr,    c, CONFMAXNM); break;
	case READ:    strncpy(s->read,    c, CONFMAXNM); break;
	case WRITE:   strncpy(s->write,   c, CONFMAXNM); break;
	case GETC:    strncpy(s->getc,    c, CONFMAXNM); break;
	case PUTC:    strncpy(s->putc,    c, CONFMAXNM); break;
	case OPEN:    strncpy(s->open,    c, CONFMAXNM); break;
	case CLOSE:   strncpy(s->close,   c, CONFMAXNM); break;
	case INIT:    strncpy(s->init,    c, CONFMAXNM); break;
	case SEEK:    strncpy(s->seek,    c, CONFMAXNM); break;
	case CONTROL: strncpy(s->control, c, CONFMAXNM); break;
	default:      fprintf(stderr, "Internal error 1\n");
	}
}

/* cktname  --  check type name for duplicates */
int cktname(int symid)
{
	struct dev_ent *s;
	char *name;

	name = symtab[symid].name;
	for (s = ftypes; s != NIL; s = s->next)
	{
		if (s->tname == name)
		{
			fprintf(stderr, "Duplicate type name %s on line %d\n",
			        name, linectr);
			exit(1);
		}
	}

	return symid;
}

/* mktype  --  make a node in the type list and initialize to defaults */
void mktype(int deviceid)
{
	struct dev_ent *s, *p;
	char *tn, *dn;

	p = NIL;
	tn = symtab[currtname].name;
	dn = symtab[deviceid].name;
	for (s = ftypes; s != NIL ; s = s->next)
	{
		if (s->tname == tn && s->device==dn)
		{
			fprintf(stderr, "Duplicate device %s for type %s on line %d\n",
			        dn, tn, linectr);
			exit(1);
		}
		p = s;
	}

	currtype = s = (struct dev_ent *)malloc(sizeof(struct dev_ent));
	if (ftypes != NIL) { p->next = s; }
	else               { ftypes = s; }

	initattr(s, currtname, deviceid);
}

/* initialize attributes in a type declaration node to typename... */
void initattr(struct dev_ent *fstr, int tnum, int deviceid)
{
	fstr->name = NULL;
	fstr->tname = symtab[tnum].name;
	fstr->tnum = tnum;
	fstr->device = symtab[deviceid].name;
	fstr->csr = 0;
	fstr->irq = 0;
	strncpy(fstr->intr,    "ioerr", 5);
	strncpy(fstr->init,    "ioerr", 5);
	strncpy(fstr->open,    "ioerr", 5);
	strncpy(fstr->close,   "ioerr", 5);
	strncpy(fstr->read,    "ioerr", 5);
	strncpy(fstr->write,   "ioerr", 5);
	strncpy(fstr->control, "ioerr", 5);
	strncpy(fstr->seek,    "ioerr", 5);
	strncpy(fstr->getc,    "ioerr", 5);
	strncpy(fstr->putc,    "ioerr", 5);
	fstr->minor = 0;
}

/* mkdev  --  make a node on the device list */
void mkdev(int nameid, int typid, int deviceid)
{
	struct dev_ent *s;
	char *devn, *tn, *dn;
	int   found;

	s = (struct dev_ent *)malloc(sizeof(struct dev_ent));
	s->next = NIL;
	if (devs == NIL)
	{
		devs = s;
		lastdv = s;
	}
	else
	{
		lastdv->next = s;
		lastdv = s;
	}

	ndevs++;
	tn = symtab[typid].name;
	devn = symtab[nameid].name;

	if (deviceid >= 0) { dn = symtab[deviceid].name; }
	else               { dn = NULL; }

	found = 0;
	for (s = ftypes; s != NULL ; s = s->next)
	{
		if (s->tname == tn && (dn == NULL || s->device == dn))
		{
			memcpy(lastdv, s, sizeof(struct dev_ent));
			found = 1;
			break;
		}
	}

	if (found==0)
	{
		fprintf(stderr,
		        "Bad type or device name in declaration of %s on line %d\n",
		        devn, linectr);
		exit(1);
	}

	lastdv->next = NIL;
	lastdv->name = devn;
}


/* chdname  -- check for duplicate device name */
int ckdname(int devid)
{
	struct dev_ent *s;
	char *name;

	name = symtab[devid].name;

	for (s = devs; s != NIL; s = s->next)
	{
		if (s->name == name)
		{
			fprintf(stderr, "Duplicate device name %s on line %d\n",
			        name, linectr);
			exit(1);
		}
	}

	return devid;
}

