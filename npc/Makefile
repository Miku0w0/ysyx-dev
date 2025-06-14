all:
	@echo "Write this Makefile by your self."

sim:obj_dir/Vtop
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo "Write this Makefile by your self."
	./obj_dir/Vtop

obj_dir/Vtop: top.v sim.cpp
	verilator --cc --trace top.v --exe sim.cpp --build
include ../Makefile
