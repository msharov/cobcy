include ../Config.mk

LIB	= libcobol.a
SRCS	= $(wildcard *.c) ../dbf/dbf.c ../dbf/ndx.c
OBJS	= ${SRCS:.c=.o}

#############################################

${LIB}:	${OBJS}
	@echo "Linking $@ ..."
	@rm -f $@
	@${AR} q $@ $^
	@${RANLIB} $@

%.o:	%.c
	@echo "    Compiling $< ..."
	@${CC} ${CFLAGS} -o $@ -c $<

%.s:	%.c
	@echo "    Compiling $< to assembly ..."
	@${CC} ${CFLAGS} -Os -S -o $@ -c $<

depend:
	@echo "Generating dependencies ..."
	@${CC} ${CFLAGS} -MM ${SRCS} > .depend

clean:
	@rm -f ${OBJS} ${LIB}

-include .depend
