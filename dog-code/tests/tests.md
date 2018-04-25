Tests are run via the serial port, coummunicating with **test.py**.

Each test is comprised of a *.dog* file and a *.expected* (the *.dog* file will usually be derived from a *.ass* assembly file).

The form of the *.dog* file is :

<pre>
TEST
...
*...operations to test...*
...
DUMP
</pre>

When loaded, a program beginning with the TEST opcode will run automatically.

The DUMP opcode will cause a JSON representation of the current state of DOG-1's to be passed over the serial connection to **test.py**. This is compared with the corresponding *.expected* file.

The JSON sent from DOG-1 will look something like:

<pre>
{
"pc": 5,
"status": 48,
"accA": 18,
"accB": 239,
"xReg": 0,
"pcStackP": 0,
"xStackP": 0
}
</pre>

Whereas the JSON in the *.expected* file will look something like:

<pre>
{
"pc": -1,
"status": 48,
"accA": 18,
"accB": -1,
"xReg": -1,
"pcStackP": -1,
"xStackP": -1
}
</pre>

The comparison will ignore any items with the value -1. So in this case only the status flags and the contents of accumulator A are significant, and as they match, such a test would have been passed.
