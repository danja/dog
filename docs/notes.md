# Design Considerations

6800 has 72 instructions with seven addressing modes for a total of 197 opcodes.
The 6800 architecture was modeled after the DEC PDP-11 processor.

In computer engineering, an orthogonal instruction set is an instruction set architecture where all instruction types can use all addressing modes. It is "orthogonal" in the sense that the instruction type and the addressing mode vary independently. An orthogonal instruction set does not impose a limitation that requires a certain instruction to use a specific register.

https://en.wikipedia.org/wiki/PDP-11_architecture#General_register_addressing_modes

https://en.wikipedia.org/wiki/Addressing_mode

see SKIP

ROR #n

rotate the status, then allow instruction skip if aux flag set

The direct addressing mode allowed fast access to the first 256 bytes of memory. I/O devices were addressed as memory so there were no special I/O instructions.


Early on :

Sketch uses 2884 bytes (8%) of program storage space. Maximum is 32256 bytes.

Global variables use 178 bytes (8%) of dynamic memory, leaving 1870 bytes for local variables. Maximum is 2048 bytes.

with PROGMEM

Sketch uses 2884 bytes (8%) of program storage space. Maximum is 32256 bytes.

Global variables use 50 bytes (2%) of dynamic memory, leaving 1998 bytes for local variables. Maximum is 2048 bytes.

https://www.arduino.cc/reference/en/language/variables/utilities/progmem/

boo! not as straightforward as it first looked, leaving out unless I really need...
