# DOG-1
'Danny's Obtuse Gadget'

![DOG-1](https://github.com/danja/dog/blob/master/docs/dog-1.JPG?raw=true)

Playing with a cheapo TM1638 card driven by an Arduino Uno. Trying to put together a 1970's-style single-board computer a bit like the KIM-1, except with two boards and my own instruction set etc.

I'm writing up the [DOG-1 Manual](https://github.com/danja/dog/blob/master/docs/manual.md) as I go along.

### Status  

**2018-04-22** : to try a 'proper' program, filled in the opcodes to support this - the Galois pseudorandom number generator as described in [Wikipedia](https://en.wikipedia.org/wiki/Linear-feedback_shift_register#Galois_LFSRs).

<pre>
LDAi AC ; put 0xAC in acc A
LDBi E1 ; put 0xE1 in acc B
LSR ; shift right (and into carry)
BCC FE; branch -2 if carry clear
EORAi B4 ; toggle mask
EORBi 00
PAUSE
BZC FB ; branch up to the shift
HALT
</pre>

Unfortunately, figuring out the correct branching - using 2's complement jumps back - gets mighty confusing. So right now it's waiting for me to sort out the single-stepping and a suitable test or two.

2018-04-16 : made a mess of my TM1638 lib, took me a while to fix that. Then I got in a mess with my flags. Turned out I'd put in !x rather than ~x for bitwise negation, took me ages to spot. But pretty much back on track. Got this to run ok:

<pre>
LDAi F6 ; put 0xF6 in acc A
STAa 99 01 ; store acc A at 0199
LDBa 99 01  ; load acc B from 0199
CAB ; compare A & B
BZS 01; branch if zero set
ERR ; display Err
OK ; display ok
HALT
</pre>

2018-03-31 : up to about 40 opcodes (untested). Made a minimal assembler in Python, should be enough for testing. (It takes opcode/hex value map directly from defines in the C source, so will be in sync).

2018-03-30 : now implemented 7 instructions for each of acc A & B (refactored to use same routines)

2018-03-30 : some tidying/refactoring. Decided to create a dedicated 8-bit stack (ALU Stack) for experimenting with stack-oriented programming and maths. Added controls (two-button) to allow display of register contents.
(see https://en.wikipedia.org/wiki/Stack-oriented_programming_language https://www.forth.com/starting-forth/2-stack-manipulation-operators-arithmetic/ )

2018-03-29 (10pm) : implemented simple serial transfer of DOG-1 programs from computer USB/serial port, with very crude python script. (Just enough for testing opcodes).

2018-03-29 (3am!) : implemented a handful of instructions - got a first program running! So chuffed I did a [VIDEO](https://youtu.be/qjk-y1qbj7w)

2018-03-28 : refactored, TM1638 interface bits moved to a separate library - https://github.com/danja/TM1638lite

2018-03-27 : UI, program input implemented.
