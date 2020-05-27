################ Source files ##########################################

hexv/exe	:= $Ohexv/hexv
hexv/srcs	:= $(wildcard hexv/*.c)
hexv/objs	:= $(addprefix $O,$(hexv/srcs:.c=.o))
hexv/deps	:= ${hexv/objs:.o=.d}

################ Compilation ###########################################

all:		hexv/all
hexv/all:	${hexv/exe}
${hexv/exe}:	${hexv/objs} ${liba}
	@echo "Linking $@ ..."
	@${CC} ${ldflags} -o $@ $^ ${libs}

################ Installation ##########################################

.PHONY:		hexv/install hexv/uninstall hexv/clean

ifdef bindir
hexv/exei	:= ${exed}/$(notdir ${hexv/exe})

${hexv/exei}:	${hexv/exe} | ${exed}
	@echo "Installing $@ ..."
	@${INSTALL_PROGRAM} $< $@

install:	hexv/install
hexv/install:	${hexv/exei}
uninstall:	hexv/uninstall
hexv/uninstall:
	@if [ -f ${hexv/exei} ]; then\
	    echo "Removing ${hexv/exei} ...";\
	    rm -f ${hexv/exei};\
	fi
endif

################ Compilation ###########################################

clean:		hexv/clean
hexv/clean:
	@if [ -d ${builddir}/hexv ]; then\
	    rm -f ${hexv/exe} ${hexv/objs} ${hexv/deps} $Ohexv/.d;\
	    rmdir ${builddir}/hexv;\
	fi

${hexv/objs}:	Makefile hexv/Module.mk ${confs} $Ohexv/.d

-include ${hexv/deps}
