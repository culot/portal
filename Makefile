PROG=		portal
VERSION=	0.4

SRCS=		portal.cc        \
		pkg.cc           \
		gfx.cc           \
		event.cc         \
		window.cc        \
		inputwindow.cc   \
		popupwindow.cc   \
		scrollwindow.cc  \
		tray.cc          \
                ui.cc

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
