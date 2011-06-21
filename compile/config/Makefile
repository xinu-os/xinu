#
# Make the Xinu configuration program
#

CC     = gcc
LEX    = flex
YACC   = bison -y    # Flag enables yacc-compatible filenames
CFLAGS = 
LFLAGS = -lfl

CONFIG = config

all: ${CONFIG}

${CONFIG}: lex.yy.c y.tab.c
	$(CC) ${CFLAGS} -o $@ y.tab.c ${LFLAGS}

lex.yy.c: config.l
	$(LEX) config.l

y.tab.c: config.y
	$(YACC) config.y

clean:
	rm -f ${CONFIG} lex.yy.c y.tab.c
