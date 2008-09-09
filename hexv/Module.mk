################ Source files ##########################################

hexv/EXE	:= hexv/hexv
hexv/SRCS	:= $(wildcard hexv/*.c)
hexv/OBJS	:= $(addprefix $O,$(hexv/SRCS:.c=.o))
hexv/LIBS	:= ${COBLIB} -lncurses

################ Compilation ###########################################

all:		hexv/all
hexv/all:	${hexv/EXE}
${hexv/EXE}:	${hexv/OBJS} ${COBLIB}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $^ ${hexv/LIBS}

################ Installation ##########################################

.PHONY:		hexv/install hexv/uninstall hexv/clean

ifdef BINDIR
hexv/EXEI	:= $(addprefix ${BINDIR}/,$(notdir ${hexv/EXE}))
install:	hexv/install
hexv/install:	${hexv/EXEI}
${hexv/EXEI}:	${hexv/EXE}
	@echo "Installing $@ ..."
	@${INSTALLBIN} $< $@
uninstall:	hexv/uninstall
hexv/uninstall:
	@echo "Removing ${hexv/EXEI} ..."
	@rm -f ${hexv/EXEI}
endif

################ Compilation ###########################################

clean:		hexv/clean
hexv/clean:
	@rm -f ${hexv/EXE} ${hexv/OBJS} $(hexv/OBJS:.o=.d)

${hexv/OBJS}:	Makefile Config.mk hexv/Module.mk
-include ${hexv/OBJS:.o=.d}
