CC	= gcc
CFLAGS	= -Wall -O6 -pipe -I../adtlib/include
RM	= rm -f
INSTALL	= install -s -m 755 
AR	= ar
RANLIB	= ranlib
 
SRCS	= dbf.c ndx.c
LIBOBJ	= dbf.o ndx.o
 
LIB	= libdbf.a
 
############################################################

all:	${LIB}
 
${LIB}:	${LIBOBJ}
	@echo Lumping ${LIB}
	@${AR} ruv ${LIB} ${LIBOBJ}
	@${RANLIB} ${LIB}
 
.c.o:
	@echo "	Compiling $<"
	@${CC} ${CFLAGS} -c $<
 
clean:
	@${RM} *.bak *~ ${LIBOBJ} ${LIB}
 
depend: ${SRCS}
	@${CC} ${CFLAGS} -MM ${SRCS} > .depend
 
ifeq (.depend,$(wildcard .depend))
include .depend
endif
 
