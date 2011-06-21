#
#  Makefile for the Xinu operating system.
#
#  $Id: Makefile 2132 2009-11-17 23:22:38Z brylow $

PLATFORM = wrt54gl

# DETAIL   = -DDETAIL

include platforms/${PLATFORM}/platformVars

CONFIG    = platforms/${PLATFORM}/xinu.conf

KERNARC   = kernel.a
MAIN_SRC  = ../system/main.c

TOPDIR    = ..
LIBDIR    = ${TOPDIR}/lib

# Default make target is in platformVars file.

###########################
# Source file information #
###########################

# Major system components to build into Xinu
# Each component is a directory in the source tree and should include a
# Makerules specifying what files to include
# (The loader should always be first, ensuring the image starts with it.)
SYSCOMPS  = loader system/platforms/${PLATFORM} system
COMPS     = ${SYSCOMPS} ${DEVCOMPS} ${APPCOMPS}

# Location of the conf.c file defining system devices
CONFC     = ../system/conf.c

# Libraries to build into Xinu
# All libraries should exist as archive files in the lib directory
LIBS      = libxc libdsp

# Include expected files, each should add its part to the compile source
COMP_SRC :=
include ${COMPS:%=${TOPDIR}/%/Makerules}

COMP_OBJ = $(patsubst %.S,%.o,$(filter %.S,$(COMP_SRC))) \
           $(patsubst %.c,%.o,$(filter %.c,$(COMP_SRC)))

CONF_OBJ = ${CONFC:%.c=%.o}

LIB_ARC  = ${LIBS:%=${LIBDIR}/%.a}

MAIN_OBJ = ${MAIN_SRC:%.c=%.o}

# Data is relative to the compile directory
DATA    = data

DATA_SRC :=
include ${DATA:%=%/Makerules}

DATA_OBJ = $(patsubst %,%.o,$(DATA_SRC))

###########################
# Documentation Variables #
###########################
DOCGEN_CONFIG = Doxyfile
DOCS := $(shell grep ^OUTPUT_DIRECTORY ${DOCGEN_CONFIG} | awk '{ print $$3 }')

INDENT = indent
INDENT_TYPES = -T uchar -T ushort -T uint -T ulong -T bool -T message    \
               -T irqmask -T semaphore -T mailbox -T qid_type -T tid_typ \
               -T syscall -T devcall -T shellcmd -T thread -T interrupt  \
               -T device
INDENT_ARGS = -npro -kr -bl -blf -bls -bli0 -nce -ncs -nlps -l74 -lc74 -nut
INDENT_FILES = $(shell find ${TOPDIR} -name '*.[ch]')

# Export variables for recursive make calls (such as the library)
export
################
# Make targets #
################
xinu.elf: ${COMP_OBJ} ${CONF_OBJ} ${MAIN_OBJ} ${DATA_OBJ} ${LIB_ARC}
	${LD} -o $@ ${LDFLAGS} $^

${COMP_OBJ}: ${CONF_OBJ}

${KERNARC}: ${COMP_OBJ} ${CONF_OBJ} ../system/unistd.o
	rm -f ${KERNARC}
	${AR} cr ${KERNARC} $^

objects: ${COMP_OBJ}

clean:
	rm -f *.o ${COMP_OBJ} ${CONF_OBJ} ${MAIN_OBJ} ${DATA_OBJ}
	rm -f xinu.boot xinu.bin xinu.elf
	rm -rf ${DOCS}

depend:
	$(MAKEDEP) ${INCLUDE} ${COMP_SRC} > .deps

indent:
	${INDENT} ${INDENT_ARGS} ${INDENT_TYPES} ${INDENT_FILES}
	rm -f ${INDENT_FILES:%=%~}

${MKIMAGE}:
	@echo "ERROR: The U-Boot utilities are not part of Xinu."
	@echo "ERROR: Please acquire the mkimage utility separately."
	exit 1

docs:
	${DOCGEN} ${DOCGEN_CONFIG}

debug: 
	$(MAKE) DEBUG="-DDEBUG ${BUGFLAG}"

libclean: ${LIBS:%=%-clean}

confclean:
	$(MAKE) -C config/ clean

depclean:
	rm -f .deps
	touch .deps

realclean: clean depclean libclean confclean
	rm -f ../include/conf.h
	rm -f ../system/conf.c
	rm -f vn version
	rm -f Makefile.bak

###################
# Special targets #
###################
${MAIN_OBJ}: ${CONF_OBJ} ${MAIN_SRC}

${COMPS}:
	$(MAKE) COMPS="$@" objects

${DATA_OBJ}:
	$(OBJCOPY) $(OCFLAGS) ${@:%.o=%} $@

${CONFC}: config/config
	./config/config ${CONFIG}

config/config:
	$(MAKE) -C config/

${LIBS}: ${COMP_OBJ}
	$(MAKE) -C ${LIBDIR}/$@

${LIB_ARC}: ${COMP_OBJ}
	$(MAKE) -C ${@:%.a=%} install

${LIBS:%=%-clean}:
	$(MAKE) -C ${LIBDIR}/${@:%-clean=%} clean

include .deps
