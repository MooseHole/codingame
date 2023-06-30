const HeadingInterval = 1;
const MaxDepth = 10;
const XBoundary = 16000;
const YBoundary = 9000;
const PlayerSpeed = 1000;
const PlayerKillRange = 2000;
const ZombieSpeed = 400;
const ZombieKillRange = 0; //400;
const DefaultSpeed = 0;
const TurnsToSimulate = 50;

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
        if (Math.floor(this.distanceTo(target) <= speed)) {
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
            if (human.dead) {
                return;
            }
            var distance = this.location.distanceTo(human.location);
            if (distance <= ZombieKillRange) {
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

        if (aliveHumans == 0) {
            return -9000000;
        }

        return Math.pow(aliveHumans, 2) * 10 * Player.fibbonacci(killCount) + aliveHumans * 1000;
    }

    static simulateAssault(player, targetId) {
        var killCount = 0;
        var totalScore = 0;
        player.targetId = targetId;

        // If the zombie is closer to the target than the player, it's a bad idea to move towards it.
        var intercept = player.getIntercept(targetId);
        if (intercept.playerSteps > intercept.zombieSteps) {
            totalScore += TurnsToSimulate * -1000;
        } else if (intercept.playerSteps == intercept.zombieSteps) {
            totalScore += TurnsToSimulate * 1000;
        }

        totalScore += intercept.playerSteps * -1000;

        for (var i = 0; i < TurnsToSimulate; i++) {
            player.myZombies.forEach(function (zombie) {
                zombie.step();
            });
            player.step();
            killCount = player.kill();
            player.myZombies.forEach(function (zombie) {
                zombie.eat();
            });

            totalScore += Player.findScoreThisTurn(player, killCount);
        }

        var aliveHumans = 0;
        player.myHumans.forEach(function (human) {
            if (!human.dead && human.id != player.id) {
                aliveHumans++;
            }
        });

        if (aliveHumans == 0) {
            totalScore = -9999999999;
        }

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

        var bestScore = 0;
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

        var cacheKey = "getIntercept"+this.location.x+","+this.location.y+","+zombie.location.x+","+zombie.location.y;
        if (zombiesTarget instanceof Person) {
            cacheKey += ","+zombiesTarget.id;
        }

        if (cache.has(cacheKey)) {
            return cache.get(cacheKey);
        }

        var bestLocation = this.location;
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
            for (var zombieDistance = 0; zombieDistance <= (distanceToHuman - ZombieKillRange); zombieDistance += zombie.speed) {
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

        cache[cacheKey] = [bestLocation, bestNumSteps, zombieSteps];

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
        this.location.moveToTarget(this.getTargetLocation(this.targetId), this.speed);
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
        Player.updateTarget(this);

        var myTarget = zombies.get(this.bestTargetId);
        if (myTarget instanceof Zombie) {
            var myTargetLocation = this.getTargetLocation(myTarget.id)
            console.log(myTargetLocation.x + ' ' + myTargetLocation.y + ' Gonna kill ' + this.bestTargetId);
        } else {
            console.log(this.location.x + ' ' + this.location.y + ' No Target ' + this.bestTargetId);
        }
    }

    outputTurnAngle() {
        var nextLocation = new Coordinate(this.location.x, this.location.y);
        nextLocation.moveAngle(this.bestHeading, this.speed);
        console.log(nextLocation.x + ' ' + nextLocation.y + ' My heading is ' + this.bestHeading);
    }

    outputTurn() {
        var anglePlayer = this.clone();
        var targetingPlayer = this.clone();
        var angleScore = Player.updateAngle(anglePlayer);
        var targetScore = Player.updateTarget(targetingPlayer);

        var aliveZombies = 0;
        player.myZombies.forEach(function (zombie) {
            if (!zombie.dead) {
                aliveZombies++;
            }
        });

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

var player = new Player(-1, 0, 0);

// game loop
while (true) {
    cache = new Map();

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
    player.outputTurnTarget();
//    player.outputTurn();
//    player.takeTurn();
}
