PROG=		portal
VERSION=	0.3

SRCS=		portal.cc pkg.cc ui.cc gfx.cc area.cc panel.cc form.cc event.cc prompt.cc
OBJS=		${SRCS:N*.h:R:S/$/.o/g}

CC?=		cc
CFLAGS+=	-g -Wall
CXXFLAGS+=	-g -Wall -std=c++11
CPPFLAGS+=
LDADD=		-lstdc++ -lpthread -lncurses
DEFS=		-DVERSION=${VERSION}

all:		${PROG}

${PROG}:	${OBJS}
	${CC} ${CXXFLAGS} -o ${.TARGET} ${OBJS} ${LDADD}

.cc.o:
	${CC} ${CXXFLAGS} ${CPPFLAGS} ${DEFS} -c ${.IMPSRC}

.c.o:
	${CC} ${CFLAGS} -o ${.TARGET} -c ${.IMPSRC}

check:	${SRCS}
	cppcheck --enable=all --suppress=missingIncludeSystem ${CPPFLAGS} ${.ALLSRC}

clean:
	rm -f ${PROG} ${OBJS}
