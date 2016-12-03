import serial
import serial.tools.list_ports

from UIUtils import *

from time import sleep

from controller import *
import serial_finder

from ArtificialHorizon import *

__author__ = 'johna, tina and luca'

#version 3.1

POS_PORTS_MULTI = 65
POS_PORTS_X = 135
POS_PORTS_Y = 0
CON_TO_X = 130
CON_TO_Y = 20

start = 0

st = True

p_factor = 1

no_serial = False

ports = serial_finder.serial_ports()

UI = UI()

cont = Controller(UI)

UI.textwrite(175, 250, "Please connect the serial device", 10, 10, 10, 50)

UI.update()                         #Updates display

while(len(ports) == 0):
    ports = serial_finder.serial_ports()
    UI.shouldQuit()

UI.textdelete(175, 250, "Please connect the serial device", 50)
UI.textwrite(0, POS_PORTS_Y, "Possible ports: ")

for i in range(len(ports)):
    UI.textwrite(POS_PORTS_X + POS_PORTS_MULTI * i, POS_PORTS_Y, str(ports[i]))

port = serial_finder.find_port(ports)

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

UI.textwrite(0, 450, "Power factor:")

dataObjs = [DataHandling(UI, "PSR", "Pressure", "mbars", 90),
            DataHandling(UI, "VLT", "Current", "amps", 110),
            DataHandling(UI, "TMP", "Temperature", "degrees C",130),
            DataHandling(UI, "DPT", "Depth", "feet", 150),
            MotHandling(UI, "MOT", "M1", "", 250),
            MotHandling(UI, "MOT", "M2", "", 270),
            MotHandling(UI, "MOT", "M3", "", 290),
            MotHandling(UI, "MOT", "M4", "", 310),
            MotHandling(UI, "MOT", "M5", "", 330),
            MotHandling(UI, "MOT", "M6", "", 350),
            YPRHandling(UI, "YAW", "Y", "", 170, 0, 10),
            YPRHandling(UI, "PCH", "P", "", 170, 100, 10),
            YPRHandling(UI, "ROL", "R", "", 170, 200, 10)]

AH = AH(dataObjs, UI)



UI.update()

while True:

    if no_serial:
        UI.textdelete(0, 40, "Serial device Connected")
        UI.textwrite(0, 40, "Connect the serial device", 255, 10, 10)
        UI.textdelete(CON_TO_X, CON_TO_Y, str(port))
        for i in range(len(ports)):
            UI.textdelete(POS_PORTS_X + POS_PORTS_MULTI * i, POS_PORTS_Y, str(ports[i]))
        ports = serial_finder.serial_ports()
        for i in range(len(ports)):
            UI.textwrite(POS_PORTS_X + POS_PORTS_MULTI * i, POS_PORTS_Y, str(ports[i]))
        if(len(ports) > 0):
            port = serial_finder.find_port(ports)
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

    else:
        UI.textdelete(0, 40, "Connect the serial device")
        UI.textwrite(0, 40, "Serial device Connected", 10, 125, 10)


    if not cont.isConnected():                                      # Updates controller and shows whether it is connected.
        UI.textdelete(0, 60, "Controller connected")
        UI.textwrite(0, 60, "Connect the controller", 255, 10, 10)

    else:
        UI.textdelete(0, 60, "Connect the controller")
        UI.textwrite(0, 60, "Controller connected", 10, 125, 10)

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
                    UI.textdelete(140, 450, str(p_factor))
                    p_factor = p_factor / 2.0
                    if(p_factor < 0.5):
                        p_factor = 1

    try:
        outbound.write("STR") #  sends a signal to tell that this is the start of data
        outbound.write(chr(buttons1))# writes the buttons first
        outbound.write(chr(buttons2))

        outbound.write(str(int(cont.getPrimaryX() * p_factor)))# casts the floats to ints, then to strings for simple parsing
        outbound.write(" ")
        outbound.write(str(int(cont.getPrimaryY() * p_factor)))
        outbound.write(" ")
        outbound.write(str(int(cont.getSecondaryX() * p_factor)))
        outbound.write(" ")
        outbound.write(str(int(cont.getSecondaryY() * p_factor)))
        outbound.write(" ")
        outbound.write(str(int(cont.getTriggers() * p_factor)))

        outbound.write(" ")

    except serial.serialutil.SerialException:
        no_serial = True

    except:
        print "Crashed while sending controller input"

    for Object in dataObjs:
            Object.wasUpdated = False

    try:
        counter = 10
        proceed = False

        while True and counter > 0:
            counter -= 1
            if outbound.readable():
                if 'S' == outbound.read(1):
                    if 'T' == outbound.read(1):
                        if 'R' == outbound.read(1):
                            proceed = True
                            if st:
                                start = time.time()
                                st = False
                            break

        if(proceed):                                            # Reads the serial line.
                linesToRead = int(outbound.read(3)) # allows for up to 999 lines to be read...
                if linesToRead >= 25:
                    linesToRead = 25
                for i in range(0, linesToRead // 2):
                    label = outbound.readline().rstrip().lstrip()
                    found = False

                    rev = outbound.readline().rstrip().split(",")
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
                        print "unknown datatype: " + label
                        print "data: " + rev

        else:
            if not st:
                end = time.time()
                st = True
                print "Lost data after" + str(end - start) + "seconds"

    except serial.serialutil.SerialException:
        no_serial = True

    except:
        print "Crashed while reading from arduino"

    for Object in dataObjs:
            Object.writeOldData()

    AH.update()

    UI.textwrite(140, 450, str(p_factor))

    UI.update()                         #Updates display
    sleep(0.01)                         #Waits for 10ms

    UI.shouldQuit()