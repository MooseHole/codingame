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
}

class Person {
    location = new Coordinate(0, 0);
    speed = 0;
    dead = false;

    constructor(id, x, y) {
        this.id = id;
        this.location.x = x;
        this.location.y = y;
    }
}

var humans = new Map();

class Zombie extends Person {
    constructor(id, x, y) {
        super(id, x, y);
        this.speed = 400;
    }

    findClosestHuman() {
        var minDistance = 30000;
        var closestHuman = null;
        humans.forEach(function (human) {
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

        humans.forEach(function (human) {
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
//new Map<string,{name:string,price:number}>();
var zombies = new Map();

class Player extends Person {
    target = null;

    constructor(id, x, y) {
        super(id, x, y);
        this.speed = 1000;
    }

    findClosestZombie() {
        var minDistance = 30000;
        var closestZombie = null;
        zombies.forEach(function (zombie) {
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

    updateTarget() {
        this.target = this.findClosestZombie();
    }

    step() {
        this.location.moveToTarget(this.target.location, this.speed);
    }

    kill() {
        if (this.dead) {
            return;
        }
        zombies.forEach(function (zombie) {
            if (zombie.dead) {
                return;
            }
            var distance = this.location.distanceTo(zombie.location);
            if (distance <= 2000) {
                zombie.dead = true;
            }
        }.bind(this));
    }

    outputTurn() {
        console.log(this.target.location.x + ' ' + this.target.location.y + ' Gonna kill ' + this.target.id);
    }
}

var player = new Player(0, 0, 0);

// game loop
while (true) {
    var inputs = readline().split(' ');
    const x = parseInt(inputs[0]);
    const y = parseInt(inputs[1]);

    player.location.x = x;
    player.location.y = y;

    const humanCount = parseInt(readline());
    for (let i = 0; i < humanCount; i++) {
        var inputs = readline().split(' ');
        const humanId = parseInt(inputs[0]);
        const humanX = parseInt(inputs[1]);
        const humanY = parseInt(inputs[2]);
        humans.set(humanId, new Person(humanId, humanX, humanY));
    }
    const zombieCount = parseInt(readline());
    for (let i = 0; i < zombieCount; i++) {
        var inputs = readline().split(' ');
        const zombieId = parseInt(inputs[0]);
        const zombieX = parseInt(inputs[1]);
        const zombieY = parseInt(inputs[2]);
        const zombieXNext = parseInt(inputs[3]);
        const zombieYNext = parseInt(inputs[4]);
        zombies.set(zombieId, new Zombie(zombieId, zombieX, zombieY));
    }

    // Find move
    player.updateTarget();

    // Step
    zombies.forEach(function (zombie) {
        zombie.step();
    });
    player.step();
    player.kill();
    zombies.forEach(function (zombie) {
        zombie.kill();
    });

    // Your destination coordinates
    player.outputTurn();
}
