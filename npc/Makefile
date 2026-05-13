# --- NPC Makefile ---
TOPNAME = top
NPC_HOME = $(abspath .)
BUILD_DIR = $(NPC_HOME)/build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

# --- 源代码路径 ---
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")

# --- 编译工具与参数 ---
VERILATOR = verilator
VERILATOR_CFLAGS += -MMD --build -cc --trace \
                    -O3 --x-assign fast --x-initial fast --noassert \
					-I$(abspath ./vsrc/common) \
                    -I$(abspath ./vsrc/core)
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""

# --- 核心目标 ---
default: $(BIN)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# 编译 NPC 仿真器
$(BIN): $(VSRCS) $(CSRCS)
	@mkdir -p $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) \
		$(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

# --- 运行逻辑 ---
IMG ?= 
ARGS ?=

run: $(BIN)
	@if [ "$(IMG)" = "" ]; then \
		echo "[ERROR] No image file specified! Usage: make run IMG=path/to/xxx.bin"; exit 1; \
	fi
	@echo "[START] Simulation Started with $(IMG) $(ARGS)"
	$(BIN) $(IMG) $(ARGS)\
	
clean:
	rm -rf $(BUILD_DIR)

gtk:
	gtkwave $(OBJ_DIR)/Top.vcd

.PHONY: default all run clean gtk