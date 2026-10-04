#include "debugger.hpp"

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

Debugger::Debugger() : pid(-1) {}

void Debugger::wait_for_signal() {
    int status = 0;
    waitpid(pid, &status, 0);

    if (WIFEXITED(status)) {
        cout << "Program exited with code " << WEXITSTATUS(status) << "\n";
        pid = -1;
    } else if (WIFSIGNALED(status)) {
        cout << "Program terminated by signal " << WTERMSIG(status) << "\n";
        pid = -1;
    } else if (WIFSTOPPED(status)) {
        cout << "Program stopped by signal " << WSTOPSIG(status) << "\n";
    }
}

Breakpoint* Debugger::find_breakpoint(uintptr_t address) {
    for (auto& breakpoint : breakpoints) {
        if (breakpoint.get_address() == address) {
            return &breakpoint;
        }
    }

    return nullptr;
}

void Debugger::handle_breakpoint() {
    if (pid == -1) {
        return;
    }

    user_regs_struct regs{};
    if (ptrace(PTRACE_GETREGS, pid, nullptr, &regs) == -1) {
        perror("PTRACE_GETREGS");
        return;
    }

    uintptr_t breakpoint_address = regs.rip - 1;
    Breakpoint* breakpoint = find_breakpoint(breakpoint_address);

    if (breakpoint == nullptr) {
        return;
    }

    regs.rip = breakpoint_address;
    if (ptrace(PTRACE_SETREGS, pid, nullptr, &regs) == -1) {
        perror("PTRACE_SETREGS");
        return;
    }

    breakpoint->disable();

    if (ptrace(PTRACE_SINGLESTEP, pid, nullptr, nullptr) == -1) {
        perror("PTRACE_SINGLESTEP");
        return;
    }

    wait_for_signal();

    if (pid != -1) {
        breakpoint->enable();
    }
}

void Debugger::run(const string& program) {
    pid = fork();

    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == -1) {
            perror("PTRACE_TRACEME");
            exit(1);
        }

        execl(program.c_str(), program.c_str(), nullptr);
        perror("execl");
        exit(1);
    }

    wait_for_signal();
    cout << "Started process with pid " << pid << "\n";
}

void Debugger::attach(pid_t target_pid) {
    if (ptrace(PTRACE_ATTACH, target_pid, nullptr, nullptr) == -1) {
        perror("PTRACE_ATTACH");
        return;
    }

    pid = target_pid;
    wait_for_signal();
    cout << "Attached to process " << pid << "\n";
}

void Debugger::step() {
    if (pid == -1) {
        cout << "No process\n";
        return;
    }

    handle_breakpoint();

    if (pid == -1) {
        return;
    }

    if (ptrace(PTRACE_SINGLESTEP, pid, nullptr, nullptr) == -1) {
        perror("PTRACE_SINGLESTEP");
        return;
    }

    wait_for_signal();
}

void Debugger::continue_execution() {
    if (pid == -1) {
        cout << "No process\n";
        return;
    }

    handle_breakpoint();

    if (pid == -1) {
        return;
    }

    if (ptrace(PTRACE_CONT, pid, nullptr, nullptr) == -1) {
        perror("PTRACE_CONT");
        return;
    }

    wait_for_signal();
}

void Debugger::set_breakpoint(uintptr_t address) {
    if (pid == -1) {
        cout << "No process\n";
        return;
    }

    if (find_breakpoint(address) != nullptr) {
        cout << "Breakpoint already exists\n";
        return;
    }

    breakpoints.emplace_back(pid, address);
    breakpoints.back().enable();
    cout << "Breakpoint set at 0x" << hex << address << dec << "\n";
}

void Debugger::delete_breakpoint(uintptr_t address) {
    for (auto it = breakpoints.begin(); it != breakpoints.end(); ++it) {
        if (it->get_address() == address) {
            it->disable();
            breakpoints.erase(it);
            cout << "Breakpoint deleted\n";
            return;
        }
    }

    cout << "Breakpoint not found\n";
}

void Debugger::print_registers() {
    if (pid == -1) {
        cout << "No process\n";
        return;
    }

    user_regs_struct regs{};
    if (ptrace(PTRACE_GETREGS, pid, nullptr, &regs) == -1) {
        perror("PTRACE_GETREGS");
        return;
    }

    cout << hex;
    cout << "RIP = 0x" << regs.rip << '\n';
    cout << "RSP = 0x" << regs.rsp << '\n';
    cout << "RBP = 0x" << regs.rbp << '\n';
    cout << "RAX = 0x" << regs.rax << '\n';
    cout << "RBX = 0x" << regs.rbx << '\n';
    cout << "RCX = 0x" << regs.rcx << '\n';
    cout << "RDX = 0x" << regs.rdx << '\n';
    cout << "RSI = 0x" << regs.rsi << '\n';
    cout << "RDI = 0x" << regs.rdi << '\n';
    cout << dec;
}

void Debugger::print_memory(uintptr_t address, int count) {
    if (pid == -1) {
        cout << "No process\n";
        return;
    }

    for (int i = 0; i < count; ++i) {
        uintptr_t current = address + i * sizeof(long);
        errno = 0;
        long data = ptrace(PTRACE_PEEKDATA, pid, current, nullptr);

        if (errno != 0) {
            perror("PTRACE_PEEKDATA");
            return;
        }

        cout << "0x" << hex << current << " : 0x" << data << dec << '\n';
    }
}
