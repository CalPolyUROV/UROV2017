import serial
import serial.tools.list_ports

from pygame.locals import *

from Autopilot import *

import sys

from UIUtils import *

from time import sleep

from controller import *
import serial_finder

from ArtificialHorizon import *

__author__ = 'johna, tina and luca'

#version 3.1

POS_PORTS_MULTI = 65
if sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
    POS_PORTS_MULTI = 120

POS_PORTS_X = 135
POS_PORTS_Y = 0
CON_TO_X = 130
CON_TO_Y = 15

start = 0

procs = []

st = True

p_factor = 1

no_serial = False

ports = serial_finder.serial_ports()
port = None

UI = UI(procs)

cont = Controller(UI)

UI.textwrite(175, 250, "Please connect the serial device", 10, 10, 10, 50)
UI.textwrite(250, 300, "Press X for test mode", 10, 10, 10, 50)

UI.update()                         #Updates display

while (len(ports) == 0 or port == None):
    ports = serial_finder.serial_ports()
    port = serial_finder.find_port(ports)
    UI.shouldQuit()
    cont.update()
    if(cont.getButton(2)):
        no_serial = True
        break

UI.textdelete(175, 250, "Please connect the serial device", 50)
UI.textdelete(250, 300, "Press X for test mode", 50)
UI.textwrite(0, POS_PORTS_Y, "Possible ports: ")

for i in range(len(ports)):
    UI.textwrite(POS_PORTS_X + POS_PORTS_MULTI * i, POS_PORTS_Y, str(ports[i]))

UI.textwrite(0, CON_TO_Y, "Connected To: ")
UI.textwrite(CON_TO_X, CON_TO_Y, str(port))

outbound = serial.Serial(
    port=port,
    baudrate=9600,
    parity=serial.PARITY_NONE,   # parity is error checking, odd means the message should have an odd number of 1 bits
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,   # eight bits of information per pulse/packet
    timeout=0.1
)

UI.textwrite(0, 310, "Power factor:")

dataObjs = [DataHandling(UI, "PSR", "Pressure", "mbars", 70),
            DataHandling(UI, "VLT", "Current", "amps", 90),
            DataHandling(UI, "TMP", "Temperature", "degrees C",110),
            DataHandling(UI, "DPT", "Depth", "feet", 130),
            MotHandling(UI, "MOT", "M1", "", 190),
            MotHandling(UI, "MOT", "M2", "", 210),
            MotHandling(UI, "MOT", "M3", "", 230),
            MotHandling(UI, "MOT", "M4", "", 250),
            MotHandling(UI, "MOT", "M5", "", 270),
            MotHandling(UI, "MOT", "M6", "", 290),
            YPRHandling(UI, "YAW", "Y", "", 150, 0, 10),
            YPRHandling(UI, "PCH", "P", "", 150, 100, 10),
            YPRHandling(UI, "ROL", "R", "", 150, 200, 10)]

AH = AH(dataObjs, UI)

camera = camera(UI)
try:
    camera.start()
    procs.append(camera)
except:
    pass

UI.update()

while True:

    if no_serial:
        UI.textdelete(0, 35, "Serial device Connected")
        UI.textwrite(0, 35, "Connect the serial device", 255, 10, 10)
        UI.textdelete(CON_TO_X, CON_TO_Y, str(port))
        for i in range(len(ports)):
            UI.textdelete(POS_PORTS_X + POS_PORTS_MULTI * i, POS_PORTS_Y, str(ports[i]))
        ports = serial_finder.serial_ports()
        for i in range(len(ports)):
            UI.textwrite(POS_PORTS_X + POS_PORTS_MULTI * i, POS_PORTS_Y, str(ports[i]))
        if(len(ports) > 0):
            port = serial_finder.find_port(ports)
            if(port != None):
                outbound = serial.Serial(
                port=port,
                baudrate=9600,
                parity=serial.PARITY_NONE,   # parity is error checking, odd means the message should have an odd number of 1 bits
                stopbits=serial.STOPBITS_ONE,
                bytesize=serial.EIGHTBITS,   # eight bits of information per pulse/packet
                timeout=0.1
                )
                UI.textwrite(CON_TO_X, CON_TO_Y, str(port))
                no_serial = False

    if not no_serial:
        UI.textdelete(0, 35, "Connect the serial device")
        UI.textwrite(0, 35, "Serial device Connected", 10, 125, 10)


    if not cont.isConnected():                                      # Updates controller and shows whether it is connected.
        UI.textdelete(0, 50, "Controller connected")
        UI.textwrite(0, 50, "Connect the controller", 255, 10, 10)

    else:
        UI.textdelete(0, 50, "Connect the controller")
        UI.textwrite(0, 50, "Controller connected", 10, 125, 10)

    cont.update()
    buttons1 = 0x0
    buttons2 = 0x0
    # going to eight would include the start button; however, it seems that when 0x80 (only the start button) is sent
    # the arduino lags for a second or two and reports false values for buttons
    for i in range(0, cont.getNumButtons()):
        if(cont.getButton(i)):
            if(cont.getValueForButton(i) <= 0xFF):
                buttons1 += cont.getValueForButton(i)

            else:
                buttons2 += cont.getValueForButton(i) >> 8
                if buttons2 == 1:
                    UI.textdelete(140, 310, str(p_factor))
                    p_factor = p_factor / 2.0
                    if(p_factor < 0.5):
                        p_factor = 1

    try:
        if(not no_serial):
            outbound.write("STR") #  sends a signal to tell that this is the start of data
            print "STR"
            outbound.write(chr(buttons1))# writes the buttons first
            print chr(buttons1)
            outbound.write(chr(buttons2))
            print chr(buttons2)

            outbound.write(str(int(cont.getPrimaryX() * p_factor)))# casts the floats to ints, then to strings for simple parsing
            print str(int(cont.getPrimaryX() * p_factor))
            outbound.write(" ")
            outbound.write(str(int(cont.getPrimaryY() * p_factor)))
            print str(int(cont.getPrimaryY() * p_factor))
            outbound.write(" ")
            outbound.write(str(int(cont.getSecondaryX() * p_factor)))
            print str(int(cont.getSecondaryX() * p_factor))
            outbound.write(" ")
            outbound.write(str(int(cont.getSecondaryY() * p_factor)))
            print str(int(cont.getSecondaryY() * p_factor))
            outbound.write(" ")
            outbound.write(str(int(cont.getTriggers() * p_factor)))
            print str(int(cont.getTriggers() * p_factor))

            outbound.write(" ")

    except serial.serialutil.SerialException:
        no_serial = True

    except:
        print "WARN: Crashed while sending controller input"

    for Object in dataObjs:
            Object.wasUpdated = False

    try:
        if (not no_serial):
            counter = 10
            proceed = False

            while True and counter > 0:
                print "In Loop!"
                counter -= 1
                if outbound.readable():
                    if 'S' == outbound.read(1):
                        if 'T' == outbound.read(1):
                            if 'R' == outbound.read(1):
                                proceed = True
                                print "Recieved!"
                                if st:
                                    start = time.time()
                                    st = False
                                break

            if(proceed):                                            # Reads the serial line.
                    print "part 1"
                    linesToRead = int(outbound.read(3)) # allows for up to 999 lines to be read...
                    print "part 2"
                    if linesToRead >= 25:
                        linesToRead = 25
                    print "part 3"
                    print linesToRead
                    for i in range(0, linesToRead // 2):
                        print "part 4"
                        label = outbound.readline().rstrip().lstrip()
                        print "part 5"
                        print label
                        found = False

                        rev = outbound.readline().rstrip().split(",")
                        print rev
                        i = 0

                        if label == DEBUG_LABEL:
                            found = True
                            print rev

                        else:
                            for Object in dataObjs:
                                if Object.label == label:
                                    found = True
                                    Object.update(rev[i])
                                    i += 1

                        if not found:                                                       #In case it receives weird data, it prints it out on the terminal
                            print "INFO: unknown datatype: " + label
                            print "data: " + rev

            else:
                if not st:
                    end = time.time()
                    st = True
                    print "INFO: Lost data after" + str(end - start) + "seconds"

    except serial.serialutil.SerialException:
        no_serial = True

    except:
        print "WARN: Crashed while reading from arduino"

    for Object in dataObjs:
            Object.writeOldData()

    AH.update()

    UI.textwrite(140, 310, str(p_factor))

    #print pygame.mouse.get_pos()
    #print pygame.mouse.get_pressed()
    UI.update()                         #Updates display
    #sleep(0.01)                         #Waits for 10ms

    UI.shouldQuit()