################ Source files ##########################################

dbv/exe		:= $Odbv/dbv
dbv/srcs	:= $(wildcard dbv/*.c)
dbv/objs	:= $(addprefix $O,$(dbv/srcs:.c=.o))
dbv/deps	:= ${dbv/objs:.o=.d}

################ Compilation ###########################################

all:		dbv/all
dbv/all:	${dbv/exe}
${dbv/exe}:	${dbv/objs} ${liba}
	@echo "Linking $@ ..."
	@${CC} ${ldflags} -o $@ $^ ${libs}

################ Installation ##########################################

.PHONY:		dbv/install dbv/uninstall dbv/clean

ifdef bindir
dbv/exei	:= ${exed}/$(notdir ${dbv/exe})

${dbv/exei}:	${dbv/exe} | ${exed}
	@echo "Installing $@ ..."
	@${INSTALL_PROGRAM} $< $@

install:	dbv/install
dbv/install:	${dbv/exei}
uninstall:	dbv/uninstall
dbv/uninstall:
	@if [ -f ${dbv/exei} ]; then\
	    echo "Removing ${dbv/exei} ...";\
	    rm -f ${dbv/exei};\
	fi
endif

################ Compilation ###########################################

clean:		dbv/clean
dbv/clean:
	@if [ -d ${builddir}/dbv ]; then\
	    rm -f ${dbv/exe} ${dbv/objs} ${dbv/deps} $Odbv/.d;\
	    rmdir ${builddir}/dbv;\
	fi

${dbv/objs}:	Makefile dbv/Module.mk ${confs} $Odbv/.d

-include ${dbv/deps}
