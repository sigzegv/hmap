CC=gcc

BUILD_DIR := .build
DEPS_DIR := .deps

BIN_TEST := hmap_test
BIN_SHARED := libhmap.so
BIN_STATIC := libhmap.a

CFLAGS=-fPIC
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPS_DIR)/$*.d
LIBS = -lcunit

CSRC := $(wildcard *.c)
DEPS := $(CSRC:%.c=$(DEPS_DIR)/%.d)

OBJS := $(BUILD_DIR)/hmap.o
OBJS_TEST = $(BUILD_DIR)/hmap_test.o $(BUILD_DIR)/hmap.o

all: $(BIN_SHARED) $(BIN_STATIC) $(BIN_TEST)

$(BUILD_DIR): ; @mkdir -p $@
$(DEPS_DIR): ; @mkdir -p $@

$(BIN_SHARED): $(OBJS)
	$(CC) -shared $^ -o $@ $(LIBS)

$(BIN_STATIC): $(OBJS)
	ar rcs libhmap.a $^

$(BIN_TEST): $(OBJS_TEST)
	$(CC) $^ -o $@ $(LIBS)

$(BUILD_DIR)/%.o: %.c $(DEPS_DIR)/%.d | $(DEPS_DIR)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

test: $(BIN_TEST)
	./$(BIN_TEST)

clean:
	rm -rf .build
	rm -rf .deps
	rm -rf $(BIN_TEST)

.PHONY: clean

$(DEPS):
include $(wildcard $(DEPS))
