import pygame.camera
from pygame.locals import *

from time import sleep

__author__ = "Luca"

LIST_SIZE = 20

P_CONST = 10
I_CONST = 10
D_CONST = 10

import threading

DEVICE = "/dev/video0"
#DEVICE = "/dev/bus/usb/001/008"

class AutoPilot:

    def __init__(self):
        self.errors = [0.0] * LIST_SIZE
        self.times = [0.0] * LIST_SIZE
        self.P = 0
        self.I = 0
        self.D = 0
        self.goal = 0

    def CalcAltitude(self, user, pres, time):
        if(user != 0):
            error = 0
        else:
            error = self.goal - pres

        for i in range(LIST_SIZE - 1):
            self.errors[LIST_SIZE - i] = self.errors[LIST_SIZE - i - 1]
            self.times[LIST_SIZE - i] = self.times[LIST_SIZE - i - 1]

        self.errors[0] = error
        self.times[0] = time

        if(user != 0):
            self.goal = pres
            PID = self.I + self.D + user
            if(PID > 400):
                PID = 400
            elif(self.PID < 400):
                PID = -400

            return PID

        self.I = 0
        for i in range(LIST_SIZE):
            self.I += self.errors[i] * self.times[i]
        self.I *= I_CONST

        self.D = ((self.errors[0] - self.errors[1]) / self.times[0])*  D_CONST

        self.P = error * P_CONST
        PID = self.P + self.I +self.D

        if(PID > 400):
            PID = 400
        elif(self.PID < 400):
            PID = -400

        return PID

class camera(threading.Thread):
    def __init__(self, UI):
        try:
            pygame.camera.init()
            self.UI = UI
            self.camera = pygame.camera.Camera(DEVICE, (500, 370))
            self.camera.start()
            self.snapshot = pygame.surface.Surface((500, 370), 0, self.UI.screen).convert()
            self.runing = True
            self.shouldUpdate = True
            threading.Thread.__init__(self)
            self._stop = threading.Event()
        except:
            self.runing = False
            print "FATAL: Could not initialize Camera"

    def run(self):
        while self.runing:
            try:
                if self.camera.query_image():
                    self.snapshot = self.camera.get_image(self.snapshot)
                    self.UI.blit(self.snapshot, (0, 330))
                    print self.shouldUpdate
                    if self.shouldUpdate:
                        self.UI.update()
                    self.UI.shouldQuit()
                    sleep(0.03)
            except:
                pass

    def update(self, shouldUpdate):
         self.shouldUpdate = shouldUpdate

    def end_proc(self):
        print "Called!"
        self.run = False

    def stop(self):
        self._stop.set()

    def stopped(self):
        return self._stop.isSet()