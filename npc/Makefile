# NPC Makefile
TOPNAME = top
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj_dir
BIN = $(BUILD_DIR)/$(TOPNAME)

# SRCS
VSRCS = $(shell find $(abspath ./vsrc) -name "*.v")
CSRCS = $(shell find $(abspath ./csrc) -name "*.c" -or -name "*.cc" -or -name "*.cpp")

# 工具与参数
VERILATOR = verilator

# -I$(abspath ./vsrc) 确保 verilator 能找到被 include 的 Reg.v 和 RegisterFile.v
VERILATOR_CFLAGS += -MMD --build -cc --trace \
                    -O3 --x-assign fast --x-initial fast --noassert \
					-I$(abspath ./vsrc/common) \
                    -I$(abspath ./vsrc/core)

# 编译参数
INCFLAGS = $(addprefix -I, $(INC_PATH))
CXXFLAGS += $(INCFLAGS) -DTOP_NAME="\"V$(TOPNAME)\""

# 默认目标
default: $(BIN)

# 创建构建目录
$(shell mkdir -p $(BUILD_DIR))

# 编译规则
$(BIN): $(VSRCS) $(CSRCS)
	@rm -rf $(OBJ_DIR)
	$(VERILATOR) $(VERILATOR_CFLAGS) \
		--top-module $(TOPNAME) $^ \
		$(addprefix -CFLAGS , $(CXXFLAGS)) \
		$(addprefix -LDFLAGS , $(LDFLAGS)) \
		--Mdir $(OBJ_DIR) --exe -o $(abspath $(BIN))

# 运行规则
run: $(BIN)
	@echo "[START] Simulation Started."
	@$(BIN)

# 清理
clean:
	rm -rf $(BUILD_DIR)

gtk:
	gtkwave $(OBJ_DIR)/Top.vcd

.PHONY: default all run clean gtk