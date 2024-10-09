# ISA

This project simulates the operation of a custom RISC-based CPU named `SIMP`, along with various input/output devices like LEDs, a 7-segment display, a disk drive, and a monochromatic monitor. The project involves writing both an **assembler** and a **simulator** for this CPU, using C programming language. It also includes several assembly programs that will run on this simulated environment.

## Project Structure

### Components

1. **Assembler**: Written in C, the assembler translates assembly language programs into machine code suitable for the `SIMP` CPU.
2. **Simulator**: Also written in C, the simulator mimics the behavior of the `SIMP` CPU, including memory operations, I/O operations, and execution of the machine code.

### Input/Output Devices
- **LEDs**: 32 LEDs can be controlled by the CPU.
- **7-Segment Display**: Supports 8 digits, each controlled by 4 bits.
- **Disk Drive**: The disk consists of 128 sectors, with DMA support for reading and writing.
- **Monochrome Monitor**: A 256x256 pixel monochromatic monitor is used for display.

### Files
The following files are used during the execution of the assembler and simulator:

- `memin.txt`: Initial memory content.
- `memout.txt`: Memory content at the end of execution.
- `diskin.txt`: Initial content of the disk.
- `diskout.txt`: Disk content at the end of execution.
- `irq2in.txt`: Specifies external interrupts.
- `regout.txt`: Register values after execution.
- `trace.txt`: Execution trace showing the state of registers after each instruction.
- `leds.txt`: State of the LEDs throughout the execution.
- `display7seg.txt`: 7-segment display output.
- `monitor.txt`: Final pixel values of the monitor.
- `monitor.yuv`: Binary format of the monitor output.

## Assembly Programs

1. **sort.asm**: A bubble sort algorithm that sorts 16 numbers stored in memory.
2. **binom.asm**: Computes binomial coefficients using a recursive approach.
3. **triangle.asm**: Draws a triangle on the monitor.
4. **disktest.asm**: Sums up sectors 0 to 7 of the disk and writes the result into sector 8.

## How to Run

### Assembler
The assembler converts `.asm` files into a machine-readable format.

```bash
./asm.exe program.asm memin.txt
