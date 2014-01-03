# Makefile to compile the sources

UNAME!=		/usr/bin/uname
EXEC=		ircbot
CC=		cc
INCLUDE:=	-I./include
CFLAGS=		-Wall -ansi -D_GNU_SOURCE ${INCLUDE}
LDFLAGS:=	-L/usr/lib
LIBS=		-lconfig -lircclient -lsqlite3
OBJ=		${SRC:.c=.o}
SRC=		cfg.c cmd.c daemon.c database.c events.c log.c main.c

.if (${UNAME} == "NetBSD")
LDFLAGS+=	-L/usr/pkg/lib
INCLUDE+=	-I/usr/pkg/include
BINDIR=		/usr/pkg/bin/
.else
LDFLAGS+=	-L/usr/local/lib
INCLUDE+=	-I/usr/local/include
BINDIR=		/usr/local/bin/
.endif

all: ${EXEC}

${EXEC}: ${OBJ}
	@echo "LINK	$@"
	@${CC} ${LDFLAGS} ${CFLAGS} ${OBJ} -o $@ ${LIBS}

.c.o:
	@echo "CC	$<"
	@${CC} -o $@ -c $< ${CFLAGS}

clean:
	@echo "RM	${EXEC} *.o"
	@rm -f ${SRCDIR}*.o ${EXEC} ${EXEC}.core

install:
	@echo "INSTALL		${EXEC}"
	@chmod 755 ${EXEC}
	@chown root:wheel ${EXEC}
	@cp ${EXEC} ${BINDIR}

deinstall:
	@echo "DEINSTALL	${EXEC}"
	@rm -f ${BINDIR}${EXEC}
