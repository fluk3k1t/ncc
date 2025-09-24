TARGET      := ncc
TARGET_DEBUG:= ncc_debug
TEST_TARGET := ncc_test
BUILD_DIR   := obj
TEST_DIR    := test
INC_DIR     := inc
SRC_DIR     := src
OBJ_DIR     := $(BUILD_DIR)
# OBJ_DIR_DEBUG := $(BUILD_DIR)/debug
TEST_OBJDIR := $(BUILD_DIR)/test
GTEST_DIR   := gtest

SRCS      := $(wildcard $(SRC_DIR)/*.c)
HDRS      := $(wildcard $(INC_DIR)/*.h)
OBJS      := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
OBJ_NO_MAIN    := $(filter-out $(OBJ_DIR)/main.o,$(OBJS))
TEST_SRC := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ := $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJDIR)/%.o,$(TEST_SRC))

CC      := g++
CFLAGS  := -x c -Wall -I$(INC_DIR)
LDFLAGS :=
LDLIBS  := -lgtest_main -lgtest -lpthread

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

OBJ_DIR_DEBUG := $(BUILD_DIR)/debug
OBJS_DEBUG    := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR_DEBUG)/%.o,$(SRCS))

$(OBJ_DIR_DEBUG)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -g -O0 -c $< -o $@

$(TARGET_DEBUG): $(OBJS_DEBUG)
	$(CC) -g $^ -o $@

$(TEST_OBJDIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -I$(GTEST_DIR) -c $< -o $@

$(TEST_TARGET): $(OBJ_NO_MAIN) $(TEST_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

.PHONY: run debug test clean
run: $(TARGET)
	./$(TARGET)

debug: $(TARGET_DEBUG)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJS) $(TEST_OBJ) $(TARGET) $(TEST_TARGET) $(TARGET_DEBUG)
