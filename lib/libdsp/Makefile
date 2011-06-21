#
#  Make the Xinu version of the C run-time support library
#

INCLUDE	= -I../../include -I../../system/platforms/${PLATFORM}
LIBDIR	= ..

LIBNAME = libdsp

# Make sure the library is optimized for size, if we want any optimization
CFLAGS := ${CFLAGS:-O%=-Os}

# Update the include flag
CFLAGS := ${CFLAGS:-I%=${INCLUDE}}

.s.o:
		${AS} -o $@ ${AFLAGS} $<

.c.o:
		${CC} ${CFLAGS} $<

CFILES	= linear2ulaw.c ulaw2linear.c

OFILE2 = ${CFILES:%.c=%.o}
OFILES = ${OFILE2:%.s=%.o}

all:		${LIBNAME}.a

${LIBNAME}.a:	${OFILES}
		rm -f ${LIBNAME}.a
		${AR} cr ${LIBNAME}.a ${OFILES}

install:	${LIBNAME}.a
		cp ${LIBNAME}.a ${LIBDIR}/${LIBNAME}.a

clean:
		rm -f ${OFILES} ${LIBNAME}.a ${LIBDIR}/${LIBNAME}.a
