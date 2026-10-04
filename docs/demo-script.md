# Demo Recording Script

## 1. Build

```bash
cmake -S . -B build
cmake --build build
```

## 2. Find main

```bash
nm -n build/test_program | grep ' main$'
```

Copy the printed address.

## 3. Run debugger

```bash
./build/debugger
```

Then:

```text
run ./build/test_program
break <main-address>
continue
registers
step
registers
memory <stack-address> 2
delete <main-address>
quit
```

## 4. Attach demo

Start the test program in another terminal, get its PID with `pgrep`, and use:

```text
attach <pid>
registers
step
quit
```
