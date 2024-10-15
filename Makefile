

MODULES = gui.c dictionary.c confparser.c iniparser.c datamanipulation.c

DEBUG=0
COVER=0

LIBS = $(shell pkg-config --libs gtk+-2.0) -lm
CFLAGS += -g -Wreturn-type -DDEBUG=${DEBUG}  $(shell pkg-config --cflags gtk+-2.0)

ifeq ($(COVER),1)
CFLAGS += --coverage
endif

cachis: mainprogram.c ${MODULES}
	gcc -o $@ ${CFLAGS} $< ${MODULES} ${LIBS}

clean:
	rm -fr *~
	rm -fr *.g*
	rm cache_simulator

