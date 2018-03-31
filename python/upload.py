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
time.sleep(5)


with open("prog.txt", "r") as ins:
    for line in ins:
        data = line[:2].rstrip()
        ser.write(data.encode('utf-8')) # first 2 chars encoded as bytes
        sys.stdout.write(data)
