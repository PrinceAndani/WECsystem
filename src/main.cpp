#include <iostream>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

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

    if (WIFSTOPPED(status))
        cout << "Child stopped. PID: " << child << '\n';
    

    ptrace(PTRACE_CONT, child, nullptr, nullptr);
    waitpid(child, &status, 0);

    if (WIFEXITED(status))
        cout << "Child exited with code: " << WEXITSTATUS(status) << '\n';
    

    return 0;
}