CC	= gcc
CFLAGS	= -Wall -O6 -I ../include
AR	= ar cr
RANLIB	= ar s
RM	= rm -f
INSTALL	= cp
 
SRCS	= bitarray.cc cdate.cc cstring.cc streamab.cc csocket.cc
INCLUDES= ../include/array.h ../include/array2d.h ../include/bitarray.h \
	../include/cdate.h ../include/chain.h ../include/compat.h \
	../include/csocket.h ../include/cstring.h ../include/cendian.h \
	../include/hash.h ../include/lclist.h ../include/llist.h \
	../include/macros.h ../include/matrix.h ../include/mdefs.h \
	../include/queue.h ../include/set.h ../include/stack.h \
	../include/streamab.h ../include/templats.h ../include/vector.h
RCSED	= ${SRCS} ${INCLUDES}
 
OBJS	= bitarray.o cdate.o cstring.o streamab.o csocket.o
LIBS	= 
 
LIB	= libadt.a
 
############################################################
 
${LIB}: ${OBJS}
	@echo ${LIB} ...
	@${RM} ${LIB}
	@${AR} ${LIB} ${OBJS} 
	@${RANLIB} ${LIB}
 
.cc.o:
	@echo "	Compiling $<"
	@${CC} ${CFLAGS} -c $<
 
install: ${LIB}
	@echo Installing library ...
	@${INSTALL} ${LIB} ../lib

clean:
	@${RM} .depend *.bak *~ ${OBJS} ${LIB} ../lib/${LIB}
 
depend: ${SRCS}
	@${CC} ${CFLAGS} -MM ${SRCS} > .depend
 
ifeq (.depend,$(wildcard .depend))
include .depend
endif
 
