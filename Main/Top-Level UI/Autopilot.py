__author__ = "Luca"

LIST_SIZE = 20

P_CONST = 10
I_CONST = 10
D_CONST = 10

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
