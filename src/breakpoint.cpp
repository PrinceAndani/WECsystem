#include "breakpoint.hpp"

#include <sys/ptrace.h>
#include <cerrno>
#include <cstdio>

Breakpoint::Breakpoint(pid_t pid, uintptr_t address)
    : pid(pid), address(address), original_data(0), enabled(false) {}

void Breakpoint::enable() {
    if (enabled) {
        return;
    }

    errno = 0;
    long data = ptrace(PTRACE_PEEKDATA, pid, address, nullptr);
    if (errno != 0) {
        perror("PTRACE_PEEKDATA");
        return;
    }

    original_data = data;
    long breakpoint_data = (data & ~0xffL) | 0xcc;

    if (ptrace(PTRACE_POKEDATA, pid, address, breakpoint_data) == -1) {
        perror("PTRACE_POKEDATA");
        return;
    }

    enabled = true;
}

void Breakpoint::disable() {
    if (!enabled) {
        return;
    }

    if (ptrace(PTRACE_POKEDATA, pid, address, original_data) == -1) {
        perror("PTRACE_POKEDATA");
        return;
    }

    enabled = false;
}

uintptr_t Breakpoint::get_address() const {
    return address;
}

bool Breakpoint::is_enabled() const {
    return enabled;
}
