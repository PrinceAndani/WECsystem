#include <iostream>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

using namespace std;

void print_registers(pid_t child)
{
    user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, child, nullptr, &regs) == -1)
    {
        perror("ptrace getregs");
        return;
    }

    cout << hex;

    cout << "RIP: 0x" << regs.rip << '\n';
    cout << "RSP: 0x" << regs.rsp << '\n';
    cout << "RBP: 0x" << regs.rbp << '\n';
    cout << "RAX: 0x" << regs.rax << '\n';

    cout << dec;
}

int main()
{
    pid_t child = fork();

    if (child == -1)
    {
        perror("fork");
        return 1;
    }

    if (child == 0)
    {
        if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) == -1)
        {
            perror("ptrace");
            return 1;
        }

        execl("./tests/test_program", "./tests/test_program", nullptr);

        perror("execl");
        return 1;
    }

    int status;
    waitpid(child, &status, 0);

    if (!WIFSTOPPED(status))
    {
        cerr << "Child did not stop correctly\n";
        return 1;
    }

    cout << "Initial registers:\n";
    print_registers(child);

    if (ptrace(PTRACE_SINGLESTEP, child, nullptr, nullptr) == -1)
    {
        perror("ptrace single step");
        return 1;
    }

    waitpid(child, &status, 0);

    cout << "\nAfter one instruction:\n";
    print_registers(child);

    if (ptrace(PTRACE_CONT, child, nullptr, nullptr) == -1)
    {
        perror("ptrace continue");
        return 1;
    }

    waitpid(child, &status, 0);

    if (WIFEXITED(status))
    {
        cout << "\nChild exited with code: "
             << WEXITSTATUS(status) << '\n';
    }

    return 0;
}