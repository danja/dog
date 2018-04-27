#!/usr/bin/env python
import argparse
import sys
import os.path
import serial
import threading
import time
import os
import json

port = '/dev/ttyACM0'
port1 = '/dev/ttyACM1'

if not os.path.exists(port):
    port = port1

tests_dir = "../dog-code/tests"
test_extn = ".dog"
expected_extn = ".expected"

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

ser = serial.Serial(port, 9600)

# TODO replace with handshake
print "Sleeping while Arduino reboots..."
time.sleep(4)

# --------------------------
startMarker = 60 # <
endMarker = 62 # >


def do_upload(infile):
    data = ""
    data = data + "<"

    with open(infile, "r") as ins:
        for line in ins:
            data = data + line[:2].rstrip()

    data = data + ">"
    ser.write(data)
    ser.flush();

for file in sorted(os.listdir(tests_dir)):
    if file.endswith(test_extn):
        print "\n\nTest : "+file[:len(file)-len(test_extn)]+"\n"
        test_file = os.path.join(tests_dir, file)
        do_upload(test_file)

        receiving = False
        while not receiving:
            char = ser.read()
            if(char == '<'):
                receiving = True
        result =""
        while receiving:
            char = ser.read()
            sys.stdout.write(char)

            if char != '>':
                result = result + char
            else:
                receiving = False
                print "Result = "+result
                # ser.flush()
                # time.sleep(.5)
                result_json = json.loads(result)
                expected_file = test_file[:len(test_file)-len(test_extn)]+expected_extn
                print "loading "+expected_file
                expected = open(expected_file, "r")
                expected_json = json.loads(expected.read())
                success = True
                for key in expected_json:
                    if(expected_json[key]==-1):
                        continue
                    if(expected_json[key] != result_json[key]):
                        success = False
                        print "*** FAIL! ***"
                        print "Expected : "+key+" = "+str(expected_json[key])
                        print "Got : "+key+" = "+str(result_json[key])
                if(success):
                    print "Passed."

            # time.sleep(2)




# ser.write(data.encode('utf-8'))

ser.close()

# sys.stdout.write(data.encode('utf-8'))
