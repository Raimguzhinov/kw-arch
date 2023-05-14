BIN_DIR = bin
LIBS_DIRS = ${patsubst libs/%,%,${wildcard libs/*} }
MYLIBS = mySimpleComputer myTerm myBigChars myInterface myReadKey myALU myCU mySignal

CFLAGS = -Wall ${addprefix -I libs/,${LIBS_DIRS}}
LDLIBS = ${addprefix -L libs/,${LIBS_DIRS}} ${addprefix -l,${LIBS_DIRS}}

.PHONY: all build_dirs run

all: build_dirs binary

binary: ${MYLIBS}
	${CC} $(CFLAGS) -o ${BIN_DIR}/binary project/main.c $(LDLIBS)

sat: ${MYLIBS}
	${CC} $(CFLAGS) -o ${BIN_DIR}/sat project/assembler/sat.cpp $(LDLIBS)
	./${BIN_DIR}/sat project/assembler/sat.sa resources/sat.o;

test: ${MYLIBS}
	${CC} $(CFLAGS) -o ${BIN_DIR}/testbch tests/bch.c $(LDLIBS)
	${CC} $(CFLAGS) -o ${BIN_DIR}/testif tests/iface.c $(LDLIBS)
	${CC} $(CFLAGS) -o ${BIN_DIR}/testrk tests/rk.c $(LDLIBS)
	./${BIN_DIR}/testrk;

${MYLIBS}:
	${MAKE} -C libs/${notdir $@}

build_dirs:
	mkdir -p ${BIN_DIR}

run:
	./${BIN_DIR}/binary;

clean:
	${RM} ./${BIN_DIR}/*
	${RM} $(shell find ./libs -name \*.a)
	${RM} $(shell find ./libs -name \*.o)
