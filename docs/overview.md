# DOG-1

*Danny's Obtuse Gadget, version 1.0*

(I'm writing this as if the thing was finished - far from it! - So this is all provisional design)

A very limited computer based on an Arduino Uno and a TM1638 I/O card. Runs as a virtual machine on top of the Arduino. Inspired by the single-board computers of the mid-1970's notably the [MK14](https://en.wikipedia.org/wiki/MK14) and [KIM-1](https://en.wikipedia.org/wiki/KIM-1). The architecture and instruction set is loosely derived from the processors used on these machines, the [SC/MP](https://en.wikipedia.org/wiki/National_Semiconductor_SC/MP) and [6502](https://en.wikipedia.org/wiki/MOS_Technology_6502) respectively.

Some novelty is offered by the hardware used is that it should be possible to interface with the built-in I/O of the Arduino. So eg. a digital thermometer could be made by attaching the appropriate sensor and programming the DOG-1 directly, without access to a PC.

## Basic Architecture

* 16-bit addressing
* 8-bit instructions
* 8-bit data

### Registers

Operations will be carried out using the following registers:

**16-Bit**
* Program Counter (PC) - steps through program
* Pointer Register (PR) - an auxiliary register

**8-Bit**
* Accumulator - primary
* X and Y - secondary
* Status Register (SR) - system flags
* Stack Pointer (SP) - for remembering the origin the source of subroutine jumps

**Flags**

| Bit | Flag | Name | Description |
|-----|------|------|-------------|
| 0   | N    | Negative | Set if bit 7 of ACC is set |
| 1   | V    | Overflow | - |
| 2   | Z    | Zero | - |
| 3   | C    | Carry | - |

**Addressing Modes**

nn = 2 hex digits
nnnn = 4 hex digits

| Mode | Assembler Format | Description |
|------|------------------|-------------|
| Immediate | #nn | Value is given immediately after opcode |
| Absolute | nnnn | Value is contained in the given address |
| Indirect Absolute | (nnnn) | - |
| Absolute Indexed, X | nnnn, X | - |
| Absolute Indexed, Y | nnnn, Y | - |
| Relative | nnnn | - |


## I/O

#### TM1638 Card

|         | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
|---------|---|---|---|---|---|---|---|---|
| LEDs    | * | * | * | * | * | * | * | * |
|         |   |   |   |   |   |   |   |   |
| 7-Segs  | 8 | 8 | 8 | 8 | 8 | 8 | 8 | 8 |
|         |   |   |   |   |   |   |   |   |
| Buttons | o | o | o | o | o | o | o | o |

The LEDs will usually display the contents of the Status Register.

Generally, the 7-Segment displays 0-3 will display the current value (address) of the Program Counter. Displays 6 and 7 showing the contents at that address. All in hexadecimal.

The functionality of the I/O will depend on the system's mode : **Program** or **Run**. Display 4 shows the current mode, P or R.
Display 5 is unused (TBD).

Push-button 4 switches between these modes.
At any time, pressing buttons 4 and 5 **together** will reset the PC to 0000.

### Program Mode

Pressing the buttons 0-3, 6-7 will increment the value corresponding to that of the display above it. Programming is achieved by pressing button 3 to increment the PC (with overflow occurring, counting up on displays 0-2). Pressing button 7 will increment the value on display 7 (*without* overflowing to display 6), ditto for button 6/display 6, together providing the value at the given address.

Pressing button 4 again will switch to *Run* mode.

### Run Mode

Initially the system will be halted at the current address. Pressing button 3 will single-step through the program (pressing buttons 0-3 will cause the PC to skip to the corresponding address [running or skipping code in between? TBD]).

Alternately the program may be run in real time by pressing button 5. Pressing this button again will halt the program.

## Instruction Set

*note to self* - things like LDA will have a version for each of the addressing modes, ~ 6, so it's probably an idea to hop 8 values between base versions...hmm, testing values for switch statements via masks?

Using pointer register - probably mainly for table lookup, maybe for subroutine-like things too
should support ld, st, inc & dec, swap with PC, conditional swap

| Instruction | OpCode | Size | Operation | Description |
| ----------- | ------ | ---- | --------- | ----------- |
| NOP         | 00     | 1    | PC++      | No operation |
| CLF         | 00     | 1    | PC++      | No operation |
| LDA #nn     | 10     | 2    | ACC <- nn | Load accumulator, immediate |
| LDA  nnnn     | 11     | 3    | ACC <- (nn) | Load accumulator from value at address |
| STA  nnnn     | 18     | 3    | ACC -> (nn) | Store accumulator at given address |
| JMP nnnn      | 20     | 3    | PC <- nn  | jump to given address |
| JNZ nnnn      | 20     | 3    | PC <- nn  | jump to given address |
| SPC nnnn      | 28     | 3    | PC -> (nn)  | store program counter value at address |
| AND #nn        | 30     | 1    | Halt      | Stops program flow |
| AND nnnn        | 30     | 1    | Halt      | Stops program flow |
| OR #nn        | 30     | 1    | Halt      | Stops program flow |
| OR nnnn        | 30     | 1    | Halt      | Stops program flow |
| XOR #nn        | 30     | 1    | Halt      | Stops program flow |
| XOR nnnn        | 30     | 1    | Halt      | Stops program flow |
| ROL         | 30     | 1    | Halt      | Stops program flow |
| ROR         | 30     | 1    | Halt      | Stops program flow |
| ADD #nn         | FF     | 1    | Halt      | Stops program flow |
| ADD nnnn         | FF     | 1    | Halt      | Stops program flow |
| SUB #nn         | FF     | 1    | Halt      | Stops program flow |
| SUB nn         | FF     | 1    | Halt      | Stops program flow |
| CMP          | FF     | 1    | Halt      | Stops program flow |
| HLT         | FF     | 1    | Halt      | Stops program flow |
| HLT         | FF     | 1    | Halt      | Stops program flow |
| HLT         | FF     | 1    | Halt      | Stops program flow |
| HLT         | FF     | 1    | Halt      | Stops program flow |
| HLT         | FF     | 1    | Halt      | Stops program flow |
