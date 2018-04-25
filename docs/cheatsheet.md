## DOG-1 USER Interface Cheatsheet

|         | 0   | 1   | 2   | 3   | 4   | 5   | 6   | 7   |
| :------ |:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| **LEDs** |  Negative  |  Overflow  |  Zero   |  Carry  | \*  | \*  | \*  | \*  |
| **7-Segs** | - | - | Program- | Counter | *Mode* | *Inc/Dec* | Op- | Code |
| **Buttons** | o   | o   | o   | o   | o   | o   | o   | o   |


**Flags**

| Bit | Flag | Name     | Description                |
| --- | ---- | -------- | -------------------------- |
| 0   | N    | Negative | Set if bit 7 of ACC is set |
| 1   | V    | Overflow | -                          |
| 2   | Z    | Zero     | -                          |
| 3   | C    | Carry    | -                          |
| 4-7 | X    | Aux      | -                          |

The functionality of the I/O will depend on the system's mode : **Program** or **Run**. Display 4 shows the current mode, P or R.

Push-button 4 switches between these modes.
At any time, pressing buttons 4 and 5 **together** will reset the PC to 0000.

### Program Mode

Pressing the buttons 0-3, 6-7 will increment the value corresponding to that of the display above it. Programming is achieved by pressing button 3 to increment the PC (with overflow occurring, counting up on displays 0-2). Pressing button 7 will increment the value on display 7 (_without_ overflowing to display 6), ditto for button 6/display 6, together providing the value at the given address.

Pressing button 5 will switch the response from increment to decrement. The PC buttons/display _does_ carry values and wrap at max and min (0000). The code buttons/display act independently to each other and don't wrap in the &lt;0 direction.

Pressing button 4 will switch to **Run** mode.

#### Double Key Presses

* 0 & 1 - full-on reset & wipe
* 4 & 5 - reset pc
* 0 & 4 - display Accumulators A, B
* 0 & 5 - display Index Register
* 0 & 6 - display PC Stack Pointer
* 0 & 7 - display Auxiliary Stack Pointer & status

* 0 & 3 - flip from single-step to free run

### Run Mode

Initially the system will be halted at the current address. Pressing button 3 will single-step through the program (pressing buttons 0-3 will cause the PC to skip to the corresponding address [running or skipping code in between? TBD]).

Alternately the program may be run in real time by pressing button 5. Pressing this button again will halt the program.

The HALT opcode will terminate a program and wait for keyboard input before switching to Program mode and zeroing the program counter.

#### Special Instructions

* Pause

If the instruction PAUSE is encountered in a program, the program will freeze at this point at display 'PAUSE...'.
The flags and registers maynow be inspected. Pressing key 4 sets the program running again.
