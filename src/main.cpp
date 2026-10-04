#include "debugger.hpp"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

int main() {
    Debugger debugger;
    string line;

    while (true) {
        cout << "debugger> ";
        if (!getline(cin, line)) {
            break;
        }

        stringstream input(line);
        string command;
        input >> command;

        if (command == "run") {
            string program;
            input >> program;
            debugger.run(program);
        } else if (command == "attach") {
            pid_t pid;
            input >> pid;
            debugger.attach(pid);
        } else if (command == "step") {
            debugger.step();
        } else if (command == "continue") {
            debugger.continue_execution();
        } else if (command == "break") {
            string value;
            input >> value;
            uintptr_t address = stoull(value, nullptr, 16);
            debugger.set_breakpoint(address);
        } else if (command == "delete") {
            string value;
            input >> value;
            uintptr_t address = stoull(value, nullptr, 16);
            debugger.delete_breakpoint(address);
        } else if (command == "registers") {
            debugger.print_registers();
        } else if (command == "memory") {
            string address_text;
            int count;
            input >> address_text >> count;
            uintptr_t address = stoull(address_text, nullptr, 16);
            debugger.print_memory(address, count);
        } else if (command == "quit") {
            break;
        }
    }

    return 0;
}
