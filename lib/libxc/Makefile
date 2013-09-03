#
# Make Xinu's C run-time support library
#

# Name of this library
LIBNAME := libxc

# C files to compile (.c)
CFILES  := abs.c      \
           atoi.c     \
           atol.c     \
           bzero.c    \
           ctype_.c   \
           doprnt.c   \
           doscan.c   \
           fgetc.c    \
           fgets.c    \
           fprintf.c  \
           fputc.c    \
           fputs.c    \
           fscanf.c   \
           labs.c     \
           memchr.c   \
           memcmp.c   \
           memcpy.c   \
           memset.c   \
           printf.c   \
           qsort.c    \
           rand.c     \
           sprintf.c  \
           sscanf.c   \
           strchr.c   \
           strcmp.c   \
           strcpy.c   \
           strlcpy.c  \
           strlen.c   \
           strncat.c  \
           strncmp.c  \
           strncpy.c  \
           strnlen.c  \
           strrchr.c  \
           strstr.c

# Add deprecated malloc and free if needed
CFILES  += malloc.c free.c

# Assembly files to compile (.S)
SFILES  :=

# Directory in which to place the output library, and
# location of common Makerules
LIBDIR  := ..

include $(LIBDIR)/Makerules
