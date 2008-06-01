include ../Config.mk

COBS	:= $(wildcard bvt*.cob)
SRCS	:= $(COBS:.cob=.c)
INCS	:= $(COBS:.cob=.h)
OBJS	:= $(SRCS:.c=.o)
BVTS	:= $(COBS:.cob=)
LIBS	:= -lcobol
COBC	:= ../cobcy
CFLAGS	+= -I../lib
LDFLAGS	+= -L../lib
SEP	= "------------------------------------------------------------"

.PHONY:	all run clean depend
.SECONDARY: ${INCS} ${SRCS}

run:	${BVTS}
	@for i in ${BVTS}; do					\
	    echo "Running $$i";		echo ${SEP} > $$i.out;	\
	    cat $$i.h >> $$i.out;	echo ${SEP} >> $$i.out;	\
	    cat $$i.c >> $$i.out;	echo ${SEP} >> $$i.out;	\
	    ./$$i >> $$i.out;		echo ${SEP} >> $$i.out;	\
	    diff $$i.std $$i.out;				\
	    if [ $$? == 0 ]; then rm $$i.out; fi;		\
	done

bvt%:	bvt%.o
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $^ ${LIBS}

%.c:	%.cob
	@echo "    Converting $< ..."
	@${COBC} -o $@ $<

%.o:	%.c
	@echo "    Compiling $< ..."
	@${CC} ${CFLAGS} -o $@ -c $<

%.s:	%.c
	@echo "    Compiling $< to assembly ..."
	@${CC} ${CFLAGS} -Os -S -o $@ -c $<

clean:
	@echo "Removing generated files ..."
	@rm -f ${BVTS} ${OBJS} ${SRCS} ${INCS}
	@rm -f output-file *.dbf *.ndx runall.out
