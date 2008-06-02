include ../Config.mk

LIB	= libadt.a
SRCS	= $(wildcard *.cc)
OBJS	= ${SRCS:.cc=.o}
 
############################################################
 
${LIB}: ${OBJS}
	@echo "Linking $@ ..."
	@rm -f $@
	@${AR} q $@ $^ &> /dev/null
	@${RANLIB} $@

%.o:	%.cc
	@echo "    Compiling $< ..."
	@${CXX} ${CXXFLAGS} -o $@ -c $<

clean:
	@rm -f ${OBJS} ${LIB}
 
depend: ${SRCS}
	@${CXX} ${CXXFLAGS} -MM ${SRCS} > .depend
 
-include .depend
