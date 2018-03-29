import sys
import os.path
import serial
import threading

port = '/dev/ttyACM0'
port1 = '/dev/ttyACM1'

if not os.path.exists(port):
    port = port1

print port

ser = serial.Serial(port, 9600)

line_number = 0

with open("prog.txt", "r") as ins:
    for line in ins:
        if line_number != 0:
            data = line[:2] # first two chars on line
        else:
            data = line[:4].rstrip() # INIT
        line_number = line_number + 1
#        ser.write(data)
        sys.stdout.write(bytearray(data))


def read_from_port(ser):
            print("test")
            # reading = ser.readline().decode()
            print ser.read()

# thread = threading.Thread(target=read_from_port, args=(ser,))
# thread.start()
