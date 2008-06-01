CC	= gcc
CFLAGS	= -Wall -g -pipe -I../adtlib -L../dbf -I../dbf
RM	= rm -f
INSTALL	= install -s -m 755 
AR	= ar
RANLIB	= ranlib
 
SRCS	= display.c main.c
 
OBJS	= display.o main.o
LIBS	= -lncurses -ldbf
 
EXE	= dbv
 
############################################################

all:	${LIB} ${EXE}
 
${EXE}: ${OBJS} ${LIB}
	@echo Linking ${EXE}
	@${CC} ${CFLAGS} -o ${EXE} ${OBJS} ${LIB} ${LIBS}

${LIB}:	${LIBOBJ}
	@echo Lumping ${LIB}
	@${AR} ruv ${LIB} ${LIBOBJ}
	@${RANLIB} ${LIB}
 
.c.o:
	@echo "	Compiling $<"
	@${CC} ${CFLAGS} -c $<
 
clean:
	@${RM} *.bak *~ ${OBJS} ${EXE}
 
depend: ${SRCS}
	@${CC} ${CFLAGS} -MM ${SRCS} > .depend
 
ifeq (.depend,$(wildcard .depend))
include .depend
endif
 
