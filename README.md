# DOG-1
'Danny's Obtuse Gadget'

Playing with a cheapo TM1638 card driven by an Arduino Uno. Trying to put together a simple single-board computer a bit like the Kim-1, except with two boards and my own instruction set etc.

If written up the [design/manual](https://github.com/danja/dog/blob/master/docs/overview.md) as far as instruction set (still a lot to be decided around there - will probably make most of that up as I go along).

*Status*  
2018-03-29 (10pm) : implemented simple serial transfer of DOG-1 programs from computer USB/serial port, with very crude python script. (Just enough for testing opcodes).
2018-03-29 (3am!) : implemented a handful of instructions - got a first program running! So chuffed I did a [VIDEO](https://youtu.be/qjk-y1qbj7w)

2018-03-28 : refactored, TM1638 interface bits moved to a separate library - https://github.com/danja/TM1638lite

2018-03-27 : UI, program input implemented.
