import pygame
from pygame.locals import *

__author__ = "Luca"

class UI:
    def __init__(self):
        pygame.init()                                           #initializes the UI
        self.screen = pygame.display.set_mode((1000, 500))
        pygame.display.set_caption("Cal Poly Control Center")

        self.background = pygame.Surface(self.screen.get_size())          # Set background to white.
        self.background = self.background.convert()
        self.background.fill((255, 255, 255))
        self.update()

    def update(self):
        pygame.display.update()

    def textwrite(self, Positionx, Positiony, Text, r = 10, g = 10 , b = 10, size = 25): #Function that write on screen Strings.

        font = pygame.font.Font(None, size)
        text = font.render(Text, 0, (r, g, b))
        textpos = text.get_rect()
        textpos.x = Positionx
        textpos.y = Positiony
        self.background.blit(text, textpos)
        self.screen.blit(self.background, (0, 0))

    def textdelete(self, Positionx, Positiony, Text, size = 25):                 #Function that deletes strings

        font = pygame.font.Font(None, size)
        text = font.render(Text, 0, (255, 255, 255))
        textpos = text.get_rect()
        textpos.x = Positionx
        textpos.y = Positiony
        self.background.blit(text, textpos)
        self.screen.blit(self.background, (0, 0))

    def blit(self, obj, objPos):

        self.background.blit(obj, objPos)

    def drawRect(self, rect, color = (10, 10, 10), width = 0):

        pygame.draw.rect(self.background, color, rect, width)

    def shouldQuit(self):
        for event in pygame.event.get():
            if event.type == QUIT:
                quit()
