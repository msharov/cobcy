################ Source files ##########################################

dbv/EXE		:= dbv/dbv
dbv/SRCS	:= $(wildcard dbv/*.c)
dbv/OBJS	:= $(addprefix $O,$(dbv/SRCS:.c=.o))
dbv/LIBS	:= ${COBLIB} -lncurses

################ Compilation ###########################################

all:		dbv/all
dbv/all:	${dbv/EXE}
${dbv/EXE}:	${dbv/OBJS} ${COBLIB}
	@echo "Linking $@ ..."
	@${LD} ${LDFLAGS} -o $@ $^ ${dbv/LIBS}

################ Installation ##########################################

.PHONY:		dbv/install dbv/uninstall dbv/clean

ifdef BINDIR
dbv/EXEI	:= $(addprefix ${BINDIR}/,$(notdir ${dbv/EXE}))
install:	dbv/install
dbv/install:	${dbv/EXEI}
${dbv/EXEI}:	${dbv/EXE}
	@echo "Installing $@ ..."
	@${INSTALLBIN} $< $@
uninstall:	dbv/uninstall
dbv/uninstall:
	@echo "Removing ${dbv/EXEI} ..."
	@rm -f ${dbv/EXEI}
endif

################ Compilation ###########################################

clean:		dbv/clean
dbv/clean:
	@rm -f ${dbv/EXE} ${dbv/OBJS} $(dbv/OBJS:.o=.d)
	@rmdir $Odbv &> /dev/null || true

${dbv/OBJS}:	Makefile Config.mk dbv/Module.mk
-include ${dbv/OBJS:.o=.d}
