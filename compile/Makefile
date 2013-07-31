#
#  Makefile for the Xinu operating system.
#
#  $Id: Makefile 2132 2009-11-17 23:22:38Z brylow $

PLATFORM = none

# DETAIL   = -DDETAIL

include platforms/${PLATFORM}/platformVars

CONFIG    = platforms/${PLATFORM}/xinu.conf

KERNARC   = kernel.a
MAIN_SRC  = ../system/main.c

TOPDIR    = ..
LIBDIR    = ${TOPDIR}/lib

# Default make target is in platformVars file.
$(VERBOSE).SILENT:

###########################
# Source file information #
###########################

# Major system components to build into Xinu
# Each component is a directory in the source tree and should include a
# Makerules specifying what files to include
# (The loader should always be first, ensuring the image starts with it.)
SYSCOMPS  = loader/platforms/${PLATFORM} system/platforms/${PLATFORM} system
COMPS     = ${SYSCOMPS} ${DEVCOMPS} ${APPCOMPS}

# Location of the conf.c file defining system devices
CONFC     = ../system/conf.c

# Libraries to build into Xinu
# All libraries should exist as archive files in the lib directory
LIBS      = libxc libdsp

# Include expected files, each should add its part to the compile source
COMP_SRC :=
ifneq (${PLATFORM},none)
include ${COMPS:%=${TOPDIR}/%/Makerules}
endif

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
xinu.elf: ${COMP_OBJ} ${CONF_OBJ} ${MAIN_OBJ} ${DATA_OBJ} ${LIB_ARC} ${USRTHRS_OBJ}
	@echo "    Linking" $@
	${LD} -o $@ ${LDFLAGS} $^

${COMP_OBJ}: ${CONF_OBJ}

${KERNARC}: ${COMP_OBJ} ${CONF_OBJ} ../system/unistd.o
	rm -f ${KERNARC}
	${AR} cr ${KERNARC} $^

objects: ${COMP_OBJ}

%.o: %.c
	@echo "    Compiling" $@
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.S
	@echo "    Assembling" $@
	$(CC) $(CFLAGS) -o $@ $<

clean:
	@echo "    Cleaning all objects"
	rm -f *.o ${COMP_OBJ} ${CONF_OBJ} ${MAIN_OBJ} ${DATA_OBJ} ${USRTHRS_OBJ}
	rm -f xinu.boot xinu.bin xinu.elf

depend:
	@echo "    Building dependencies"
	$(MAKEDEP) ${INCLUDE} ${COMP_SRC} > .deps

indent:
	@echo "    Indenting sources"
	${INDENT} ${INDENT_ARGS} ${INDENT_TYPES} ${INDENT_FILES}
	rm -f ${INDENT_FILES:%=%~}

${MKIMAGE}:
	@echo "ERROR: The U-Boot utilities are not part of Xinu."
	@echo "ERROR: Please acquire the mkimage utility separately."
	exit 1

docs:
	@echo "    Building documentation"
	${DOCGEN} ${DOCGEN_CONFIG}

debug: 
	@echo "    Building debug mode"
	$(MAKE) DEBUG="-DDEBUG ${BUGFLAG}"

libclean: ${LIBS:%=%-clean}

docsclean:
	@echo "    Cleaning documentation"
	rm -rf ${DOCS}

confclean:
	@echo "    Cleaning configuration"
	$(MAKE) -C config/ clean

depclean:
	@echo "    Cleaning dependencies"
	rm -f .deps
	touch .deps

realclean: clean docsclean depclean libclean confclean ${PLATCLEAN}
	@echo "    Cleaning EVERYTHING"
	rm -f ../include/conf.h
	rm -f ../system/conf.c
	rm -f vn version
	rm -f Makefile.bak

###################
# Special targets #
###################
${MAIN_OBJ}: ${CONF_OBJ} ${MAIN_SRC}

${COMPS}:
	@echo "    Building" $@
	$(MAKE) COMPS="$@" objects

${DATA_OBJ}:
	@echo "    Object Copy" $@
	$(OBJCOPY) $(OCFLAGS) ${@:%.o=%} $@

${CONFC}: config/config
	@echo "    Configuring" $@
	./config/config ${CONFIG}

config/config:
	@echo "    Building" $@
	$(MAKE) -C config/

${LIBS}: ${CONF_OBJ}
	@echo "    Building" $@
	$(MAKE) -C ${LIBDIR}/$@

${LIB_ARC}: ${CONF_OBJ}
	@echo "    Installing" $@
	$(MAKE) -C ${@:%.a=%} install

${LIBS:%=%-clean}:
	@echo "    Cleaning" $@
	$(MAKE) -C ${LIBDIR}/${@:%-clean=%} clean

include .deps
