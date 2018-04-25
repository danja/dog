#!/usr/bin/env python
import sys
import argparse

src = "../src/dog-1/dog-1.ino"

infile = "test.txt"
outfile = "out.txt"

codes_dump = "../docs/opcodes.md"

start = "START OPCODES"
end = "END OPCODES"
define = "#define"

dict = {}
hex = []
codes = []
comments = []

args = argparse.ArgumentParser(description='Minimal assembler for DOG-1.')
args.add_argument('-i','--input-file', action="store", dest='input')
args.add_argument('-o','--output-file', action="store", dest='output')
## args.sqlite_file.name
args_dict = vars(args.parse_args())
if args_dict["input"]:
    infile = args_dict["input"]
if args_dict["output"]:
    outfile = args_dict["output"]

# read source file, make dictionary from opcodes
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
            comment_marker = line.find('//')
            if comment_marker != -1:
                comment = line[comment_marker+2:]
                # print comment
            else:
                comment = ""
            dict.update({chunks[1] : chunks[2][2:]})
            hex.append(chunks[2])
            codes.append(chunks[1])
            comments.append(comment)

# save the list of opcodes to file
def dump_codes():
    with open(codes_dump, "w") as f:
    # LATER    f.write("| Inst | Syntax | Mode | Size |C|Z|V|N| Symbolic | Description |\n")
    # LATER    f.write("| ---- | ------ | ---- | ---- |-|-|-|-| -------- | ----------- |\n")
        # for code in codes:
        for i in range(0, len(codes)):
    # LATER        f.write("| "+codes[i]+" | "+codes[i]+" | ---- | ---- |-|-|-|-| -------- | "+comments[i]+" |\n")
            f.write(hex[i]+ " " +codes[i]+"  "+comments[i]+"  \n") # note two spaces for line break

        # print chunks[1]+" "+chunks[2]

# LDAi 66 ; put 0x66 in acc A
# STAa 07 00 ; store acc A at 0070
# HALT

 # assemble program
def assemble():
    with open(infile, "r") as ins:
        target = open(outfile,"w")
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
                    target.write(dict[chunk] + " " + line.strip()+"\n")
                    # print dict[chunk] + " " + line.strip()
                else:
                    if chunk != "\n":
                        target.write(chunk+"\n")
                        # print chunk
        target.close()

load_dictionary()
dump_codes()
# print dict["LDAi"]
assemble()
