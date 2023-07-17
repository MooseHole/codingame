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
            self.level = -1

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
    
    def hash(self):
        return self.x * 10 + self.y

class Unit:
    def __init__(self, x, y):
        self.location = Coordinate(x, y)

    def clone(self):
        return Unit(self.location.x, self.location.y)

adjacentCells = {}

class LegalAction:
    def __init__(self, atype, index, dir_1, dir_2, grid, playerUnits, opponentUnits):
        self.atype = atype
        self.index = index
        self.dir_1 = dir_1
        self.dir_2 = dir_2
        self.grid = grid.clone()
        self.playerUnits = []
        self.opponentUnits = []
        for i in range(len(playerUnits)):
            self.playerUnits.append(playerUnits[i].clone())
        for i in range(len(opponentUnits)):
            self.opponentUnits.append(opponentUnits[i].clone())
        self.performAction()
        self.scoreFound = False

    def performAction(self):
        if (self.atype == 'MOVE&BUILD'):
            self.moveAndBuild()
        if (self.atype == 'PUSH&BUILD'):
            self.pushAndBuild()

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

    def moveAndBuild(self):
        self.playerUnits[self.index].location = self.getCoordinateFromDirection(self.playerUnits[self.index].location, self.dir_1)
        buildLocation = self.getCoordinateFromDirection(self.playerUnits[self.index].location, self.dir_2)
        # print("Build at " + str(buildLocation.x) + "," + str(buildLocation.y), file=sys.stderr, flush=True)
        self.grid.grid[buildLocation.x][buildLocation.y].build()

    def pushAndBuild(self):
        pushedUnitLocation = self.getCoordinateFromDirection(self.playerUnits[self.index].location, self.dir_1)
        for i in range(len(self.opponentUnits)):
            if (self.opponentUnits[i].location.x == pushedUnitLocation.x and self.opponentUnits[i].location.y == pushedUnitLocation.y):
                self.opponentUnits[i].location = self.getCoordinateFromDirection(self.opponentUnits[i].location, self.dir_2)
        self.grid.grid[pushedUnitLocation.x][pushedUnitLocation.y].build()

    def getScore(self):
        if (self.scoreFound):
            return self.score

        # print("getScore() for " + self.toStr(), file=sys.stderr, flush=True)
        self.score = 0
        for i in range(len(self.playerUnits)):
            # Score for my location
            myLocation = self.playerUnits[i].location
            myLevel = self.grid.grid[myLocation.x][myLocation.y].level

            # If I scored
            if (self.atype == 'MOVE&BUILD' and i == self.index):
                self.score += myLevel * 10

            # Score for adjacent cells
            escapeRoutes = 0
            for adjacentCell in adjacentCells[myLocation.hash()]:
                checkLevel = self.grid.grid[adjacentCell.x][adjacentCell.y].level
                if (checkLevel == myLevel + 1):
                    self.score += checkLevel

                possible = False
                if (checkLevel >= 0 and checkLevel < myLevel + 2):
                    possible = True

                    for unit in self.playerUnits:
                        if (unit.location.x == adjacentCell.x and unit.location.y == adjacentCell.y):
                                possible = False
                    if (possible):
                        escapeRoutes += 1


            self.score += escapeRoutes

        self.scoreFound = True
        return self.score
    
    def toStr(self):
        return self.atype + " " + str(self.index) + " " + self. dir_1 + " " + self.dir_2

size = int(input())
print("Load size " + str(size), file=sys.stderr, flush=True)
for i in range(size):
    for j in range(size):
        hash = i * 10 + j
        adjacentCells[hash] = []
        for x in range (-1, 1):
            for y in range (-1, 1):
                if (x != 0 and y != 0 and i + x >= 0 and i + x < size and j + y >= 0 and j + y < size):
                    adjacentCells[hash].append(Coordinate(i + x, j + y))


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
        legalAction = LegalAction(atype, index, dir_1, dir_2, grid, playerUnits, opponentUnits)
        legalActions.append(legalAction)
        print("Load legal action " + legalAction.toStr() + " score:" + str(legalAction.getScore()), file=sys.stderr, flush=True)
        
    bestScore = -999999
    for i in range(len(legalActions)):
        score = legalActions[i].getScore()
        if (score > bestScore):
            bestScore = score
            bestAction = legalActions[i]
            print("Found bestAction score: " + str(bestScore) + " action: " + bestAction.toStr(), file=sys.stderr, flush=True)

    print(bestAction.toStr())
