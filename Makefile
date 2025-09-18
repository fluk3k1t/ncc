TARGET      := ncc
TEST_TARGET := ncc_test
BUILD_DIR   := obj
TEST_DIR    := test
INC_DIR     := inc
SRC_DIR     := src
OBJ_DIR     := $(BUILD_DIR)
TEST_OBJDIR := $(BUILD_DIR)/test
GTEST_DIR   := gtest

SRCS      := $(wildcard $(SRC_DIR)/*.c)
HDRS      := $(wildcard $(INC_DIR)/*.h)
OBJS      := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
OBJ_NO_MAIN    := $(filter-out $(OBJ_DIR)/main.o,$(OBJS))
TEST_SRC := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ := $(patsubst $(TEST_DIR)/%.c,$(TEST_OBJDIR)/%.o,$(TEST_SRC))

CC      := g++
CFLAGS  :=  -x c -Wall -I$(INC_DIR)
LDFLAGS :=
LDLIBS  := -lgtest_main -lgtest -lpthread

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

$(TEST_OBJDIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -I$(GTEST_DIR) -c $< -o $@

$(TEST_TARGET): $(OBJ_NO_MAIN) $(TEST_OBJ)
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)


.PHONY: run test clean
run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJS) $(TEST_OBJ) $(TARGET) $(TEST_TARGET)
