include ../Config.mk

LIB	:= libdbf.a
SRCS	:= $(wildcard *.c)
OBJS	:= ${SRCS:.c=.o}
 
 
############################################################

${LIB}:	${OBJS}
	@echo "Linking $@ ..."
	@${AR} ruv $@ $^
	@${RANLIB} $@

%.o:	%.c
	@echo "    Compiling $< ..."
	@${CC} ${CFLAGS} -o $@ -c $<

clean:
	@rm -f ${LIB} ${OBJS}
 
depend: ${SRCS}
	@${CC} ${CFLAGS} -MM ${SRCS} > .depend
 
-include .depend
