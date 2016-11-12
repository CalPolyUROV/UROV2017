__author__ = "Luca"

CHAR_SIZE = 5

DEBUG_LABEL = "STS"

class DataHandling(object):

    def __init__(self, UI, label, tytle, unit, posY, posX = 0,size = 3):
        if not isinstance(label, str):
            raise TypeError("Param 2 is not a string")

        if not isinstance(tytle, str):
            raise TypeError("Param 3 is not a string")

        if not isinstance(unit, str):
            raise TypeError("Param 4 is not a string")

        if not (isinstance(posY, int) or isinstance(posY, float) or isinstance(posY, long)):
            raise TypeError("Param 5 is not a number")

        if not (isinstance(posX, int) or isinstance(posX, float) or isinstance(posX, long)):
            raise TypeError("Param 6 is not a number")

        if not (isinstance(size, int) or isinstance(size, float) or isinstance(size, long)):
            raise TypeError("Param 7 is not a number")

        self.UI = UI
        self.lable = label
        self.tytle = tytle
        self.unit = unit
        self.posY = posY
        self.data = [0.0] * size
        self.filtered = 0
        self.tytlePosX = posX
        self.wasUpdated = True
        self.wasUpdatedBefore = True

        self.UI.textwrite(self.tytlePosX, self.posY, self.tytle + ": ")

    def filterData(self):
        self.filtered = round(sum(self.data)/len(self.data), 2)

    def update(self, rev):
        try:
            index = len(self.data) - 1
            while index > 0:
                self.data[index] = self.data[index - 1]
                index -= 1
            self.data[0] = float(rev)

            textDelete = str(self.filtered) + " " + self.unit
            self.UI.textDelete(self.tytlePosX + (len(self.tytle) + len(textDelete)) * CHAR_SIZE + 10, self.posY, textDelete)

            self.filterData()

            textWrite = str(self.filtered) + " " + self.unit
            self.UI.textwrite(self.tytlePosX + (len(self.tytle) + len(textWrite)) * CHAR_SIZE + 10, self.posY, textWrite, 10, 125, 10)

            self.wasUpdated = True
            self.wasUpdatedBefore = True

            return True

        except:
            print "Could not read " + self.tytle + " data"
            return False

    def writeOldData(self):
        if self.wasUpdated or not self.wasUpdatedBefore:
            return
        self.wasUpdatedBefore = False
        textWrite = str(self.filtered) + " " + self.unit
        self.UI.textwrite(self.tytlePosX + (len(self.tytle) + len(textWrite)) * CHAR_SIZE + 10, self.posY, textWrite, 255, 10, 10)

class YPRHandling(DataHandling):

    def filterData(self):
        if abs(self.data[0] - self.data[len(self.data) - 1]) <= 5:      #Filters the noise in the data.
            max = len(self.data)
        elif abs(self.data[0] - self.data[len(self.data) - 1]) <= 10:
            max = len(self.data) / 2
        else:
            max = len(self.data) / 3
        i = 0
        total = 0
        while i < max:
            total += self.data[i]
            i += 1
        self.filtered = total / float(i)

def getDataObj(L, lable):

    if not isinstance(L, list):
        raise TypeError("Param 1 is not a list")

    if not isinstance(lable, str):
        raise TypeError("Param 2 is not a string")

    for Object in L:
        if type(Object) is DataHandling or type(Object) is YPRHandling:
            if Object.lable == lable:
                return Object

    raise LookupError("No such element with label: " + lable)