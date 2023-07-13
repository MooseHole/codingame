const HeadingInterval = 1;
const MaxDepth = 10;
const XBoundary = 16000;
const YBoundary = 9000;
const PlayerSpeed = 1000;
const PlayerKillRange = 2000;
const ZombieSpeed = 400;
const ZombieKillRange = 0;
const DefaultSpeed = 0;
const HighNumber = 99999999;
const LowNumber = -99999999;
const PlayerId = -1;

const TurnsToSimulate = 21; // (max(XBoundary, YBoundary) / ZombieSpeed) + 1 = 41
const MaxSavedScores = 3; // ideal 3
const MaxResolution = 1000;
const MinResolution = 10;
const ResolutionFactor = 10;

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
        if (Math.ceil(this.distanceTo(target) <= speed)) {
            this.x = target.x;
            this.y = target.y;
        } else {
            var direction = this.directionTo(target);
            this.x += Math.floor(Math.cos(direction) * speed);
            this.y += Math.floor(Math.sin(direction) * speed);
        }
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
    kiting = false;

    constructor(id, x, y) {
        super(id, x, y);
        this.speed = ZombieSpeed;
        this.kiting = false;
    }

    static findClosestHuman(zombie) {
        var minDistance = XBoundary + YBoundary;
        var closestHuman = null;
        var myLocation = zombie.location;
        zombie.myHumans.forEach(function (human) {
            if (human.dead) {
                return;
            }
            var distance = myLocation.distanceTo(human.location);
            if (distance < minDistance) {
                minDistance = distance;
                closestHuman = human;
            }
        });

        if (closestHuman instanceof Person) {
            zombie.kiting = closestHuman.id == PlayerId;
        }

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

        var myLocation = this.location;
        this.myHumans.forEach(function (human) {
            if (human.dead || human.id == PlayerId) {
                return;
            }
            var distance = myLocation.distanceTo(human.location);
            if (distance == 0) {
                human.dead = true;
            }
        });
    }
}

var zombies = new Map();

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

    clone() {
        var clonePlayer = new Player(this.id, this.location.x, this.location.y);

        var cloneHumans = new Map(humans);
        cloneHumans.set(clonePlayer.id, clonePlayer);
        humans.forEach(function (human) {
            var cloneHuman = new Person(human.id, human.location.x, human.location.y);
            cloneHuman.dead = human.dead;
            cloneHumans.set(cloneHuman.id, cloneHuman);
        });

        var cloneZombies = new Map(zombies);
        zombies.forEach(function (zombie) {
            var cloneZombie = new Zombie(zombie.id, zombie.location.x, zombie.location.y);
            cloneZombie.dead = zombie.dead;
            cloneZombie.myHumans = cloneHumans;
            cloneZombies.set(zombie.id, cloneZombie);
        });

        clonePlayer.myZombies = cloneZombies;
        clonePlayer.myHumans = cloneHumans;

        return clonePlayer;
    }

    static findScoreThisTurn(survivorCount, counts, useRealScore) {
        // Scoring works as follows:
        //   A zombie is worth the number of humans still alive squared x10, not including Ash.
        //  If several zombies are destroyed during on the same round, the nth zombie killed's worth is multiplied by the (n+2)th number of the Fibonnacci sequence (1, 2, 3, 5, 8, and so on). As a consequence, you should kill the maximum amount of zombies during a same turn.

        var thisScore = 0;
        if (survivorCount == 0) {
            thisScore = LowNumber;
        } else {
            var realScore = Math.pow(survivorCount, 2) * 10 * killScore(counts.killCount);

            if (useRealScore) {
                thisScore = realScore;
            } else {
                thisScore = realScore; // + counts.kiteCount;
            }

            if (/*useRealScore &&*/ counts.kiteCount > 1 && counts.kiteDistance < PlayerSpeed) {
                thisScore += counts.kiteCount * 1000;
            }
        }
        
        return {
            'maxScore': thisScore,
            'survivorCount': survivorCount,
            'killCount': counts.killCount,
            'kiteCount': counts.kiteCount,
            'aliveCount': counts.aliveCount,
            'kiteDistance': counts.kiteDistance
          };
    }

    static canSaveAnyHuman(player) {
        for (let human of player.myHumans.values()) {
            if (human instanceof Person) {
                if (human.dead || human.id == PlayerId) {
                    continue;
                }

                var playerSteps = Math.floor(Math.max(0, human.location.distanceTo(player.location) - PlayerKillRange) / PlayerSpeed);
                var zombieSteps = 99999999;
                var closestZombie = -1;
                for (let zombie of player.myZombies.values()) {
                    if (zombie instanceof Zombie) {
                        if (zombie.dead) {
                            continue;
                        }

                        var thisZombieSteps = Math.floor(Math.max(0, human.location.distanceTo(zombie.location)) / ZombieSpeed);
                        if (thisZombieSteps < zombieSteps) {
                            zombieSteps = thisZombieSteps;
                            closestZombie = zombie.id;
                        }
                    }
                }

                if (playerSteps < zombieSteps) {
                    return true;
                }
            }
        }
 
        return false;
    }

    static simulate(player, targetLocation) {
        var simulatedPlayer = player.clone();
        var score = Player.findScoreThisTurn(0, {
            'killCount': 0,
            'kiteCount': 0,
            'aliveCount': 0,
            'kiteDistance': 0
          }, true);
          score.maxScore = LowNumber - 1;
          var maxScore = score;

        for (var simulatedSteps = 0; simulatedSteps < TurnsToSimulate; simulatedSteps++) {
            simulatedPlayer.myZombies.forEach(function (zombie) {
                zombie.step();
            });
            simulatedPlayer.goToLocation(targetLocation);
            var counts = simulatedPlayer.kill();
            simulatedPlayer.myZombies.forEach(function (zombie) {
                zombie.eat();
            });

            var survivorCount = 0;
            simulatedPlayer.myHumans.forEach(function (human) {
                if (!human.dead && human.id != simulatedPlayer.id) {
                    survivorCount++;
                }
            });

            if (simulatedSteps == 0 && survivorCount > 0 && counts.aliveCount == 0) {
                score = Player.findScoreThisTurn(survivorCount, counts, simulatedSteps == 0);
                score.maxScore = HighNumber;
                break;
            }

            if (simulatedPlayer.location.x == targetLocation.x && simulatedPlayer.location.y == targetLocation.y) {
                if (!Player.canSaveAnyHuman(simulatedPlayer)) {
                    break;
                }

                score = Player.findScoreThisTurn(survivorCount, counts, simulatedSteps == 0);
                if (maxScore.maxScore < score.maxScore) {
                    maxScore = score;
                }
            }
        }

        var survivorCount = 0;
        simulatedPlayer.myHumans.forEach(function (human) {
            if (!human.dead && human.id != simulatedPlayer.id) {
                survivorCount++;
            }
        });
        maxScore.maxScore += survivorCount * 10;

        return maxScore;
    }

    goToLocation(location) {
        this.location.moveToTarget(location, this.speed);
    }

    kill() {
        if (this.dead) {
            return;
        }

        var killCount = 0;
        var kiteCount = 0;
        var aliveCount = 0;
        var myLocation = this.location;
        var kiteCenter = new Coordinate(0, 0);
        var kiteLocations = [];
        this.myZombies.forEach(function (zombie) {
            if (zombie instanceof Zombie) {
                if (zombie.dead) {
                    return;
                }
                var distance = myLocation.distanceTo(zombie.location);
                if (distance <= PlayerKillRange) {
                    zombie.dead = true;
                    killCount++;
                }
                else if (zombie.kiting) {
                    kiteLocations.push(zombie.location);
                    kiteCount++;
                }

                if (!zombie.dead) {
                    aliveCount++;
                }
            }
        });

        kiteLocations.forEach(function (location) {
            kiteCenter.x += location.x;
            kiteCenter.y += location.y;
        });

        kiteCenter.x = Math.floor(kiteCenter.x / kiteCount);
        kiteCenter.y = Math.floor(kiteCenter.y / kiteCount);

        var kiteDistance = 0;

        kiteLocations.forEach(function (location) {
            kiteDistance += location.distanceTo(kiteCenter);
        });

        if (kiteCount > 0) {
            kiteDistance /= kiteCount;
        }

        return {
            'killCount': killCount,
            'kiteCount': kiteCount,
            'aliveCount': aliveCount,
            'kiteDistance': kiteDistance
          };
    }
}

var pointScores = new Map();

var player = new Player(PlayerId, 0, 0);

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

    expandAt(newResolution) {
        if (!this.expanded && !this.isLowestResolution()) {
            for (var newX = this.topLeft.x; newX < this.bottomRight.x; newX += newResolution) {
                for (var newY = this.topLeft.y; newY < this.bottomRight.y; newY += newResolution) {
                    var newTile = new Tile(newX, newY, newResolution);
                    this.addScore(newTile);
                }
            }
        }

        this.expanded = true;
    }

    expand() {
        this.expandAt(this.resolution / ResolutionFactor);
    }

    isLowestResolution() {
        return this.resolution == MinResolution;
    }

    addScore(newTile) {
        if (this.bestTiles.length == 0) {
            this.bestTiles.push(newTile);
            return;
        }

        var score = newTile.findScore().maxScore;
        if (this.bestTiles[0].findScore().maxScore < score) {
            // Insert at the beginning of the list and remove the rest
            this.bestTiles = [newTile];
            return;
        } else if (this.bestTiles[0].findScore().maxScore == score && this.bestTiles.length < MaxSavedScores) {
            // Add to the list
            this.bestTiles.push(newTile);
            return;
        } 

        return;
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
            console.error(this.bestTiles[index].toString());
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
            this.expand();
        }
        
        var bestTile = this.bestTiles[0].getBestTile();
/*
        var bestTile = null;*/
        var bestScore = LowNumber;
        for (var index = 0; index < this.bestTiles.length; index++) {
            var thisScore = this.bestTiles[index].getBestTile().findScore();
            if (thisScore > bestScore) {
                bestScore = thisScore;
                bestTile = this.bestTiles[index].getBestTile();
            }
        }

        return bestTile.getBestTile();
    }

    toString() {
        return "Tile({" + this.center + "} {" + this.resolution + "} {" + this.findScore() + "})";
    }

    outputMainTile() {
        this.expandAt(MaxResolution);
        var bestTile = this.getBestTile();

        // this.printBestTiles();
        var foundScore = bestTile.findScore();
        var score = foundScore.maxScore;
        var quote = "";
        if (score == HighNumber) {
            quote = " Hail 2 The King Baby";
        } else if (score > 3500) {
            quote = " This Is My Boomstick";
        } else if (score > 2000) {
            quote = " Groovy              ";
        } else if (score > 500) {
            quote = " Come Get Some       ";
        } else if (score > 250) {
            quote = " Gimme Sum Sugar Baby";
        } else if (score > 100) {
            quote = " Shop Smrt Shop S-Mrt";
        } else if (score >= 0) {
            quote = " Everybody Dies Here ";
        } else if (score < 0) {
            quote = " Klaatu Barada N...  ";
        }

        console.log(bestTile.center + quote + ' ' + foundScore.maxScore + ' sur:' + foundScore.survivorCount + ' kill:' + foundScore.killCount + ' kite:' + foundScore.kiteCount + ' alive:' + foundScore.aliveCount + ' kiteSpread:' + foundScore.kiteDistance);
    }
}


var fibonnacciCache = new Map();
var killScoreCache = new Map();

function fibonnacci(n) {
    if (!fibonnacciCache.has(n)) {
        fibonnacciCache.set(n, fibonnacci(n - 1) + fibonnacci(n - 2));
    }

    return fibonnacciCache.get(n);
}

fibonnacciCache.set(0, 0);
fibonnacciCache.set(1, 1);
fibonnacciCache.set(2, 2);
fibonnacciCache.set(3, 3);
fibonnacciCache.set(4, 5);
fibonnacciCache.set(5, 8);
fibonnacciCache.set(6, 13);
fibonnacciCache.set(7, 21);
fibonnacciCache.set(8, 34);
fibonnacciCache.set(9, 55);
fibonnacciCache.set(10, 89);
fibonnacciCache.set(11, 144);
fibonnacciCache.set(12, 233);
fibonnacciCache.set(13, 277);
fibonnacciCache.set(14, 610);
fibonnacciCache.set(15, 987);
fibonnacciCache.set(16, 1597);
fibonnacciCache.set(17, 2584);
fibonnacciCache.set(18, 4181);
fibonnacciCache.set(19, 6765);
fibonnacciCache.set(20, 10946);
fibonnacciCache.set(21, 17711);
fibonnacciCache.set(22, 28657);
fibonnacciCache.set(23, 46368);
fibonnacciCache.set(24, 75025);
fibonnacciCache.set(25, 121393);
fibonnacciCache.set(26, 196418);
fibonnacciCache.set(27, 317811);
fibonnacciCache.set(28, 514229);
fibonnacciCache.set(29, 832040);
fibonnacciCache.set(30, 1346269);
fibonnacciCache.set(31, 2178309);
fibonnacciCache.set(32, 3524578);
fibonnacciCache.set(33, 5702887);
fibonnacciCache.set(34, 9227465);
fibonnacciCache.set(35, 14930352);
fibonnacciCache.set(36, 24157817);
fibonnacciCache.set(37, 39088169);
fibonnacciCache.set(38, 63245986);
fibonnacciCache.set(39, 102334155);
fibonnacciCache.set(40, 165580141);
fibonnacciCache.set(41, 267914296);
fibonnacciCache.set(42, 433494437);
fibonnacciCache.set(43, 701408733);
fibonnacciCache.set(44, 1134903170);
fibonnacciCache.set(45, 1836311903);
fibonnacciCache.set(46, 2971215073);
fibonnacciCache.set(47, 4807526976);
fibonnacciCache.set(48, 7778742049);
fibonnacciCache.set(49, 12586269025);
fibonnacciCache.set(50, 20365011074);

killScoreCache.set(0, 0);
function killScore(n) {
    if (!killScoreCache.has(n)) {
        killScoreCache.set(n, killScore(n - 1) + fibonnacci(n));
    }

    return killScoreCache.get(n);
}

for (var i = 0; i < 40; i++) {
    killScore(i);
}

// game loop
while (true) {
    pointScores = new Map();
    humans = new Map();
    zombies = new Map();
    var board = new Tile(0, 0, XBoundary);
    board.bottomRight.x = XBoundary;
    board.bottomRight.y = YBoundary;

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
        var thisZombie = new Zombie(zombieId, zombieX, zombieY);
        thisZombie.myHumans = humans;
        zombies.set(zombieId, thisZombie);
    }

    player.myZombies = zombies;
    player.myHumans = humans;
    humans.set(player.id, player);

    board.outputMainTile();
}
