PROG=		portal
SRCS=		portal.cc pkg.cc ui.cc gfx.cc area.cc panel.cc form.cc event.cc prompt.cc
TBSRCDIR=	termbox
TBSRCS=		${TBSRCDIR}/utf8.c ${TBSRCDIR}/termbox.c
TBOBJS=		${TBSRCS:N*.h:R:S/$/.o/g} 
OBJS=		${SRCS:N*.h:R:S/$/.o/g} ${TBOBJS}

CC?=		cc
CFLAGS+=	-g -Wall
CXXFLAGS+=	-g -Wall -std=c++11
CPPFLAGS+=	-I./termbox
LDADD=		-lstdc++ -lpthread

all:		${PROG}

${PROG}:	${OBJS}
	${CC} ${CXXFLAGS} -o ${.TARGET} ${OBJS} ${LDADD}

.cc.o:
	${CC} ${CXXFLAGS} ${CPPFLAGS} -c ${.IMPSRC}

.c.o:
	${CC} ${CFLAGS} -o ${.TARGET} -c ${.IMPSRC}

check:	${SRCS}
	cppcheck --enable=all --suppress=missingIncludeSystem ${CPPFLAGS} ${.ALLSRC}

clean:
	rm -f ${PROG} ${OBJS}
