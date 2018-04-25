#!/usr/bin/env python
import argparse
import sys
import os.path
import serial
import threading
import time


port = '/dev/ttyACM0'
port1 = '/dev/ttyACM1'

if not os.path.exists(port):
    port = port1

infile = "prog.txt"

args = argparse.ArgumentParser(description='Minimal assembler for DOG-1.')
args.add_argument('-i','--input-file', action="store", dest='input')
args.add_argument('-p','--port', action="store", dest='port')
## args.sqlite_file.name
args_dict = vars(args.parse_args())
if args_dict["input"]:
    infile = args_dict["input"]
if args_dict["port"]:
    port = args_dict["port"]

# print port



ser = serial.Serial(port, 9600)
print "Sleeping while Arduino reboots..."
time.sleep(4)

data = ""
startMarker = 60 # <
endMarker = 62 # >

data = data + "<"

with open(infile, "r") as ins:
    for line in ins:
        data = data + line[:2].rstrip()

data = data + ">"

ser.write(data)
# ser.write(data.encode('utf-8'))

ser.close()
sys.stdout.write(data.encode('utf-8'))

#
# for line in data:
#     time.sleep(.1)
#     ser.write(line.encode('utf-8'))
#     sys.stdout.write(line.encode('utf-8'))





# with open("prog.txt", "r") as ins:
#     ser.write(startMarker);
#     for line in ins:
#         data = line[:2].rstrip()
#         ser.write(data.encode('utf-8')) # first 2 chars encoded as bytes
#         # ser.flush()
#         # time.sleep(.1)
#         sys.stdout.write(data)
#     ser.write(endMarker);
