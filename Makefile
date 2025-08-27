DIR := $(shell pwd)
SRC_DIR := $(DIR)/src
BUILD_DIR := $(DIR)/build
LIB_DIR := $(DIR)/ext/lib/native
INC_DIR := $(DIR)/ext/include $(SRC_DIR) $(BUILD_DIR)

CPPFLAGS := $(addprefix -I, $(INC_DIR)) -MMD -MP
LDFLAG := -L$(LIB_DIR) -lkoopa
CCEXT := .cc
GCC := g++
CXXFLAGS = -Wall -g -O0 -std=c++20
EXEC := compiler

FB_SRCS := $(patsubst $(SRC_DIR)/%.l, $(BUILD_DIR)/%.lex$(CCEXT), $(shell find $(SRC_DIR) -name "*.l"))
FB_SRCS += $(patsubst $(SRC_DIR)/%.y, $(BUILD_DIR)/%.tab$(CCEXT), $(shell find $(SRC_DIR) -name "*.y"))
CC_SRCS := $(shell find $(SRC_DIR) -name "*.cc")
FB_OBJS := $(patsubst $(BUILD_DIR)/%$(CCEXT), $(BUILD_DIR)/%.o, $(FB_SRCS))
CC_OBJS := $(patsubst $(SRC_DIR)/%$(CCEXT), $(BUILD_DIR)/%.o, $(CC_SRCS))
OBJS := $(FB_OBJS) $(CC_OBJS)

DEPS := $(OBJS:.o=.d)

$(BUILD_DIR)/$(EXEC): $(FB_SRCS) $(OBJS)
	$(GCC) $(OBJS) $(LDFLAG) -ldl -lpthread -o $@


define cxx_recipe
	mkdir -p $(dir $@)
	$(GCC) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<
endef
$(BUILD_DIR)/%.o: $(SRC_DIR)/%$(CCEXT); $(cxx_recipe)
$(BUILD_DIR)/%.o: $(BUILD_DIR)/%$(CCEXT); $(cxx_recipe)

$(BUILD_DIR)/%.tab$(CCEXT): $(SRC_DIR)/%.y
	mkdir -p $(dir $@)
	bison -d -o $@ $<

$(BUILD_DIR)/%.lex$(CCEXT): $(SRC_DIR)/%.l
	mkdir -p $(dir $@)
	flex -o $@ $<

.PHONY: clean

clean:
	-rm -rf $(BUILD_DIR)

run_koopa:
	koopac out.koopa | llc --filetype=obj -o out.o
	clang out.o -L$(CDE_LIBRARY_PATH)/native -lsysy -o out
	./out; echo $$?

run_asm:
	clang out.S -c -o out.o -target riscv32-unknown-linux-elf -march=rv32im -mabi=ilp32
	ld.lld out.o -L$(CDE_LIBRARY_PATH)/riscv32 -lsysy -o out
	qemu-riscv32-static out; echo $$?

-include $(DEPS)