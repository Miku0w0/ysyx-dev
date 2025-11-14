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

$(SIM_EXE): $(BUILD_DIR) $(VERILOG_SRCS) $(SIM_MAIN)
	$(VERILATOR) --cc --trace $(TOP).v --exe $(SIM_MAIN) --build -CFLAGS "-Wall -g" -Mdir $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) *.vcd
	
include ../Makefile
