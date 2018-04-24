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

print port

# sort out later
# parser = argparse.ArgumentParser()


ser = serial.Serial(port, 9600)
print "Sleeping while Arduino reboots..."
time.sleep(4)

data = ""
startMarker = 60 # <
endMarker = 62 # >

data = data + "<"

with open("prog.txt", "r") as ins:
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
