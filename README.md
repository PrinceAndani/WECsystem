# Simple Linux Debugger

A small debugger written in C++ using the Linux `ptrace` system call.

## Features

This project implements only the required basic features:

- Run a program under debugger control.
- Attach to an existing process.
- Step one instruction.
- Create a breakpoint at a memory address.
- Delete a breakpoint.
- Print registers.
- Print memory.

`continue` is included only so that breakpoints can be reached and tested.

The bonus features (debug symbols, variables, symbol breakpoints, and backtrace) are not implemented.

## Technologies

- C++23
- Linux `ptrace`
- CMake

## Build

From the project directory:

```bash
cmake -S . -B build
cmake --build build
```

This creates:

```text
build/debugger
build/test_program
```

## Run

Start the debugger:

```bash
./build/debugger
```

### Run a program

Inside the debugger:

```text
run ./build/test_program
```

### Show registers

```text
registers
```

### Step one instruction

```text
step
```

### Set a breakpoint

First find the address of `main` in the test program:

```bash
nm -n build/test_program | grep ' main$'
```

Because the demo program is linked without PIE, the address printed by `nm` is the runtime address. Use that address inside the debugger:

```text
break <main-address>
continue
```

The debugger will stop when it reaches that address.

### Delete a breakpoint

```text
delete 0x401136
```

### Print memory

The second value is the number of machine words to print.

```text
memory 0x7fffffffe000 4
```

### Attach to an existing process

Find a process ID:

```bash
pgrep test_program
```

Then:

```text
attach 12345
```

### Exit

```text
quit
```

## How it works

`ptrace` is used for all debugger control.

When `run` is used, the debugger creates a child process. The child calls `PTRACE_TRACEME` and then executes the target program. The parent waits for the child to stop.

For `step`, the debugger uses `PTRACE_SINGLESTEP` and waits for the process again.

For a breakpoint, the debugger reads the original machine word with `PTRACE_PEEKDATA`, replaces its first byte with `0xCC` (the x86-64 software breakpoint instruction), and writes it back with `PTRACE_POKEDATA`. When the breakpoint is hit, the debugger restores the original instruction and single-steps it before putting the breakpoint back.

Registers are read using `PTRACE_GETREGS`.

Memory is read using `PTRACE_PEEKDATA`.

## Simple demonstration

A useful demonstration is:

1. Build the project.
2. Find `main` with `nm`.
3. Start `./build/debugger`.
4. Run `./build/test_program`.
5. Set a breakpoint at `main`.
6. Continue.
7. Print registers.
8. Step one instruction.
9. Print registers again.
10. Print a small memory range.
11. Delete the breakpoint.
12. Quit.

## Limitations

This is intentionally a simple learning project. It targets Linux x86-64 and does not implement symbol loading, variable inspection, disassembly, backtraces, or a full debugger command language.
