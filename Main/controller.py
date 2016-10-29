__author__ = 'johna, tina and luca'

from sfml.window import Joystick
from sys import platform

import pygame
from pygame.locals import *
import time


deadZone = 15
minValue = 0
maxValue = 400 # higher gives more range of values however also adds more to send each time

# These are there codes for the getButton command, you can just use the variables for readability
A = 0
B = 1
X = 2
Y = 3
L_TRIGGER = 4
R_TRIGGER = 5
BACK = 6
START = 7
L_JOYSTICK_CLICK = 8
R_JOYSTICK_CLICK = 9

if platform == "linux" or platform == "linux2":
    L_JOYSTICK_CLICK = 9
    R_JOYSTICK_CLICK = 10
    WIN_BUTTON = 8

# These are their values in the signal sent to the arduino
A_HEX = 0x1
B_HEX = 0x2
X_HEX = 0x4
Y_HEX = 0x8
L_TRIGGER_HEX = 0x10
R_TRIGGER_HEX = 0x20
BACK_HEX = 0x40
BROKEN_HEX = 0x80   # this hex signal over serial seems to lag the arduino by a second, so we are not using it, ever
START_HEX = 0x100
L_JOYSTICK_CLICK_HEX = 0x200
R_JOYSTICK_CLICK_HEX = 0x400

if platform == "linux" or platform == "linux2":
    L_JOYSTICK_CLICK = 9
    R_JOYSTICK_CLICK = 10
    WIN_BUTTON = 8

#loop to find the correct controller
joystick = 0;
foundController = False

pygame.init()
screen = pygame.display.set_mode((1000, 500))
pygame.display.set_caption("Cal Poly Control Center")

background = pygame.Surface(screen.get_size())
background = background.convert()
background.fill((250, 250, 250))

writeonscreen = "Looking for controller, press A to chose the controller"

font = pygame.font.Font(None, 50)
text = font.render(writeonscreen, 1, (10, 10, 10))
textpos = text.get_rect()
textpos.centerx = background.get_rect().centerx
textpos.centery = background.get_rect().centery
background.blit(text, textpos)

screen.blit(background, (0, 0))

pygame.display.flip()

def GUIrun():
    background.blit(text, textpos)

    screen.blit(background, (0, 0))

    pygame.display.flip()

    for event in pygame.event.get():
        if event.type == QUIT:
            quit()

while not foundController:
    GUIrun()
    for i in range(0,5):
        Joystick.update()
        if Joystick.is_connected(i):
            Joystick.update()
            if Joystick.is_button_pressed(i, A):
                foundController = True
                joystick = i

writeonscreen = "Found controller " + str(i)
background.fill((250, 250, 250))
font = pygame.font.Font(None, 50)
text = font.render(writeonscreen, 1, (10, 10, 10))
textpos = text.get_rect()
textpos.centerx = background.get_rect().centerx
textpos.centery = background.get_rect().centery

background.blit(text, textpos)

screen.blit(background, (0, 0))

pygame.display.flip()

writeonscreen = "Buttons: " + str(Joystick.get_button_count(i))
font = pygame.font.Font(None, 50)
text = font.render(writeonscreen, 1, (10, 10, 10))
textpos = text.get_rect()
textpos.centerx = background.get_rect().centerx
textpos.centery = background.get_rect().centery + 50

background.blit(text, textpos)

screen.blit(background, (0, 0))

pygame.display.flip()

time.sleep(2)

# used inside the class, not necessary to call from outside this class, use the other calls
def getAxis(joyStickNumber, axis):
    size = maxValue - minValue
    return ((applyDeadZone(Joystick.get_axis_position(joyStickNumber, axis))/(100.0-deadZone)) * size) - minValue

def getPrimaryX():
    return getAxis(joystick, Joystick.X)

def getPrimaryY():
    return -getAxis(joystick, Joystick.Y)

def getSecondaryX():
    return getAxis(joystick, Joystick.U)

def getSecondaryY():
    if platform == "linux" or platform == "linux2":
        return -getAxis(joystick, Joystick.V)
    return -getAxis(joystick, Joystick.R)

def getTriggers():
    if(platform == "linux" or platform == "linux2"):
        value = getAxis(joystick, Joystick.Z)/2 - getAxis(joystick, Joystick.R)/2
    else:
        value = getAxis(joystick, Joystick.Z)
    return value

def isConnected():
    return Joystick.is_connected(joystick)

def getButton(button):
    return Joystick.is_button_pressed(joystick, button)

def getNumButtons():
    return Joystick.get_button_count(joystick)

def update():
    Joystick.update()

def setDeadZone(value):
    global deadZone
    deadZone = value

def getValueForButton(button):
    return {
        A:A_HEX,
        B:B_HEX,
        X: X_HEX,
        Y: Y_HEX,
        L_TRIGGER: L_TRIGGER_HEX,
        R_TRIGGER: R_TRIGGER_HEX,
        BACK: BACK_HEX,
        START: START_HEX,
        L_JOYSTICK_CLICK: L_JOYSTICK_CLICK_HEX,
        R_JOYSTICK_CLICK: R_JOYSTICK_CLICK_HEX
    }.get(button, 0)

# This creates a dead zone to prevent situations where you are unable to stop the motors because of touchy input
def applyDeadZone(value):
    negative = False
    if value <= 0:
        negative = True
    temp = abs(value)
    temp -= deadZone
    if temp <= 0:
        temp = 0
    return -temp if negative else temp

