include config.mk

SRC = ivrit_status.c
OBJ = ${SRC:.c=.o}

all: options ivrit_status

options:
	@echo ivrit_status build options:
	@echo "CC       = ${CC}"

clean:
	rm -f ivrit_status ${OBJ} ivrit_status-${VERSION}.tar.gz

dist: clean
	mkdir -p ivrit_status-${VERSION}
	cp -R Makefile config.mk ${SRC} ivrit_status-${VERSION}
	tar -cf ivrit_status-${VERSION}.tar ivrit_status-${VERSION}
	gzip ivrit_status-${VERSION}.tar
	rm -rf ivrit_status-${VERSION}

install: all

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ivrit_status ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/ivrit_status

