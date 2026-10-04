#ifndef BREAKPOINT_HPP
#define BREAKPOINT_HPP

#include <sys/types.h>
#include <cstdint>

using namespace std;

class Breakpoint {
private:
    pid_t pid;
    uintptr_t address;
    long original_data;
    bool enabled;

public:
    Breakpoint(pid_t pid, uintptr_t address);

    void enable();
    void disable();

    uintptr_t get_address() const;
    bool is_enabled() const;
};

#endif
