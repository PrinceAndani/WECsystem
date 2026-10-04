#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include "breakpoint.hpp"

#include <sys/types.h>
#include <sys/user.h>
#include <cstdint>
#include <string>
#include <vector>

using namespace std;

class Debugger {
private:
    pid_t pid;
    vector<Breakpoint> breakpoints;

    void wait_for_signal();
    void handle_breakpoint();
    Breakpoint* find_breakpoint(uintptr_t address);

public:
    Debugger();

    void run(const string& program);
    void attach(pid_t target_pid);
    void step();
    void continue_execution();
    void set_breakpoint(uintptr_t address);
    void delete_breakpoint(uintptr_t address);
    void print_registers();
    void print_memory(uintptr_t address, int count);
};

#endif
