const HeadingInterval = 1;
const MaxDepth = 10;
const XBoundary = 16000;
const YBoundary = 9000;
const PlayerSpeed = 1000;
const PlayerKillRange = 2000;
const ZombieSpeed = 400;
const ZombieKillRange = 0;
const DefaultSpeed = 0;

const TurnsToSimulate = 11; // max(XBoundary, YBoundary) / ZombieSpeed + 1
const MaxSavedScores = 3;
const MaxResolution = 1000;
const MinResolution = 1;
const ResolutionFactor = 10;

const ShowDebug = false;

class Coordinate {
    constructor(x, y) {
        this.x = x;
        this.y = y;
    }

    distanceTo(target) {
        return Math.sqrt(Math.pow(target.x - this.x, 2) + Math.pow(target.y - this.y, 2));
    }

    directionTo(target) {
        return Math.atan2(target.y - this.y, target.x - this.x);
    }

    degreeToRadians(degree) {
        return degree * Math.PI / 180;
    }

    moveToTarget(target, speed) {
        if (Math.ceil(this.distanceTo(target) <= speed)) {
            this.x = target.x;
            this.y = target.y;
        } else {
            var direction = this.directionTo(target);
            this.x += Math.floor(Math.cos(direction) * speed);
            this.y += Math.floor(Math.sin(direction) * speed);
        }
    }

    moveAngle(degree, speed) {
        var direction = this.degreeToRadians(degree);
        this.x += Math.floor(Math.cos(direction) * speed);
        this.y += Math.floor(Math.sin(direction) * speed);
    }

    // Returns the midpoint between this coordinate and other coordinate
    midpointTo(other) {
        return new Coordinate(Math.floor((this.x + other.x) / 2), Math.floor((this.y + other.y) / 2));
    }

    inBounds() {
        return this.x >= 0 && this.x <= XBoundary && this.y >= 0 && this.y <= YBoundary;
    }

    toString() {
        if (this.x < 0) {
            this.x = 0;
        } else if (this.x > XBoundary) {
            this.x = XBoundary;
        }

        if (this.y < 0) {
            this.y = 0;
        } else if (this.y > YBoundary) {
            this.y = YBoundary;
        }

        return this.x + " " + this.y;
    }
}

class Person {
    location = new Coordinate(0, 0);
    speed = DefaultSpeed;
    dead = false;

    constructor(id, x, y) {
        this.id = id;
        this.location.x = x;
        this.location.y = y;
    }
}

var humans = new Map();

class Zombie extends Person {
    myHumans = new Map();
    constructor(id, x, y) {
        super(id, x, y);
        this.speed = ZombieSpeed;
    }

    static findClosestHuman(zombie) {
        var minDistance = XBoundary + YBoundary;
        var closestHuman = null;
        zombie.myHumans.forEach(function (human) {
            if (human.dead) {
                return;
            }
            var distance = zombie.location.distanceTo(human.location);
            if (distance < minDistance) {
                minDistance = distance;
                closestHuman = human;
            }
        }.bind(zombie));

        return closestHuman;
    }

    step() {
        if (this.dead) {
            return;
        }
        var closestHuman = Zombie.findClosestHuman(this);
        if (closestHuman instanceof Person) {
            this.location.moveToTarget(closestHuman.location, this.speed);
        }
    }

    eat() {
        if (this.dead) {
            return;
        }

        var myKills = [];
        this.myHumans.forEach(function (human) {
            if (human.dead || human.id == -1) {
                return;
            }
            var distance = this.location.distanceTo(human.location);
            if (distance == 0) {
                myKills.push(human.id);
                human.dead = true;
            }
        }.bind(this));

        return myKills[0];
    }
}

var zombies = new Map();

var cache = new Map();

class Player extends Person {
    targetId = -1;
    bestHeading = 0;
    bestTargetId = -1;
    myZombies = null;
    myHumans = null;

    constructor(id, x, y) {
        super(id, x, y);
        this.speed = PlayerSpeed;
        this.myZombies = zombies;
        this.myHumans = humans;
    }

    static findCentroid(zombies) {
        var x = 0;
        var y = 0;
        var denominator = 0;

        zombies.forEach(function (zombie) {
            if (zombie.dead) {
                return;
            }
            x += zombie.location.x;
            y += zombie.location.y;
            denominator++;
        });
        if (denominator == 0) {
            return new Coordinate(0, 0);
        }
        return new Coordinate(x / denominator, y / denominator);
    }

    clone() {
        var clonePlayer = new Player(this.id, this.location.x, this.location.y);

        var cloneHumans = new Map(humans);
        cloneHumans.set(clonePlayer.id, clonePlayer);
        humans.forEach(function (human) {
            var cloneHuman = new Person(human.id, human.location.x, human.location.y);
            cloneHuman.dead = human.dead;
            cloneHumans.set(cloneHuman.id, cloneHuman);
        }.bind(this));

        var cloneZombies = new Map(zombies);
        zombies.forEach(function (zombie) {
            var cloneZombie = new Zombie(zombie.id, zombie.location.x, zombie.location.y);
            cloneZombie.dead = zombie.dead;
            cloneZombie.myHumans = cloneHumans;
            cloneZombies.set(zombie.id, cloneZombie);
        }.bind(this));

        clonePlayer.myZombies = cloneZombies;
        clonePlayer.myHumans = cloneHumans;

        return clonePlayer;
    }

    static findClosestZombie(player) {
        var minDistance = XBoundary + YBoundary;
        var closestZombie = null;
        player.myZombies.forEach(function (zombie) {
            if (zombie.dead) {
                return;
            }
            var distance = player.location.distanceTo(zombie.location);
            if (distance < minDistance) {
                minDistance = distance;
                closestZombie = zombie;
            }
        }.bind(player));

        return closestZombie;
    }

    static fibbonacci(n) {
        if (n <= 0) {
            return 0;
        }

        if (n == 1) {
            return 1;
        }

        if (n == 2) {
            return 2;
        }

        return this.fibbonacci(n - 1) + this.fibbonacci(n - 2);
    }

    static findScoreThisTurn(player, killCount) {
        // Scoring works as follows:
        //   A zombie is worth the number of humans still alive squared x10, not including Ash.
        //  If several zombies are destroyed during on the same round, the nth zombie killed's worth is multiplied by the (n+2)th number of the Fibonnacci sequence (1, 2, 3, 5, 8, and so on). As a consequence, you should kill the maximum amount of zombies during a same turn.

        var aliveHumans = 0;
        player.myHumans.forEach(function (human) {
            if (!human.dead && human.id != player.id) {
                aliveHumans++;
            }
        });

        return Math.pow(aliveHumans, 2) * 10 * Player.fibbonacci(killCount) ;// + aliveHumans * 1000;
    }

    static simulate(player, targetLocation) {
        var simulatedPlayer = player.clone();
        var maxScore = -99999999;

        for (var i = 0; i < TurnsToSimulate; i++) {
            simulatedPlayer.myZombies.forEach(function (zombie) {
                zombie.step();
            });
            simulatedPlayer.goToLocation(targetLocation);
            var killCount = simulatedPlayer.kill();
            simulatedPlayer.myZombies.forEach(function (zombie) {
                zombie.eat();
            });

            maxScore = Math.max(Player.findScoreThisTurn(simulatedPlayer, killCount), maxScore);
        }

        var aliveHumans = [];
        simulatedPlayer.myHumans.forEach(function (human) {
            if (!human.dead && human.id != simulatedPlayer.id) {
                aliveHumans.push(human.id);
            }
        });

        if (aliveHumans.length == 0) {
            return -999999999;
        }

        if (ShowDebug) {
            var stillLiving = "";
            aliveHumans.forEach(function (humanId) {
                stillLiving += humanId + ", ";
            });

            console.error("Score: " + maxScore + " Humans: " + stillLiving + " Evaulated: " + targetLocation);
        }

        return maxScore;
    }

    static simulateAssault(player, targetId) {
        var totalScore = 0;
        player.targetId = targetId;

        // If the zombie is closer to the target than the player, it's a bad idea to move towards it.
        var intercept = player.getIntercept(targetId);
        if (intercept.playerSteps > intercept.zombieSteps) {
            totalScore += TurnsToSimulate * -1000;
        }

        totalScore += intercept.playerSteps * -1000;

        totalScore += Player.simulate(player, intercept.bestLocation);

        return totalScore;
    }

    static simulateMovement(player, heading) {

        var killCount = 0;
        while (killCount < 1 && player.location.inBounds()) {
            player.myZombies.forEach(function (zombie) {
                zombie.step();
            });
            player.move(heading);
            killCount = player.kill();
            player.myZombies.forEach(function (zombie) {
                zombie.eat();
            });
        }

        return Player.findScoreThisTurn(player, killCount);
    }

    static updateTarget(player) {
        if (this.dead) {
            return;
        }

        var bestScore = -999999999;
        player.myZombies.forEach(function (zombie) {
            if (zombie.dead) {
                return;
            }

            var playerClone = player.clone();
            var thisScore = Player.simulateAssault(playerClone, zombie.id);
            if (thisScore > bestScore) {
                bestScore = thisScore;
                player.bestTargetId = zombie.id;
            }

        }.bind(player));

        return bestScore;
    }

    static updateAngle(player) {
        var bestScore = 0;
        for (var heading = 0; heading < 360; heading += HeadingInterval) {
            var playerClone = player.clone();
            var thisScore = Player.simulateMovement(playerClone, heading);
            if (thisScore > bestScore) {
                bestScore = thisScore;
                player.bestHeading = heading;
            }
        }
        return bestScore;
    }

    getIntercept(zombieId) {
        var zombie = this.myZombies.get(zombieId);
        var zombiesTarget = Zombie.findClosestHuman(zombie);

        var cacheKey = "getIntercept" + this.location + "," + zombie.location;
        if (zombiesTarget instanceof Person) {
            cacheKey += "," + zombiesTarget.id;
        }

        if (cache.has(cacheKey)) {
            return cache.get(cacheKey);
        }

        var bestLocation = new Coordinate(this.location.x, this.location.y);
        var bestNumSteps = 10000;
        var zombieSteps = 10000;

        var centerOfZombies = Player.findCentroid(this.myZombies);

        // Find intercept point between the zombie and the target.
        if (zombiesTarget instanceof Person) {
            var minDistanceToCenter = 100000;
            var distanceToCenter = 10000;
            var distanceToHuman = zombie.location.distanceTo(zombiesTarget.location);
            bestNumSteps = 0;
            zombieSteps = Math.ceil(distanceToHuman / zombie.speed) + 1;
            for (var zombieDistance = 0; zombieDistance <= distanceToHuman; zombieDistance += zombie.speed) {
                // For each location the zombie will be at
                var zombieLocation = new Coordinate(zombie.location.x, zombie.location.y);
                zombieLocation.moveToTarget(zombiesTarget.location, zombieDistance);

                // Find the number of steps the player will need to take to get to that location.
                var distanceToZombie = this.location.distanceTo(zombieLocation);
                var numSteps = Math.ceil((distanceToZombie - PlayerKillRange) / player.speed) + 1;

                // Find the distance from the center of the zombies to the zombie's location.
                distanceToCenter = centerOfZombies.distanceTo(zombieLocation);

                if (distanceToCenter < minDistanceToCenter) {
                    minDistanceToCenter = distanceToCenter;
                    bestLocation.x = zombieLocation.x;
                    bestLocation.y = zombieLocation.y;
                    bestNumSteps = numSteps;
                }
            }
        }

        cache.set(cacheKey, [bestLocation, bestNumSteps, zombieSteps]);

        return {
            'bestLocation': bestLocation,
            'playerSteps': bestNumSteps,
            'zombieSteps': zombieSteps
        }
    }

    getTargetLocation(zombieId) {
        var intercept = this.getIntercept(zombieId);
        return intercept.bestLocation;
    }

    step() {
        this.goToLocation(this.getTargetLocation(this.targetId));
    }

    goToLocation(location) {
        this.location.moveToTarget(location, this.speed);
    }

    move(degree) {
        this.location.moveAngle(degree, this.speed);
    }

    kill() {
        if (this.dead) {
            return;
        }

        var killCount = 0;
        this.myZombies.forEach(function (zombie) {
            if (zombie.dead) {
                return;
            }
            var distance = this.location.distanceTo(zombie.location);
            if (distance <= PlayerKillRange) {
                zombie.dead = true;
                killCount++;
            }
        }.bind(this));

        return killCount;
    }

    outputTurnTarget() {
        var myTarget = zombies.get(this.bestTargetId);
        if (myTarget instanceof Zombie) {
//            var myTargetLocation = this.getTargetLocation(myTarget.id)
//            console.log(myTargetLocation + ' Gonna kill ' + this.bestTargetId);
            console.log(myTarget.location + ' Gonna kill ' + this.bestTargetId);
        } else {
            console.log(this.location + ' No Target ' + this.bestTargetId);
        }
    }

    outputTurnAngle() {
        var nextLocation = new Coordinate(this.location.x, this.location.y);
        nextLocation.moveAngle(this.bestHeading, this.speed);
        console.log(nextLocation + ' My heading is ' + this.bestHeading);
    }

    outputTurn() {
        var anglePlayer = this.clone();
        var targetingPlayer = this.clone();
        var angleScore = Player.updateAngle(anglePlayer);
        var targetScore = Player.updateTarget(targetingPlayer);

        if (angleScore > targetScore) {
            anglePlayer.outputTurnAngle();
        } else {
            targetingPlayer.outputTurnTarget();
        }
    }

    takeTurn(depth = 0, angleScore = 0, targetScore = 0) {
        var anglePlayer = player.clone();
        var targetingPlayer = player.clone();
        var thisIterationAngleScore = Player.updateAngle(anglePlayer);
        var thisIterationTargetScore = Player.updateTarget(targetingPlayer);

        if (thisIterationAngleScore > thisIterationTargetScore) {
            angleScore += thisIterationAngleScore;

            if (depth > MaxDepth) {
                return angleScore;
            }

            var anglePlayerAliveZombies = false;
            anglePlayer.myZombies.forEach(function (zombie) {
                if (anglePlayerAliveZombies) {
                    return;
                }

                if (!zombie.dead) {
                    anglePlayerAliveZombies = true;
                }
            });

            if (anglePlayerAliveZombies) {
                angleScore += anglePlayer.takeTurn(depth + 1, angleScore, targetScore);
            }
        } else {
            targetScore += thisIterationTargetScore;

            if (depth > MaxDepth) {
                return targetScore;
            }

            var targetingPlayerAliveZombies = false;
            targetingPlayer.myZombies.forEach(function (zombie) {
                if (targetingPlayerAliveZombies) {
                    return;
                }

                if (!zombie.dead) {
                    targetingPlayerAliveZombies = true;
                }
            });

            if (targetingPlayerAliveZombies) {
                targetScore += targetingPlayer.takeTurn(depth + 1, angleScore, targetScore);
            }
        }

        if (depth == 0) {
            if (angleScore > targetScore) {
                anglePlayer.outputTurnAngle();
            } else {
                targetingPlayer.outputTurnTarget();
            }
        }

    }
}

var pointScores = new Map();

var player = new Player(-1, 0, 0);

class Tile {
    topLeft = new Coordinate(0, 0);
    center = new Coordinate(0, 0);
    bottomRight = new Coordinate(0, 0);
    resolution = MaxResolution;
    expanded = false;
    bestTiles = [];

    constructor(x, y, resolution) {
        this.topLeft.x = x;
        this.topLeft.y = y;
        this.center.x = Math.floor(x + resolution / 2);
        this.center.y = Math.floor(y + resolution / 2);
        this.bottomRight.x = x + resolution;
        this.bottomRight.y = y + resolution;
        this.resolution = resolution;
    }

    expand(newResolution) {
        if (!this.expanded && !this.isLowestResolution()) {
            for (var newX = this.topLeft.x; newX < this.bottomRight.x; newX += newResolution) {
                for (var newY = this.topLeft.y; newY < this.bottomRight.y; newY += newResolution) {
                    if ((new Coordinate(newX, newY)).inBounds()) {
                        var newTile = new Tile(newX, newY, newResolution);
                        this.addScore(newTile);
                    }
                }
            }
        }

        this.expanded = true;
    }

    isLowestResolution() {
        return this.resolution == MinResolution;
    }

    addScore(newTile) {
        var score = newTile.findScore();
        for (var index = 0; index < MaxSavedScores; index++) {
            if (this.bestTiles.length < index + 1) {
                this.bestTiles.push(newTile);
                return true;
            }

            if (score > this.bestTiles[index].findScore()) {
                this.bestTiles.splice(index, 0, newTile);
                this.bestTiles.splice(MaxSavedScores, 1);
                return true;
            }
        }
    }

    findScore() {
        var key = this.center.x * 10000 + this.center.y;

        if (!(pointScores.has(key))) {
            var score = Player.simulate(player, this.center);
            pointScores.set(key, score);
        }

        return pointScores.get(key);
    }

    printBestTiles() {
        for (var index = 0; index < this.bestTiles.length; index++) {
            if (!this.isLowestResolution()) {
                this.bestTiles[index].printBestTiles();
            }
        }
    }

    getBestTile() {
        if (this.isLowestResolution()) {
            return this;
        }

        if (!this.expanded) {
            this.expand(this.resolution / ResolutionFactor);
        }
        
        var bestScore = -999999999;
        var bestTile = null;
        for (var index = 0; index < this.bestTiles.length; index++) {
            var thisScore = this.bestTiles[index].findScore();
            if (thisScore > bestScore) {
                bestScore = thisScore;
                bestTile = this.bestTiles[index];
            }

            return bestTile.getBestTile();
        }

        return this;
    }

    toString() {
        return "Tile({" + this.center + "} {" + this.resolution + "})";
    }
}

// game loop
while (true) {
    cache = new Map();
    pointScores = new Map();

    var inputs = readline().split(' ');
    const x = parseInt(inputs[0]);
    const y = parseInt(inputs[1]);

    player.location.x = x;
    player.location.y = y;

    const humanCount = parseInt(readline());
    humans = new Map();
    for (let i = 0; i < humanCount; i++) {
        var inputs = readline().split(' ');
        const humanId = parseInt(inputs[0]);
        const humanX = parseInt(inputs[1]);
        const humanY = parseInt(inputs[2]);
        humans.set(humanId, new Person(humanId, humanX, humanY));
    }
    const zombieCount = parseInt(readline());
    zombies = new Map();
    for (let i = 0; i < zombieCount; i++) {
        var inputs = readline().split(' ');

        const zombieId = parseInt(inputs[0]);
        const zombieX = parseInt(inputs[1]);
        const zombieY = parseInt(inputs[2]);
        const zombieXNext = parseInt(inputs[3]);
        const zombieYNext = parseInt(inputs[4]);
        var thisZombie = new Zombie(zombieId, zombieX, zombieY);
        thisZombie.myHumans = humans;
        zombies.set(zombieId, thisZombie);
    }

    player.myZombies = zombies;
    player.myHumans = humans;
    humans.set(player.id, player);

    //    player.outputTurnAngle();
    //    player.outputTurnTarget();
    //    player.outputTurn();
    //    player.takeTurn();

    var board = new Tile(0, 0, 16000);
    board.expand(MaxResolution);
    var bestTile = board.getBestTile();
    console.log(bestTile.center + ' ' + bestTile.findScore());
}
