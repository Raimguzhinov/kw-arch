CC = gcc
CFLAGS = -Wall -I /src/libmySimpleComputer /src/libmyTerm/

APP_NAME = mySimpleComputer
LIB_COMPUTER_NAME = mySimpleComputer

OBJ_DIR = obj
OBJ_SRC_DIR = $(OBJ_DIR)/src

LIB_DIR = lib
BIN_DIR = bin
SRC_DIR = src

APP_PATH = $(BIN_DIR)/$(APP_NAME)
LIB_COMPUTER_PATH = $(LIB_DIR)/lib$(LIB_COMPUTER_NAME).a

LDLIBS = -L$(LIB_DIR) -lmySimpleComputer -lmyTerm

SRC_EXT = c

APP_SOURCES = $(shell find $(SRC_DIR)/$(APP_NAME) -name '*.$(SRC_EXT)')
APP_OBJECTS = $(APP_SOURCES:$(SRC_DIR)/%.$(SRC_EXT)=$(OBJ_DIR)/$(SRC_DIR)/%.o)

LIB_COMPUTER_SOURCES = $(shell find $(SRC_DIR)/lib$(LIB_COMPUTER_NAME) -name '*.$(SRC_EXT)')
LIB_COMPUTER_OBJECTS = $(LIB_COMPUTER_SOURCES:$(SRC_DIR)/%.$(SRC_EXT)=$(OBJ_DIR)/$(SRC_DIR)/%.o)


DEPS=$(APP_OBJECTS:.o=.d)$(LIB_COMPUTER_SOURCES:.o=.d)

.PHONY: all
all: $(APP_PATH)

-include $(DEPS)

$(APP_PATH): $(APP_OBJECTS) $(LIB_COMPUTER_PATH)
	$(CC) $(CFLAGS) $(APP_OBJECTS) -o $@ $(LDLIBS)

$(LIB_COMPUTER_PATH): $(LIB_COMPUTER_OBJECTS)
	ar rcs $@ $^

$(OBJ_DIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

run: $(APP_PATH)
	$(APP_PATH)

.PHONY: clean
clean:
	$(RM) $(APP_PATH) $(LIB_COMPUTER_PATH)
	find $(OBJ_DIR) -name '*.o' -exec $(RM) '{}' \;
	find $(OBJ_DIR) -name '*.d' -exec $(RM) '{}' \;
