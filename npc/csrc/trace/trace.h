#ifndef __TRACE_H__
#define __TRACE_H__

#include <cstdint>

// 全局开关
extern bool itrace_enable;
extern bool mtrace_enable;
extern bool ftrace_enable;

// 初始化
void itrace_set_enable(bool enable);
void mtrace_set_enable(bool enable);
void ftrace_set_enable(bool enable);

// 记录函数
void itrace_record(uint32_t pc, uint32_t inst);
void ftrace_record(uint32_t pc, uint32_t inst);
void mtrace_record();

// ftrace 指令判断
bool is_call_inst(uint32_t inst, uint32_t pc, uint32_t &target);
bool is_ret_inst(uint32_t inst);

#endif