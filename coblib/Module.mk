################ Source files ##########################################

coblib/srcs	:= $(wildcard coblib/*.c)
coblib/incs	:= $(wildcard coblib/*.h)
coblib/objs	:= $(addprefix $O,${coblib/srcs:.c=.o})
coblib/deps	:= ${coblib/objs:.o=.d}

################ Compilation ###########################################

.PHONY:	coblib/all coblib/clean

all:		coblib/all
coblib/all:	${liba}
${liba}:	${coblib/objs}
	@echo "Linking $@ ..."
	@rm -f $@
	@${AR} qc $@ $^
	@${RANLIB} $@

################ Installation ##########################################

.PHONY:	coblib/install coblib/uninstall

ifdef includedir
incsd	:= ${DESTDIR}${includedir}/coblib
incsi	:= $(addprefix ${incsd}/,$(notdir ${coblib/incs}))

${incsd}:
	@echo "Creating $@ ..."
	@${INSTALL} -d $@
${incsi}: ${incsd}/%.h: coblib/%.h | ${incsd}
	@echo "Installing $@ ..."
	@${INSTALL_DATA} $< $@

installdirs:	${incsd}
install:	coblib/install
coblib/install:	${incsi}
uninstall:		coblib/uninstall
coblib/uninstall:	coblib/uninstall-incs
coblib/uninstall-incs:
	@if [ -d ${incsd} ]; then\
	    echo "Removing headers ...";\
	    rm -f ${incsi};\
	    rmdir ${incsd};\
	fi
endif

ifdef libdir
libad	:= ${DESTDIR}${libdir}
libai	:= ${libad}/$(notdir ${liba})
libai_r	:= ${libad}/$(notdir ${liba_r})
libai_d	:= ${libad}/$(notdir ${liba_d})

${libad}:
	@echo "Creating $@ ..."
	@${INSTALL} -d $@
${libai}:	${liba} | ${libad}
	@echo "Installing $@ ..."
	@${INSTALL_DATA} $< $@

installdirs:	${libad}
coblib/install:		${libai}
coblib/uninstall:	coblib/uninstall-libs
coblib/uninstall-libs:
	@if [ -f ${libai_r} -o -f ${libai_d} ]; then\
	    echo "Removing ${libai} ...";\
	    rm -f ${libai_r} ${libai_d};\
	fi
endif

################ Housekeeping ##########################################

clean:		coblib/clean
coblib/clean:
	@if [ -d ${builddir}/coblib ]; then\
	    rm -f ${coblib/objs} ${coblib/deps} ${liba_r} ${liba_d} $Ocoblib/.d;\
	    rmdir ${builddir}/coblib;\
	fi

${coblib/objs}:	Makefile coblib/Module.mk ${confs} $Ocoblib/.d

-include ${coblib/deps}
