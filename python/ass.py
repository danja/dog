#!/usr/bin/env python
import sys

src = "../src/dog-1/dog-1.ino"

prog = "test.txt"

codes_dump = "opcodes.txt"

start = "START OPCODES"
end = "END OPCODES"
define = "#define"

dict = {}
codes = []

def load_dictionary():
    in_opcodes = False
    with open(src, "r") as ins:
        for line in ins:
            if start in line:
                in_opcodes = True
            if end in line:
                in_opcodes = False
            if not in_opcodes:
                continue
            if not define in line:
                continue
            line = " ".join(line.split()) # normalise whitespace
            chunks = line.strip().split(" ")
            dict.update({chunks[1] : chunks[2][2:]})
            codes.append(chunks[1])

def dump_codes():
    with open(codes_dump, "w") as f:
        f.write("| Inst | Syntax | Mode | Size |C|Z|V|N| Symbolic | Description |\n")
        f.write("| ---- | ------ | ---- | ---- |-|-|-|-| -------- | ----------- |\n")
        for code in codes:
            f.write("| "+code+" | ------ | ---- | ---- |-|-|-|-| -------- | ----------- | "+"\n")

        # print chunks[1]+" "+chunks[2]

# LDAi 66 ; put 0x66 in acc A
# STAa 07 00 ; store acc A at 0070
# HALT

def do_codes():
    with open(prog, "r") as ins:
        for line in ins:
            # strip comments
            comment = line.find(';')
            if comment != -1:
                cutline = line[:comment]
            else:
                cutline = line.strip()
            # print line
            #
            cutline = " ".join(cutline.split()) # normalise whitespace
            chunks = cutline.split(" ")
            for chunk in chunks:
                chunk = chunk.strip()
                if chunk in dict.keys():
                    print dict[chunk] + " " + line.strip()
                else:
                    if chunk != "\n":
                        print chunk

load_dictionary()
dump_codes()
# print dict["LDAi"]
do_codes()
