-include Config.mk

################ Source files ##########################################

exe	:= $O${name}
srcs	:= $(sort $(wildcard *.cc))
objs	:= $(addprefix $O,$(srcs:.cc=.o) scanner.o parser.o)
deps	:= ${objs:.o=.d}
docs	:= $(wildcard docs/*)
confs	:= Config.mk config.h ${name}.pc
oname   := $(notdir $(abspath $O))
liba_r	:= $Olib${name}.a
liba_d	:= $Olib${name}_d.a
ifdef debug
liba	:= ${liba_d}
else
liba	:= ${liba_r}
endif

################ Compilation ###########################################

.SUFFIXES:
.PHONY: all clean distclean maintainer-clean

all:	${exe}

${exe}:	${objs}
	@echo "Linking $@ ..."
	@${CC} ${ldflags} -o $@ $^ ${libs}

$O%.o:	%.cc
	@echo "    Compiling $< ..."
	@${CXX} ${cxxflags} -MMD -MT "$(<:.cc=.s) $@" -o $@ -c $<

$O%.o:	%.c
	@echo "    Compiling $< ..."
	@${CC} ${cflags} -MMD -MT "$(<:.c=.s) $@" -o $@ -c $<

%.s:	%.cc
	@echo "    Compiling $< to assembly ..."
	@${CXX} ${cxxflags} -S -o $@ -c $<

%.s:	%.c
	@echo "    Compiling $< to assembly ..."
	@${CC} ${cflags} -S -o $@ -c $<

$Oscanner.o $Oparser.o:	%.o: %.cc
	@echo "    Compiling $< ..."
	@${CXX} ${cxxflags} -I. -MMD -o $@ -c $<

$Oscanner.cc:	scanner.l | $Oparser.cc
	@echo "    Generating $@ ..."
	@${FLEX} -o $@ $<

$Oparser.cc:	parser.y
	@echo "    Generating $@ ..."
	@${BISON} -d -o $@ $<

################ Installation ##########################################

.PHONY:	install installdirs uninstall

ifdef bindir
exed	:= ${DESTDIR}${bindir}
exei	:= ${exed}/$(notdir ${exe})

${exed}:
	@echo "Creating $@ ..."
	@${INSTALL} -d $@
${exei}:	${exe} | ${exed}
	@echo "Installing $@ ..."
	@${INSTALL_PROGRAM} $< $@

installdirs:	${exed}
install:	${exei}
uninstall:
	@if [ -f ${exei} ]; then\
	    echo "Removing ${exei} ...";\
	    rm -f ${exei};\
	fi
endif
ifdef docdir
docsd	:= ${DESTDIR}${docdir}/${name}
docsi	:= $(addprefix ${docsd}/,$(notdir ${docs}))

${docsd}:
	@echo "Creating $@ ..."
	@${INSTALL} -d $@
${docsi}: ${docsd}/%: docs/% | ${docsd}
	@echo "Installing $@ ..."
	@${INSTALL_DATA} $< $@

install:	install-html
install-html:	${docsi}
installdirs:	${docsd}
uninstall:	uninstall-html
uninstall-html:
	@if [ -d ${docsd} ]; then\
	    echo "Removing documentation ...";\
	    rm -f ${docsi};\
	    rmdir ${docsd};\
	fi
endif
ifdef pkgconfigdir
pcd	:= ${DESTDIR}${pkgconfigdir}
pci	:= ${pcd}/${name}.pc

${pcd}:
	@echo "Creating $@ ..."
	@${INSTALL} -d $@
${pci}:	${name}.pc | ${pcd}
	@echo "Installing $@ ..."
	@${INSTALL_DATA} $< $@

installdirs:	${pcd}
install:	${pci}
uninstall:	uninstall-pc
uninstall-pc:
	@if [ -f ${pci} ]; then\
	    echo "Removing ${pci} ...";\
	    rm -f ${pci};\
	fi
endif

################ Modules ###############################################

include coblib/Module.mk
include test/Module.mk
include hexv/Module.mk
include dbv/Module.mk

################ Maintenance ###########################################

clean:
	@if [ -d ${builddir} ]; then\
	    rm -f ${exe} ${objs} ${deps} $Oparser.cc $Oparser.hh $Oscanner.cc $O.d;\
	    rmdir ${builddir};\
	fi

distclean:	clean
	@rm -f ${oname} ${confs} config.status

maintainer-clean: distclean

$O.d:	${builddir}/.d
	@[ -h ${oname} ] || ln -sf ${builddir} ${oname}
$O%/.d:	$O.d
	@[ -d $(dir $@) ] || mkdir $(dir $@)
	@touch $@
${builddir}/.d:	Makefile
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	@touch $@

Config.mk:	Config.mk.in
config.h:	config.h.in
${name}.pc:	${name}.pc.in
${objs}:	Makefile ${confs} $O.d
${confs}:	configure
	@if [ -x config.status ]; then echo "Reconfiguring ...";\
	    ./config.status;\
	else echo "Running configure ...";\
	    ./configure;\
	fi

-include ${dep}
