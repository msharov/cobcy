CC	= gcc
CFLAGS	= -Wall -I../adtlib/include
RM	= rm -f
INSTALL	= cp 
AR	= ar cr
RANLIB	= ar s
 
SRCS	= dbf.c ndx.c
LIBOBJ	= dbf.o ndx.o
 
LIB	= libdbf.a
 
############################################################

all:	${LIB}
 
${LIB}:	${LIBOBJ}
	@echo Lumping ${LIB}
	@${AR} ${LIB} ${LIBOBJ}
	@${RANLIB} ${LIB}
 
.c.o:
	@echo "	Compiling $<"
	@${CC} ${CFLAGS} -c $<
 
clean:
	@${RM} .depend *.bak *~ ${LIBOBJ} ${LIB}
 
depend: ${SRCS}
	@${CC} ${CFLAGS} -MM ${SRCS} > .depend
 
ifeq (.depend,$(wildcard .depend))
include .depend
endif
 
