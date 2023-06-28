const HeadingInterval = 1;
const XBoundary = 16000;
const YBoundary = 9000;
const PlayerSpeed = 1000;
const ZombieSpeed = 400;
const DefaultSpeed = 0;

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

    findClosestHuman() {
        var minDistance = XBoundary + YBoundary;
        var closestHuman = null;
        this.myHumans.forEach(function (human) {
            if (human.dead) {
                return;
            }
            var distance = this.location.distanceTo(human.location);
            if (distance < minDistance) {
                minDistance = distance;
                closestHuman = human;
            }
        }.bind(this));

        return closestHuman;
    }

    step() {
        if (this.dead) {
            return;
        }
        var closestHuman = this.findClosestHuman();
        if (closestHuman instanceof Person) {
            this.location.moveToTarget(closestHuman.location, this.speed);
        }
    }

    kill() {
        if (this.dead) {
            return;
        }

        this.myHumans.forEach(function (human) {
            if (human.dead) {
                return;
            }
            var distance = this.location.distanceTo(human.location);
            if (distance == 0) {
                human.dead = true;
            }
        }.bind(this));
    }
}

var zombies = new Map();

class Player extends Person {
    targetId = -1;
    bestHeading = 0;
    myZombies = null;
    myHumans = null;

    constructor(id, x, y) {
        super(id, x, y);
        this.speed = PlayerSpeed;
        this.myZombies = zombies;
        this.myHumans = humans;
    }

    findClosestZombie() {
        var minDistance = XBoundary + YBoundary;
        var closestZombie = null;
        this.myZombies.forEach(function (zombie) {
            if (zombie.dead) {
                return;
            }
            var distance = this.location.distanceTo(zombie.location);
            if (distance < minDistance) {
                minDistance = distance;
                closestZombie = zombie;
            }
        }.bind(this));

        return closestZombie;
    }

    fibbonacci(n) {
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

    findScoreThisTurn(killCount) {
        // Scoring works as follows:
        //   A zombie is worth the number of humans still alive squared x10, not including Ash.
        //  If several zombies are destroyed during on the same round, the nth zombie killed's worth is multiplied by the (n+2)th number of the Fibonnacci sequence (1, 2, 3, 5, 8, and so on). As a consequence, you should kill the maximum amount of zombies during a same turn.

        var aliveHumans = 0;
        this.myHumans.forEach(function (human) {
            if (!human.dead) {
                aliveHumans++;
            }
        });
        return Math.pow(aliveHumans, 2) * 10 * this.fibbonacci(killCount);
    }

    simulateAssault(target) {
        console.error("simulateAssault A");
        var clonePlayer = new Player(this.id, this.location.x, this.location.y);
        clonePlayer.targetId = target.id;

        console.error("simulateAssault B");
        var cloneHumans = new Map(humans);
        cloneHumans.set(clonePlayer.id, clonePlayer);
        humans.forEach(function (human) {
            var cloneHuman = new Person(human.id, human.location.x, human.location.y);
            cloneHuman.dead = human.dead;
            cloneHumans.set(cloneHuman.id, cloneHuman);
        }.bind(this));

        console.error("simulateAssault C");
        var cloneZombies = new Map(zombies);
        zombies.forEach(function (zombie) {
            var cloneZombie = new Zombie(zombie.id, zombie.location.x, zombie.location.y);
            cloneZombie.dead = zombie.dead;
            cloneZombie.myHumans = cloneHumans;
            cloneZombies.set(zombie.id, cloneZombie);
        }.bind(this));

        console.error("simulateAssault D");
        clonePlayer.myZombies = cloneZombies;
        clonePlayer.myHumans = cloneHumans;

        var killCount = 0;
        console.error("simulateAssault E");
        while (killCount < 1) {
            console.error("simulateAssault F");
            cloneZombies.forEach(function (zombie) {
                zombie.step();
            });
            clonePlayer.step();
            killCount = clonePlayer.kill();
            cloneZombies.forEach(function (zombie) {
                zombie.kill();
            });
        }

        console.error("simulateAssault Z");
        return this.findScoreThisTurn(killCount);
    }

    simulateMovement(heading) {
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

        var killCount = 0;
        while (killCount < 1 && clonePlayer.location.inBounds()) {
            cloneZombies.forEach(function (zombie) {
                zombie.step();
            });
            clonePlayer.move(heading);
            killCount = clonePlayer.kill();
            cloneZombies.forEach(function (zombie) {
                zombie.kill();
            });
        }

        return this.findScoreThisTurn(killCount);
    }

    updateTarget() {
        if (this.dead) {
            return;
        }

        var bestScore = 0;
        var bestTargetId = -1;
        console.error(" updateTarget A");
        this.myZombies.forEach(function (zombie) {
            if (zombie.dead) {
                return;
            }

            console.error(" updateTarget B");
            var thisScore = this.simulateAssault(zombie);
            console.error(" updateTarget C", thisScore, bestScore, zombie.id);
            if (thisScore > bestScore) {
                bestScore = thisScore;
                bestTargetId = zombie.id;
                console.error(" updateTarget D", bestTargetId);
            }
            
        }.bind(this));
        console.error(" updateTarget Z", bestTargetId);
        return bestTargetId;
    }

    updateAngle() {
        var bestScore = 0;
        for (var heading = 0; heading < 360; heading += HeadingInterval) {
            var thisScore = this.simulateMovement(heading);
            if (thisScore > bestScore) {
                bestScore = thisScore;
                this.bestHeading = heading;
            }
        }
    }

    step() {
        this.location.moveToTarget(this.myZombies.get(this.targetId).location, this.speed);
        console.error("step()", this.location.x + ' ' + this.location.y);
    }

    move(degree) {
        this.location.moveAngle(degree, this.speed);
        console.error("move(" + degree + ")", this.location.x + ' ' + this.location.y);
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
            if (distance <= 2000) {
                zombie.dead = true;
                killCount++;
            }
        }.bind(this));

        return killCount;
    }

    outputTurnTarget() {
        var bestTargetId = this.updateTarget();
        var myTarget = zombies.get(bestTargetId);
        if (myTarget instanceof Zombie) {
            console.log(myTarget.location.x + ' ' + myTarget.location.y + ' Gonna kill ' + bestTargetId);
        } else {
            console.log(this.location.x + ' ' + this.location.y + ' No Target ' + bestTargetId);
        }
    }

    outputTurnAngle() {
        this.updateAngle();

        var nextLocation = new Coordinate(this.location.x, this.location.y);
        nextLocation.moveAngle(this.bestHeading, this.speed);
        console.log(nextLocation.x + ' ' + nextLocation.y + ' My heading is ' + this.bestHeading);
    }
}

var player = new Player(-1, 0, 0);

// game loop
while (true) {
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
        zombies.set(zombieId, new Zombie(zombieId, zombieX, zombieY));
    }

    player.myZombies = zombies;
    player.myHumans = humans;
    humans.set(player.id, player);
    
    player.outputTurnAngle();
//    player.outputTurnTarget();
}
