CC:=gcc

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

CFLAGS=-Iinclude/

ifndef RELEASE
CFLAGS+=-fsanitize=address -ggdb
else
CFLAGS+=-s
endif

LDFLAGS=-lc -lreadline
ifndef RELEASE
LDFLAGS+=-fsanitize=address
endif

OBJS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))

.PHONY: all
all: $(BIN_DIR)/afi $(BIN_DIR)/afi.a

$(BIN_DIR)/afi: $(OBJS) $(OBJ_DIR)/hw/linux.o
	@mkdir -p bin
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN_DIR)/afi.a: $(OBJS)
	@mkdir -p bin
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p obj/hw/
	$(CC)  $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/hw/%.o: $(SRC_DIR)/hw/%.c
	@mkdir -p obj/hw/
	$(CC)  $(CFLAGS) -c $< -o $@
