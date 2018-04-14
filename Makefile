CC:=gcc
TARGET:=linux

SRC_DIR=src
OBJ_DIR=obj
BIN_DIR=bin

CFLAGS=--std=c99
CFLAGS+=-Iinclude/ -Iinclude/hw/$(TARGET)
CFLAGS+=-MMD

ifdef RELEASE
CFLAGS+=-s
else
CFLAGS+=-fsanitize=address -ggdb
endif

LDFLAGS=-lc -lreadline
ifndef RELEASE
LDFLAGS+=-fsanitize=address
endif

OBJS=$(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))
HWOBJS=$(patsubst $(SRC_DIR)/hw/$(TARGET)/%.c,$(OBJ_DIR)/hw/$(TARGET)/%.o,$(wildcard $(SRC_DIR)/hw/$(TARGET)/*.c))

.PHONY: all clean clean-all
all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(HWOBJS) $(BIN_DIR)/afi.a
	@mkdir -p bin
	$(CC) $^ -o $@ $(LDFLAGS)

$(BIN_DIR)/afi.a: $(OBJS)
	@mkdir -p bin
	$(AR) rcs $@ $^

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p obj/
	$(CC)  $(CFLAGS) -c $< -o $@
$(OBJ_DIR)/hw/$(TARGET)/%.o: $(SRC_DIR)/hw/$(TARGET)/%.c
	@mkdir -p obj/hw/$(TARGET)
	$(CC)  $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj/

clean-all: clean
	rm -rf bin/

-include $(OBJS:.o=.d)
