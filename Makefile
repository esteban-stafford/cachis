

MODULES = src/gui.c src/dictionary.c src/confparser.c src/iniparser.c src/datamanipulation.c src/traceparser.c src/datastore.c src/datainterface.c src/simulator.c src/mappingpolicy.c src/writepolicy.c
HEADERS = include/gui.h include/dictionary.h include/confparser.h include/iniparser.h include/datamanipulation.h include/traceparser.h include/datastore.h include/datainterface.h include/simulator.h include/computer.h include/mappingpolicy.h include/writepolicy.h

DEBUG=0
COVER=0

LIBS = $(shell pkg-config --libs gtk4) -lm
CFLAGS += -g -Iinclude -Wreturn-type -DDEBUG=${DEBUG}  $(shell pkg-config --cflags gtk4)

ifeq ($(COVER),1)
CFLAGS += --coverage
endif

all: cachis

cachis: src/cachis.c ${MODULES}
	gcc -o $@ ${CFLAGS} $< ${MODULES} ${LIBS}

clean:
	rm -fr *~
	rm -fr *.g*
	rm -f cachis
