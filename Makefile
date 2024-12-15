

MODULES = src/gui.c src/dictionary.c src/confparser.c src/iniparser.c src/statistics.c src/datamanipulation.c src/traceparser.c src/datastore.c src/datainterface.c src/simulator.c src/mappingpolicy.c src/writepolicy.c src/replacementpolicy.c
HEADERS = src/gui.h src/dictionary.h src/confparser.h src/iniparser.h src/statistics.h src/datamanipulation.h src/traceparser.h src/datastore.h src/datainterface.h src/simulator.h src/computer.h src/mappingpolicy.h src/writepolicy.h src/replacementpolicy.h

DEBUG=1
COVER=1

LIBS = $(shell pkg-config --libs gtk4) -lm
CFLAGS += -g -Wreturn-type -DDEBUG=${DEBUG}  $(shell pkg-config --cflags gtk4)

ifeq ($(COVER),1)
CFLAGS += -fprofile-arcs -ftest-coverage
endif

all: cachis

cachis: src/cachis.c ${MODULES}
	gcc -o $@ ${CFLAGS} $< ${MODULES} ${LIBS}

clean:
	rm -fr *~
	rm -fr *.g*
	rm -f cachis
