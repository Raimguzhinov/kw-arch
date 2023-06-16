BIN_DIR = bin
LIBS_DIRS = ${patsubst libs/%,%,${wildcard libs/*} }
MYLIBS = mySimpleComputer myTerm myBigChars myInterface myReadKey myALU myCU mySignal

CFLAGS = -Wall ${addprefix -I libs/,${LIBS_DIRS}}
LDLIBS = ${addprefix -L libs/,${LIBS_DIRS}} ${addprefix -l,${LIBS_DIRS}}

.PHONY: all build_dirs run format

all: build_dirs binary

binary: ${MYLIBS}
	${CC} $(CFLAGS) -o ${BIN_DIR}/binary project/main.c $(LDLIBS)

.ONESHELL:
sat: project/commands.h
	$(CXX) $(CFLAGS) -I project/commands.h -o ${BIN_DIR}/sat project/assembler/sat.cpp $(LDLIBS)
	./${BIN_DIR}/sat project/assembler/factorial.sa resources/factorial_to_machine.o;
	${MAKE} all run

.ONESHELL:
sbt: project/commands.h
	$(CXX) -std=c++20 project/basic/sbt.cpp -o ${BIN_DIR}/sbt
	./${BIN_DIR}/sbt project/basic/factorial.sb project/assembler/factorial.sa;
	$(CXX) -std=c++20 $(CFLAGS) -I project/commands.h -o ${BIN_DIR}/sbat project/assembler/sat.cpp $(LDLIBS)
	./${BIN_DIR}/sbat project/assembler/factorial.sa resources/basic_to_assembly.o;
	${MAKE} all run

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

format:
	clang-format --style GNU -i --verbose project/main.c
	clang-format --style GNU -i --verbose libs/*/*.c
	clang-format --style GNU -i --verbose libs/*/*.h

clean:
	${RM} ./${BIN_DIR}/*
	${RM} $(shell find ./libs -name \*.a)
	${RM} $(shell find ./libs -name \*.o)
	${RM} $(shell find ./resources -name \*.o)
