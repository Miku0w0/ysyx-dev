# =====================================
# Verilator 仿真 Makefile for npc
# =====================================

# 工具定义
VERILATOR = verilator
CXX       = g++
GTKWAVE   = gtkwave

# 配置部分
TOP        := top
VERILOG_SRCS := $(wildcard vsrc/*.v)
SIM_MAIN     := csrc/main.cpp
BUILD_DIR    := obj_dir
SIM_EXE      := $(BUILD_DIR)/V$(TOP)
WAVE_FILE := waveform.vcd

# NVBoard 相关
NXDC_FILE := constr/top.nxdc
SRC_AUTO_BIND := $(BUILD_DIR)/auto_bind.cpp

# 生成 auto_bind.cpp 的规则
$(SRC_AUTO_BIND): $(NXDC_FILE)
	python3 $(NVBOARD_HOME)/scripts/auto_pin_bind.py $^ $@

# NVBoard 需要的文件加入 CSRCS
NVBOARD_CSRCS := $(SRC_AUTO_BIND)

# 编译选项
VERILATOR_FLAGS = -Wall --trace --cc --exe
# =====================================
.PHONY: all build clean run
all:run
	@echo "Write this Makefile by your self."

build: $(SIM_EXE)

$(BUILD_DIR):
	mkdir -p $@

sim: $(SIM_EXE)
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Write this Makefile by your self."
	./$(SIM_EXE)

$(SIM_EXE): $(BUILD_DIR) $(VERILOG_SRCS) $(SIM_MAIN) $(SRC_AUTO_BIND) $(NVBOARD_ARCHIVE)
	$(VERILATOR) --cc --trace $(TOP).v --exe \
		$(SIM_MAIN) $(NVBOARD_CSRCS) \
		-CFLAGS "-Wall -g $(CXXFLAGS)" \
		-LDFLAGS "$(LDFLAGS)" \
		--Mdir $(BUILD_DIR) --build -o V$(TOP)

clean:
	rm -rf $(BUILD_DIR) *.vcd

# 包含 NVBoard 基础规则
include $(NVBOARD_HOME)/scripts/nvboard.mk

include ../Makefile
