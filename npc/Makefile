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

# 默认镜像文件名，如果命令行没传 IMG=xxx，则默认用 inst
IMG ?= inst
# 定义资源目录
RES_DIR = ./resources
# 定义对应的 hex 和 bin 路径
HEX_FILE = $(RES_DIR)/hex/$(IMG).hex
BIN_FILE = $(RES_DIR)/bin/$(IMG).bin
# 创建 bin 目录确保转换时目录存在
$(shell mkdir -p $(RES_DIR)/bin)

# 运行规则
run: $(BIN)
	@echo "[CHECK] Checking $(HEX_FILE)..."
	@if [ ! -f $(HEX_FILE) ]; then \
		echo "[ERROR] $(HEX_FILE) not found!"; exit 1; \
	fi
	@echo "[CONVERT] Transforming Hex to Bin..."
	@python3 -c "import struct; f=open('$(HEX_FILE)','r'); b=open('$(BIN_FILE)','wb'); [b.write(struct.pack('<I', int(w, 16))) for l in f for l_c in [l.split('//')[0].split('#')[0].strip()] if l_c and 'v3.0' not in l_c for w in l_c.split(':')[-1].split()]; f.close(); b.close()"
	@echo "[START] Simulation Started with $(BIN_FILE)"
	@$(BIN) $(BIN_FILE)

# 清理
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(RES_DIR)/bin/*.bin

gtk:
	gtkwave $(OBJ_DIR)/Top.vcd

.PHONY: default all run clean gtk