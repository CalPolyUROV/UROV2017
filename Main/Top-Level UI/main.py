import serial
import serial.tools.list_ports

import pygame
from pygame.locals import *
from time import sleep
from types import *

import time

import controller as cont
import serial_finder

__author__ = 'johna, tina and luca'

#version 3.0

LIST_SIZE = 3                   #constants
AH_LIST_SIZE = 10
MOT_LIST_SIZE = 8

pressure = 0.0                  #initiates varibles.
current = 0.0
temperature = 0.0
depth = 0.0
ypr = 0.0
yprraw = 0.0

tmp = [0.0] * LIST_SIZE
pres = [0.0] * LIST_SIZE
curr = [0.0] * LIST_SIZE
dep = [0.0] * LIST_SIZE

st = True

pr = 0
rr =0
yr =0

pchr = 0
yawr = 0
rolr = 0

pchl = [0.0] * AH_LIST_SIZE
yawl = [0.0] * AH_LIST_SIZE
roll = [0.0] * AH_LIST_SIZE

mot = [0.0] * MOT_LIST_SIZE

start = 0

p_factor = 1

no_serial = False

def textwrite(Positionx, Positiony, Text, r = 10, g = 10 , b = 10, size = 25): #Function that write on screen Strings.

    writeonscreen = Text
    font = pygame.font.Font(None, size)
    text = font.render(writeonscreen, 0, (r, g, b))
    textpos = text.get_rect()
    textpos.centerx = Positionx
    textpos.centery = Positiony
    background.blit(text, textpos)
    screen.blit(background, (0, 0))

def textdelete(Positionx, Positiony, Text, size = 25):                 #Function that deletes strings

    writeonscreen = Text
    font = pygame.font.Font(None, size)
    text = font.render(writeonscreen, 0, (255, 255, 255))
    textpos = text.get_rect()
    textpos.centerx = Positionx
    textpos.centery = Positiony
    background.blit(text, textpos)
    screen.blit(background, (0, 0))

#@param l: the list contain a record of that data
#@param rev: The data received
#@param correction: A value to be removed from the data received before inputing it to the list
def readOutput(L, rev, correction = 0):

    if type(L) is not ListType:
        raise TypeError("Param 1 is not a list")

    if type(correction) is not (IntType or FloatType or LongType):
        raise TypeError("Param 3 is not a number")

    if type(rev) is not StringType:
        raise TypeError("Param 2 is not a string")

    try:
        tempo = float(rev)
        index = len(L) - 1
        while index > 0:
            L[index] = L[index - 1]
            index -= 1
        L[0] = tempo - correction

        return True

    except:
        print rev + " <-",
        return False

def filterYPR(L, YPRR):
    if type(L) is not ListType:
        raise TypeError("Param 1 is not a list")

    try:
        if abs(L[0] - L[9]) <= 5:      #Filters the noise in the data.
            max = 10
        elif abs(L[0] - L[9]) <= 10:
            max = 5
        else:
            max = 3
        i = 0
        total = 0
        while i < max:
            total += L[i]
            i += 1
        YPRR = total / float(i)

    except:
        print "Crashed while filtering YPR data"
        return False

    return True


pygame.init()                                           #initializes the UI
screen = pygame.display.set_mode((1000, 500))
pygame.display.set_caption("Cal Poly Control Center")

background = pygame.Surface(screen.get_size())          # Set background to white.
background = background.convert()
background.fill((255, 255, 255))

ports = serial_finder.serial_ports()

textwrite(500, 250, "Please connect the serial device", 10, 10, 10, 50)
pygame.display.update()                         #Updates display

while(len(ports) == 0):
    ports = serial_finder.serial_ports()

    for event in pygame.event.get():
        if event.type == QUIT:
            quit()

textdelete(500, 250, "Please connect the serial device", 50)
textwrite(75, 10, "Possible ports: ")

for i in range(len(ports)):
    textwrite(175 + 65 * i, 10, str(ports[i]))

port = serial_finder.find_port(ports)

textwrite(70, 30, "Connected To: ")
textwrite(160, 30, str(port))

outbound = serial.Serial(
    port=port,
    baudrate=9600,
    parity=serial.PARITY_NONE,   # parity is error checking, odd means the message should have an odd number of 1 bits
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,   # eight bits of information per pulse/packet
    timeout=0.1
)

img1 = pygame.image.load('ArtificialHorizon.png')                       #Loads the Artificial Horizon images.
img2 = pygame.image.load("ArtificialHorizonOverlay.png")
img3 = pygame.image.load("ArtificialHorizonMarker.png").convert_alpha()

pygame.display.update()

while True:

    if no_serial:
        textdelete(110, 50, "Serial device Connected")
        textwrite(115, 50, "Connect the serial device", 255, 10, 10)
        textdelete(160, 30, str(port))
        for i in range(len(ports)):
            textdelete(175 + 65 * i, 10, str(ports[i]))
        ports = serial_finder.serial_ports()
        for i in range(len(ports)):
            textwrite(175 + 65 * i, 10, str(ports[i]))
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
            textwrite(160, 30, str(port))
            no_serial = False

    else:
        textdelete(115, 50, "Connect the serial device")
        textwrite(110, 50, "Serial device Connected", 10, 125, 10)


    if not cont.isConnected():                                      # Updates controller and shows whether it is connected.
        textdelete(98, 70, "Controller connected")
        textwrite(105, 70, "Connect the controller", 255, 10, 10)

    else:
        textdelete(105, 70, "Connect the controller")
        textwrite(98, 70, "Controller connected", 10, 125, 10)

    textwrite(45, 90, "Pressure:")                     #Labels in black.
    textwrite(40, 110, "Current:")
    textwrite(64, 130, "Temperature:")
    textwrite(34, 150, "Depth:")
    textwrite(25, 190, "YPR:")
    textwrite(40, 170, "YPRraw:")

    textwrite(20, 250, "M1:")                        #Motor info. Work in progress.
    textwrite(20, 270, "M2:")
    textwrite(20, 290, "M3:")
    textwrite(20, 310, "M4:")
    textwrite(20, 330, "M5:")
    textwrite(20, 350, "M6:")
    textwrite(20, 370, "X1:")
    textwrite(20, 390, "X2:")

    textwrite(60, 450, "Power factor:")

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
                    textdelete(140, 450, str(p_factor))
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

    got = ''                                                            #shows in red the old data in case of lost connection.
    textwrite(200, 90, (str(pressure) + " mbars"), 255, 10, 10)
    textwrite(200, 110, (str(current) + " amps"), 255, 10, 10)
    textwrite(200, 130, (str(temperature) + " degrees C"), 255, 10, 10)
    textwrite(200, 150, (str(depth) + " feet"), 255, 10, 10)
    textwrite(200, 170, str(ypr), 255, 10, 10)
    textwrite(200, 190, str(yprraw), 255, 10, 10)

    for i in range(len(mot)):
        textwrite(100, 250 + 20 * i, str(mot[i]), 255, 10, 10)                        #motor info.
        pygame.draw.rect(background, (255, 10, 10), (150, 245 + i * 20, (10 * mot[i]), 10))

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
                if(label == "PSR"):                                                    # Pressure Data.
                    rev = outbound.readline().rstrip()
                    if(readOutput(pres, rev)):
                        textdelete(200, 90, str(pressure) + " mbars")
                        pressure = round(sum(pres)/len(pres), 2)
                        textwrite(200, 90, str(pressure) + " mbars", 10, 125, 10)
                    else:
                        print "Could not read pressure data"

                elif(label == "VLT"):                                                   # Electric Current data.
                    rev = outbound.readline().rstrip()
                    if(readOutput(curr, rev)):
                        textdelete(200, 110, str(current) + " amps")
                        current = round(sum(curr)/len(curr), 2)
                        textwrite(200, 110, str(current) + " amps", 10, 125, 10)
                    else:
                        print "Could not read current data"

                elif(label == "TMP"):                                                   # Temperature Data.
                    rev = outbound.readline().rstrip()
                    if(readOutput(tmp, rev)):
                            textdelete(200, 130, str(temperature) + " degrees C")
                            temperature = round(sum(tmp)/len(tmp), 2)
                            textwrite(200, 130, str(temperature) + " degrees C", 10, 125, 10)
                    else:
                        print "Could not read temperature data"

                elif(label == "DPT"):                                                   #Depth data from the arduino.
                    rev = outbound.readline().rstrip()
                    if(readOutput(dep, rev)):
                        textdelete(200,150, str(depth) + " feet")
                        depth = round(sum(dep)/len(dep), 2)
                        textwrite(200, 150, str(depth) + " feet", 10, 125, 10)
                    else:
                        print "Could not read depth data"


                elif(label == "YAW"):                                                   # Taw data from the Accelerometer
                    rev = outbound.readline().rstrip()
                    if(readOutput(yawl, rev, yr)):
                        got = got + 'T'
                        yaw = rev
                    else:
                        print "Could not read yaw data"

                elif(label == "PCH"):                                                      #Pitch data from the Accelerometer
                    rev = outbound.readline().rstrip()
                    if(readOutput(pchl, rev, pr)):
                        got = got + 'T'
                        pch = rev
                    else:
                        print "Could not read pitch data"

                elif(label == "ROL"):                                                      # Rol data from the accelerometer
                    rev = outbound.readline().rstrip()
                    if(readOutput(roll, rev, rr)):
                        got = got + 'T'
                        rol = rev
                    else:
                        print "Could not read roll data"

                elif(label == "STS"):
                    print outbound.readline().rstrip()

                elif(label == "MOT"):                                                       #Data from the motors.
                    recr = outbound.readline().rstrip()
                    rec = recr.split(",")
                    if(len(rec) == len(mot)):
                        for i in range(len(mot)):
                            textdelete(100, 250, str(mot[i]))
                            try:
                                tempo = float(rec[i])
                                pygame.draw.rect(background, (255, 255, 255), (150, 245, (10 * mot[i]), 10))
                                mot[i] = tempo
                                textwrite(100, 250, mot[i], 10, 125, 10)
                                if mot[i] < 10 and mot[i] > 0:
                                    pygame.draw.rect(background, (10, 125, 10), (150, 245, (10 * mot[i]), 10))
                                elif  mot[i] >= 10 and mot[i] <= 13:
                                    pygame.draw.rect(background, (125, 125, 10), (150, 245, (10 * mot[i]), 10))
                                elif mot[i] > 13:
                                    pygame.draw.rect(background, (255, 125, 10), (150, 245, (10 * mot[i]), 10))
                            except:
                                print "Could not read motor " + str(i) + " data: " + rec[i]
                    else:
                        print "Lenth of received data does not match expected lenth: R-" + str(len(rec)) + " E-" + str(len(mot))

                else:                                                       #In case it receives weird data, it prints it out on the terminal
                    print "unknown datatype:", label
                    print "data:", outbound.readline().rstrip()

        else:
            if not st:
                end = time.time()
                st = True
                print "Lost data after" + str(end - start) + "seconds"

    except serial.serialutil.SerialException:
        no_serial = True

    except:
        print "Crashed while reading from arduino"

    try:
        for event in pygame.event.get(pygame.KEYDOWN):
            if event.type == pygame.KEYDOWN:
                if event.key == K_n:                 #Nomalizes the Artificial Horizon
                    yr = int(yaw)
                    pr = int(pch)
                    rr = int(rol)
                elif event.key == K_r:               #Sets the Artificial Horizon to the Raw data from the accelerometer.
                    yr = 0
                    pr = 0
                    rr = 0

    except:
        print "Crashed while reading keyboard inputs"

    filterYPR(pchl, pchr)
    filterYPR(roll, rolr)
    yawr = sum(yawl) / len(yawl)

    try:
        if got == 'TTT':                                                       #Prints out the Accelerometer data.
            textdelete(200,170, str(ypr))
            textdelete(200,190, str(yprraw))
            yprraw = 'Y:' + str(round(yawr)) + ' P:' + str(round(pchr)) + ' R:' + str(round(rolr))
            ypr = 'Y:' + str(round(float(yaw))) + ' P:' + str(round(float(pch))) + ' R:' + str(round(float(rol)))
            textwrite(200, 170, ypr, 10, 125, 10)
            textwrite(200, 190, yprraw, 10, 125, 10)

    except:
        print "Crashed while computing YPR data"

    try:
        img1pos = img1.get_rect()                  #Places down the artificial horizon background
        img1pos.centerx = 750
        img1pos.centery = 250 + (round(pchr * 5))
        background.blit(img1, img1pos)

        img2pos = img2.get_rect()                  #Places down the artificial horizon overlay
        img2pos.centerx = 750
        img2pos.centery = 250
        background.blit(img2, img2pos)

        img4 = pygame.transform.rotate(img3, round(rolr)) #Places down the artificial horizon marker
        img4pos = img4.get_rect()
        img4pos.centerx = 750
        img4pos.centery = 264
        background.blit(img4, img4pos)

    except:
        print "Crashed while loading AH images"

    textwrite(140, 450, str(p_factor))

    pygame.display.update()                         #Updates display
    sleep(0.01)                                     #Waits for 10ms

    for event in pygame.event.get():                #It makes the UI quit if the X button is pressed
        if event.type == QUIT:
            quit()