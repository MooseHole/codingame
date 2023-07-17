import sys
import math

class Cell:
    level = 0
    def __init__(self, initialValue):
        self.setLevel(initialValue)

    def setLevel(self, level):
        if (level == '.'):
            self.level = 4
        else:
            self.level = int(level)

        if (self.level > 3):
            self.level = -100

    def build(self):
        self.setLevel(self.level + 1)
        # print("build() new level: " + str(self.level), file=sys.stderr, flush=True)

    def toStr(self):
        if (self.level < 0):
            return '.'
        else:
            return str(self.level)

class Grid:
    def __init__(self, size):
        self.size = size
        self.grid = [[Cell(0) for i in range(size)] for j in range(size)]
        self.currentRow = 0

    def loadRow(self, row):
        for i in range(self.size):
            self.grid[i][self.currentRow].setLevel(row[i])
        self.currentRow += 1

    def clone(self):
        clone = Grid(self.size)
        for i in range(self.size):
            for j in range(self.size):
                clone.grid[i][j].setLevel(self.grid[i][j].level)
        return clone
    
    def toStr(self):
        output = ""
        for i in range(self.size):
            for j in range(self.size):
                output += self.grid[i][j].toStr()
            output += "\n"
        return output

class Coordinate:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    def clone(self):
        return Coordinate(self.x, self.y)

class Unit:
    def __init__(self, x, y):
        self.location = Coordinate(x, y)

    def clone(self):
        return Unit(self.location.x, self.location.y)

class LegalAction:
    def __init__(self, atype, index, dir_1, dir_2, grid, playerUnits):
        self.atype = atype
        self.index = index
        self.dir_1 = dir_1
        self.dir_2 = dir_2
        self.grid = grid.clone()
        self.playerUnits = []
        for i in range(len(playerUnits)):
            self.playerUnits.append(playerUnits[i].clone())
        self.performAction()
        self.scoreFound = False

    def performAction(self):
        if (self.atype == 'MOVE&BUILD'):
            self.move()
            self.build()

    def getCoordinateFromDirection(self, coordinate, direction):
        coordinateClone = coordinate.clone()
        if (direction == 'N'):
            coordinateClone.y -= 1
        elif (direction == 'S'):
            coordinateClone.y += 1
        elif (direction == 'E'):
            coordinateClone.x += 1
        elif (direction == 'W'):
            coordinateClone.x -= 1
        elif (direction == 'NW'):
            coordinateClone.y -= 1
            coordinateClone.x -= 1
        elif (direction == 'SW'):
            coordinateClone.y += 1
            coordinateClone.x -= 1
        elif (direction == 'NE'):
            coordinateClone.y -= 1
            coordinateClone.x += 1
        elif (direction == 'SE'):
            coordinateClone.y += 1
            coordinateClone.x += 1
        return coordinateClone

    def move(self):
        self.playerUnits[self.index].location = self.getCoordinateFromDirection(self.playerUnits[self.index].location, self.dir_1)

    def build(self):
        buildLocation = self.getCoordinateFromDirection(self.playerUnits[self.index].location, self.dir_2)
        # print("Build at " + str(buildLocation.x) + "," + str(buildLocation.y), file=sys.stderr, flush=True)
        self.grid.grid[buildLocation.x][buildLocation.y].build()

    def getScore(self):
        if (self.scoreFound):
            return self.score

        # print("getScore() for " + self.toStr(), file=sys.stderr, flush=True)
        self.score = 0
        for i in range(len(self.playerUnits)):
            myLevel = self.grid.grid[self.playerUnits[i].location.x][self.playerUnits[i].location.y].level
            self.score += myLevel * 10
            # print("getScore() " + str(self.playerUnits[i].location.x) + "," + str(self.playerUnits[i].location.y) + " myLevel: " + str(myLevel) + " total:" + str(self.score), file=sys.stderr, flush=True)
            for x in range (-1, 1):
                for y in range (-1, 1):
                    if (x != 0 and y != 0):
                        checkLevel = self.grid.grid[self.playerUnits[i].location.x + x][self.playerUnits[i].location.y + y].level
                        if (checkLevel == myLevel + 1):
                            self.score += checkLevel
                            # print("getScore() " + str(x) + "," + str(y) + " level: " + str(checkLevel) + " total:" + str(self.score), file=sys.stderr, flush=True)


        self.scoreFound = True
        return self.score
    
    def toStr(self):
        return self.atype + " " + str(self.index) + " " + self. dir_1 + " " + self.dir_2

size = int(input())
print("Load size " + str(size), file=sys.stderr, flush=True)
units_per_player = int(input())
print("Load units_per_player " + str(units_per_player), file=sys.stderr, flush=True)

# game loop
while True:
    grid = Grid(size)
    for i in range(size):
        print("Load grid row " + str(i), file=sys.stderr, flush=True)
        grid.loadRow(input())

    print("Grid:\n" + grid.toStr(), file=sys.stderr, flush=True)

    playerUnits = []
    opponentUnits = []
    legalActions = []

    for i in range(units_per_player):
        unit_x, unit_y = [int(j) for j in input().split()]
        playerUnits.append(Unit(unit_x, unit_y))
        print("Load player unit " + str(i) + " at " + str(unit_x) + "," + str(unit_y), file=sys.stderr, flush=True)
    for i in range(units_per_player):
        other_x, other_y = [int(j) for j in input().split()]
        opponentUnits.append(Unit(unit_x, unit_y))
        print("Load opponent unit " + str(i) + " at " + str(other_x) + "," + str(other_y), file=sys.stderr, flush=True)

    legal_actions = int(input())
    print("Load legal_actions " + str(legal_actions), file=sys.stderr, flush=True)
    for i in range(legal_actions):
        inputs = input().split()
        atype = inputs[0]
        index = int(inputs[1])
        dir_1 = inputs[2]
        dir_2 = inputs[3]
        legalAction = LegalAction(atype, index, dir_1, dir_2, grid, playerUnits)
        legalActions.append(legalAction)
        # print("Load legal action " + atype + " " + str(index) + " " + dir_1 + " " + dir_2 + " score:" + str(legalAction.getScore()), file=sys.stderr, flush=True)

    bestScore = -999999
    for i in range(len(legalActions)):
        score = legalActions[i].getScore()
        if (score > bestScore):
            bestScore = score
            bestAction = legalActions[i]
            print("Found bestAction score: " + str(bestScore) + " action: " + bestAction.toStr(), file=sys.stderr, flush=True)

    print(bestAction.toStr())
