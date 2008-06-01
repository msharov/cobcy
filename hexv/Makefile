CC	= gcc
CFLAGS	= -w -O6 -m486 -pipe -I${HOME}/include
#CFLAGS	= -w -g -p -pipe -I${HOME}/include
RM	= rm -f
INSTALL	= install -s -m 755 
 
SRCS	= display.c main.c
 
OBJS	= display.o main.o
LIBS	= -lncurses
 
EXE	= hexv
 
############################################################
 
${EXE}: ${OBJS}
	@echo Linking ${EXE}
	@${CC} ${CFLAGS} -o ${EXE} ${OBJS} ${LIBS}
 
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
 
