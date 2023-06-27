class Floor {
    blockedIndex = 0;
    blocked = [];
    exitPos = -1;
    elevatorPos = -1;
    level = -1;
    width = -1;

    constructor(level, width) {
        this.level = level;
        this.width = width;
    }

    get isExit() {
        return this.exitPos >= 0;
    }

    *getBlocked() {
        for (let i = 0; i < this.blocked.length; i++) {
            yield this.blocked[i];
        }
    }

    setExitPos(pos) {
        this.exitPos = pos;
    }

    setElevatorPos(pos) {
        this.elevatorPos = pos;
    }

    setBlockedPos(pos) {
        this.blocked.push(pos);
        this.blockedIndex++;
    }
}

var floors = [];

function isCloneHeadedForExit(floor, pos, direction) {
    if (!floor.isExit) {
        return false;
    }

    if (direction === 'LEFT') {
        return pos > floor.exitPos;
    }

    return pos < floor.exitPos;
}

function isCloneHeadedForElevator(floor, pos, direction) {
    if (direction === 'LEFT') {
        return pos >= floor.elevatorPos;
    }

    return pos <= floor.elevatorPos;
}

var inputs = readline().split(' ');
const nbFloors = parseInt(inputs[0]); // number of floors
const width = parseInt(inputs[1]); // width of the area
const nbRounds = parseInt(inputs[2]); // maximum number of rounds
const exitFloor = parseInt(inputs[3]); // floor on which the exit is found
const exitPos = parseInt(inputs[4]); // position of the exit on its floor
const nbTotalClones = parseInt(inputs[5]); // number of generated clones
const nbAdditionalElevators = parseInt(inputs[6]); // ignore (always zero)
const nbElevators = parseInt(inputs[7]); // number of elevators

for (let i = 0; i < nbFloors; i++) {
    floors.push(new Floor(i, width));
}

floors[exitFloor].setExitPos(exitPos);

for (let i = 0; i < nbElevators; i++) {
    var inputs = readline().split(' ');
    const elevatorFloor = parseInt(inputs[0]); // floor on which this elevator is found
    const elevatorPos = parseInt(inputs[1]); // position of the elevator on its floor
    floors[elevatorFloor].setElevatorPos(elevatorPos);
}

// game loop
while (true) {
    var inputs = readline().split(' ');
    const cloneFloor = parseInt(inputs[0]); // floor of the leading clone
    const clonePos = parseInt(inputs[1]); // position of the leading clone on its floor
    const direction = inputs[2]; // direction of the leading clone: LEFT or RIGHT

    // check if no active clones
    if (direction === 'NONE') {
        console.log('WAIT');
        continue;
    }

    // check if the leading clone is done
    if (floors[cloneFloor].isExit) {
        if (isCloneHeadedForExit(floors[cloneFloor], clonePos, direction)) {
            console.log('WAIT');
            continue;
        }

        // leading clone is on the exit floor but wrong direction
        console.log('BLOCK');
        floors[cloneFloor].setBlockedPos(clonePos);
        continue;
    }

    // leading clone needs to go to elevator
    if (isCloneHeadedForElevator(floors[cloneFloor], clonePos, direction)) {
        console.log('WAIT');
        continue;
    }

    // leading clone is on the elevator but wrong direction
    console.log('BLOCK');
    floors[cloneFloor].setBlockedPos(clonePos);
    continue;
}
