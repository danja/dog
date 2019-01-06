# DOG-1
'Danny's Obtuse Gadget'

![DOG-1](https://github.com/danja/dog/blob/master/docs/dog-1.JPG?raw=true)

Playing with a cheapo TM1638 card driven by an Arduino Uno. Trying to put together a 1970's-style single-board computer a bit like the KIM-1, except with two boards and my own instruction set etc.

I'm writing up the [DOG-1 Manual](https://github.com/danja/dog/blob/master/docs/manual.md) as I go along. I did a short intro video early on : [DOG-1 Intro](https://www.youtube.com/watch?v=qjk-y1qbj7w).

### Status

**2019-01-05** : picking up the project again after a long break. My main dev machine packed up, so I've been forced to set things up from scratch on an old laptop. First issue, the Standard C++ libraries I was using no longer work  with the latest Arduino libs. Luckily I found the [ArduinoSTL](https://github.com/mike-matera/ArduinoSTL), which (after a tweak to the headers) worked straight away.

There's nothing like returning to a project after a break to highlight bugs and gaps in documentation. On the former, the flow control still isn't quite right. Stuff will run ok, but it can take a bit of keypad fumbling. I also see I haven't yet fixed the problem of loading long Dog programs, and there are definite issues with some of the opcodes I've implemented, notably the branches. I guess I'll just had to plod through, building tests. It's a bit embarrasing for someone who's been coding for 40 years to admit, but I've been having to review [2's Complement Arithmetic](https://www.cs.cornell.edu/~tomf/notes/cps104/twoscomp.html)...

Regarding documentation, I did struggle to get things going from my current notes. Need a quickstart doc.

On a general point, while I've found the Arduino IDE pretty much usable so far, it does seem a bit clunky for use with anything but off-the-shelf libs, so once I've caught up with current issues, I plan to move over to [PlatformIO](https://platformio.org/).

I want to use [ESP32](https://www.espressif.com/en/products/hardware/esp32/overview) cards in other projects, so I may well have a go at porting Dog across, and web-enabling it.

2018-07-16 : looking at the thing again after a long break, looks like last time I committed a broken version - a typo meant it wouldn't even compile. Oops! Fixed that, tried the TONE [Bach demo](https://github.com/danja/dog/blob/master/dog-code/bach.ass), it locked up on loading, although a shorter program would upload ok. Also flipping back from Run to Program mode seems buggy. Annoying. My guess is timing errors in both cases. I think I'm using a different Uno board than before, so maybe fluked it last time. (Although I just saw a commit comment saying I had loadToEEPROM issues - dunno if I corrected those). Need to reread my notes...

2018-04-24 : have been setting up a test harness. Had a lot of trouble with serial (again), but right now it seems stable enough. The testing isn't quite there yet - it works fine for a single test, but not when trying more than one. The PC seems to have a mind of it's own...

2018-04-24 : today added the opcode TONE <note> <duration>. That in itself was pretty trivial (although I do need to finesse the duration a bit). What wasn't so straightforward was that on trying a longer program that I have before, it showed up a major problem with my serial interface code (for uploading programs via Python on the laptop). It was failing after 32 instructions - turns out the Arduino Uno has a 64 byte buffer, and it's really easy to make mistakes. Had to rework the whole interface. But I finally managed to get it going, and so made another little video : [DOG-1 Bachs!](https://youtu.be/eEgXBOtdvvg).
Based around assembler like this:

<pre>
TONE 13 04
REST 04
HALT
</pre>

2018-04-22 : to try a 'proper' program, filled in the opcodes to support this - the Galois pseudorandom number generator as described in [Wikipedia](https://en.wikipedia.org/wiki/Linear-feedback_shift_register#Galois_LFSRs).

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
