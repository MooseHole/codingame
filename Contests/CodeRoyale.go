package main

import (
	"fmt"
	"math"
	"os"
	"sort"
	"strconv"
)

//import "os"

const worldMaxX int = 1920
const worldMaxY int = 1000
const touchingDelta int = 5
const simulateTurns int = 30
const queenRadius int = 30
const queenSpeed int = 60
const queenMass int = 10000
const knightRadius int = 20
const knightSpeed int = 100
const knightMass int = 400
const knightHealth int = 30
const knightCost int = 80
const knightProduction int = 4
const knightBuildTime int = 5
const archerRadius int = 25
const archerSpeed int = 75
const archerMass int = 900
const archerRange int = 200
const archerHealth int = 45
const archerCost int = 100
const archerProduction int = 2
const archerBuildTime int = 8
const giantRadius int = 40
const giantSpeed int = 50
const giantMass int = 2000
const giantHealth int = 200
const giantCost int = 140
const giantProduction int = 1
const giantBuildTime int = 10
const towerInitialHP int = 200
const towerAddedHP int = 100
const towerMaxHP int = 800
const towerRangeCoefficient int = 1000
const approximateGoldAmount int = 250

type Coordinate struct {
	x      int
	y      int
	radius int
}

type Unit struct {
	Type     int
	Location Coordinate
	Owner    int
	Health   int
	Range    int
	Speed    int
	Mass     int
}

type Site struct {
	Id          int
	Location    Coordinate
	Gold        int
	MaxMineSize int
	Type        int
	Owner       int
	Param1      int
	Param2      int
	Cost        int
	Production  int
	BuildTime   int
}

var unitType map[int]string
var siteType map[int]string
var owner map[int]string
var units map[string][]Unit
var sites map[int]Site
var friendly Descriptor
var enemy Descriptor
var queen Descriptor
var tower Descriptor
var noStructure Descriptor
var noOwner Descriptor
var goldmine Descriptor
var barracks Descriptor
var knight Descriptor
var archer Descriptor
var giant Descriptor
var gold int
var touchedSite int
var myQueen Unit

func coordinateString(coordinate Coordinate) string {
	return "[" + strconv.Itoa(coordinate.x) + ", " + strconv.Itoa(coordinate.y) + "(" + strconv.Itoa(coordinate.radius) + ")" + "]"
}

func unitGroupIdentifier(unit Unit) string {
	return owner[unit.Owner] + unitType[unit.Type]
}

func unitString(unit Unit) string {
	return "[" + owner[unit.Owner] + " " + unitType[unit.Type] + coordinateString(unit.Location) + strconv.Itoa(unit.Health) + "]"
}

func barracksTurnsRemaining(s Site) int {
	if s.Type == barracks.index {
		return s.Param1
	}
	return -1
}

func barracksUnitType(s Site) int {
	if s.Type == barracks.index {
		return s.Param2
	}
	return -1
}

func towerHP(s Site) int {
	if s.Type == tower.index {
		return s.Param1
	}
	return -1
}

func towerAttackRadius(s Site) int {
	if s.Type == tower.index {
		return s.Param2
	}
	return -1
}

func goldmineIncome(s Site) int {
	if s.Type == goldmine.index {
		return s.Param1
	}
	return -1
}

func siteString(site Site) string {
	returnString := "[" + strconv.Itoa(site.Id) + " " + owner[site.Owner] + " " + siteType[site.Type] + coordinateString(site.Location)
	if site.Type == barracks.index {
		returnString += unitType[barracksUnitType(site)] + " Turns:" + strconv.Itoa(barracksTurnsRemaining(site))
	}
	if site.Type == tower.index {
		returnString += " HP:" + strconv.Itoa(towerHP(site)) + " Radius:" + strconv.Itoa(towerAttackRadius(site))
	}
	if site.Type == goldmine.index {
		returnString += " Income:" + strconv.Itoa(goldmineIncome(site)) + " Max:" + strconv.Itoa(site.MaxMineSize) + " Left:" + strconv.Itoa(site.Gold)
	}
	returnString += "]"
	return returnString
}

func clearUnits() {
	units = make(map[string][]Unit)
}

func initializeMaps() {
	owner = make(map[int]string)
	unitType = make(map[int]string)
	siteType = make(map[int]string)
	units = make(map[string][]Unit)
	sites = make(map[int]Site)
	distanceMap = make(map[string]float64)
}

type Descriptor struct {
	index       int
	description string
}

func initializeDescriptors() {
	friendly.index = 0
	friendly.description = "Friendly"
	enemy.index = 1
	enemy.description = "Enemy"
	noOwner.index = -1
	noOwner.description = "None"
	queen.index = -1
	queen.description = "QUEEN"
	tower.index = 1
	tower.description = "TOWER"
	goldmine.index = 0
	goldmine.description = "MINE"
	barracks.index = 2
	barracks.description = "BARRACKS"
	noStructure.index = -1
	noStructure.description = "No Structure"
	knight.index = 0
	knight.description = "KNIGHT"
	archer.index = 1
	archer.description = "ARCHER"
	giant.index = 2
	giant.description = "GIANT"
}

func initializeStrings() {
	unitType[queen.index] = queen.description
	unitType[knight.index] = knight.description
	unitType[archer.index] = archer.description
	unitType[giant.index] = giant.description
	owner[noOwner.index] = noOwner.description
	owner[friendly.index] = friendly.description
	owner[enemy.index] = enemy.description
	siteType[noStructure.index] = noStructure.description
	siteType[goldmine.index] = goldmine.description
	siteType[tower.index] = tower.description
	siteType[barracks.index] = barracks.description
}

func initialize() {
	initializeDescriptors()
	initializeMaps()
	initializeStrings()
	gold = 0
	touchedSite = -1
}

func distance(start, end Coordinate) int {
	return distanceSpecifyRadius(start, start.radius, end, end.radius)
}

func distanceSpecifyRadius(start Coordinate, startRadius int, end Coordinate, endRadius int) int {
	return int(distanceSpecifyRadiusFloat(start, float64(startRadius), end, float64(endRadius)))
}

func distanceSpecifyRadiusFloat(start Coordinate, startRadius float64, end Coordinate, endRadius float64) float64 {
	return distancePure(start.x, start.y, end.x, end.y) - startRadius - endRadius
}

var distanceMap map[string]float64

func distancePure(startX int, startY int, endX int, endY int) float64 {
	if startX == endX && startY == endY {
		return 0.0
	}
	distanceString := strconv.Itoa(startX) + ":" + strconv.Itoa(startY) + "|" + strconv.Itoa(endX) + ":" + strconv.Itoa(endY)
	dista, exists := distanceMap[distanceString]
	if !exists || dista < 0.000001 {
		dista = math.Sqrt(math.Pow(float64(startX-endX), 2) + math.Pow(float64(startY-endY), 2))
		distanceMap[distanceString] = dista
		//		fmt.Fprintln(os.Stderr, "Dist set", distanceString, dista)
	} else {
		//fmt.Fprintln(os.Stderr, "Dist exists", distanceString, dist)
	}
	//	fmt.Fprintln(os.Stderr, "Dist found", distanceString, dist)
	return dista
	//return math.Sqrt(math.Pow(float64(startX-endX), 2) + math.Pow(float64(startY-endY), 2))
}
func overlap(start Coordinate, end Coordinate) float64 {
	return float64(start.radius) + float64(end.radius) - distancePure(start.x, start.y, end.x, end.y)
}

func sameLocation(start, end Coordinate) bool {
	return start.x == end.x && start.y == end.y
}

type distanceSite struct {
	Distance int
	St       Site
}

type OwnerTypeDescriptors struct {
	Owner Descriptor
	Type  Descriptor
}

var distanceSites []distanceSite
var distanceSiteFrom Coordinate

func sortSitesByDistance(toUnit Unit, force bool) {
	// if distances already determined and storted
	if !force && toUnit.Location.x == distanceSiteFrom.x && toUnit.Location.y == distanceSiteFrom.y {
		return
	}

	distanceSites = distanceSites[:0]
	distanceSiteFrom = toUnit.Location
	for _, v := range sites {
		var thisDistanceSite distanceSite
		thisDistanceSite.Distance = distance(distanceSiteFrom, v.Location)
		thisDistanceSite.St = v
		distanceSites = append(distanceSites, thisDistanceSite)
	}

	sort.Slice(distanceSites, func(i, j int) bool {
		return distanceSites[i].Distance < distanceSites[j].Distance
	})
}

func getSiteByDistanceDescriptorIndex(toUnit Unit, descriptors []OwnerTypeDescriptors, index int, resort bool) (Site, error) {
	sortSitesByDistance(toUnit, resort)
	foundIndex := -1
	for _, s := range distanceSites {
		match := false
		for _, d := range descriptors {
			if s.St.Owner == d.Owner.index && s.St.Type == d.Type.index {
				match = true
				break
			}
		}
		if match {
			foundIndex++
			if foundIndex == index {
				return s.St, nil
			}
		}
	}

	var site Site
	return site, fmt.Errorf("getSiteByDistanceDescriptorIndex found no matching sites")
}

// stepTowerTarget searches stepunits returns the found unit, unit index, distance, and error
func stepTowerTarget(thisTower Site) (Unit, int, int, error) {
	var unit Unit
	if thisTower.Type != tower.index {
		return unit, 0, 0, fmt.Errorf("towerTarget: Site %d is not a tower", thisTower.Id)
	}

	leastDistance := 100000
	queenDistance := 100000
	var index int
	foundQueen := false
	foundOtherUnit := false
	for _, v := range stepunits {
		for i, u := range v {
			if u.Owner != thisTower.Owner {
				thisDistance := distance(thisTower.Location, u.Location)
				if thisDistance < towerAttackRadius(thisTower) {
					if u.Type == queen.index {
						foundQueen = true
						if !foundOtherUnit {
							queenDistance = thisDistance
							unit = u
							index = 0
						}
					} else if thisDistance < leastDistance {
						foundOtherUnit = true
						leastDistance = thisDistance
						unit = u
						index = i
					}
				}
			}
		}
	}

	if foundOtherUnit {
		return unit, index, leastDistance, nil
	} else if foundQueen {
		return unit, index, queenDistance, nil
	}

	return unit, 0, 0, fmt.Errorf("Tower %d found no units in range", thisTower.Id)
}

// The closest unit with the opposing owner to the given unit
func closestOpposingUnit(toUnit Unit) (Unit, int) {
	leastDistance := 100000
	var unit Unit
	var index int
	for _, v := range units {
		for i, u := range v {
			if u.Owner != toUnit.Owner {
				thisDistance := distance(toUnit.Location, u.Location)
				if thisDistance < leastDistance {
					leastDistance = thisDistance
					unit = u
					index = i
				}
			}
		}
	}

	return unit, index
}

func printEntities() {
	for _, v := range units {
		for _, u := range v {
			fmt.Fprintln(os.Stderr, unitString(u))
		}
	}

	for _, v := range sites {
		fmt.Fprintln(os.Stderr, siteString(v))
	}
}

var stepunits map[string][]Unit
var stepsites map[int]Site
var stepgold int
var simulationBuild Site
var simulationMove Coordinate
var simulationTrain int
var simulationBuilt bool
var simulationTrained bool

func moveToward(mover Unit, moverIndex int, destination Coordinate, moverRadius int) Coordinate {
	if distanceSpecifyRadius(mover.Location, moverRadius, destination, destination.radius) <= 0 {
		return mover.Location
	}

	var angleFinder Coordinate
	angleFinder.x = mover.Location.x - destination.x
	angleFinder.y = mover.Location.y - destination.y
	angle := math.Atan2(float64(angleFinder.y), float64(angleFinder.x))

	mover.Location.x -= int(float64(mover.Speed) * math.Cos(angle))
	mover.Location.y -= int(float64(mover.Speed) * math.Sin(angle))

	//fixCollisions()
	collision(unitGroupIdentifier(mover), moverIndex)
	return mover.Location
}

func stepQueen() {
	if !simulationTrained && simulationTrain >= 0 {
		trainSite := stepsites[simulationTrain]
		if trainSite.Owner == friendly.index && trainSite.Type == barracks.index && barracksTurnsRemaining(trainSite) == 0 {
			if stepgold >= trainSite.Cost {
				stepgold = stepgold - trainSite.Cost
				trainSite.Param1 = trainSite.BuildTime
				//				fmt.Fprintln(os.Stderr, "train", siteString(trainSite))
			}
		}
		stepsites[simulationTrain] = trainSite
		simulationTrained = true
	}

	q := stepunits[friendly.description+queen.description][0]
	// If build and move, wait until built before moving
	if simulationMove.x >= 0 && (simulationBuilt || simulationBuild.Id < 0) {
		q.Location = moveToward(q, 0, simulationMove, 0)
	}

	if !simulationBuilt && simulationBuild.Id >= 0 {
		q.Location = moveToward(q, 0, simulationBuild.Location, q.Location.radius)
		if distance(q.Location, simulationBuild.Location) <= 0 {
			thisSite := stepsites[simulationBuild.Id]
			if !(thisSite.Owner == enemy.index && thisSite.Type == tower.index) {
				thisSite.Owner = friendly.index
				if simulationBuild.Type == barracks.index {
					thisSite.Param2 = barracksUnitType(simulationBuild)
					if barracksUnitType(thisSite) == knight.index {
						thisSite.Cost = knightCost
						thisSite.Production = knightProduction
						thisSite.BuildTime = knightBuildTime
					}
					if barracksUnitType(thisSite) == archer.index {
						thisSite.Cost = archerCost
						thisSite.Production = archerProduction
						thisSite.BuildTime = archerBuildTime
					}
					if barracksUnitType(thisSite) == giant.index {
						thisSite.Cost = giantCost
						thisSite.Production = giantProduction
						thisSite.BuildTime = giantBuildTime
					}
				} else if simulationBuild.Type == tower.index {
					if thisSite.Type == tower.index {
						thisSite.Param1 += towerAddedHP
						if towerHP(thisSite) > towerMaxHP {
							thisSite.Param1 = towerMaxHP
						}
					} else {
						thisSite.Param1 = towerInitialHP
					}
					thisSite.Param2 = int(math.Sqrt(float64(towerHP(thisSite)*towerRangeCoefficient+thisSite.Location.radius) / math.Pi))
				} else if simulationBuild.Type == goldmine.index {
					// If already was a goldmine
					if thisSite.Type == goldmine.index {
						if goldmineIncome(thisSite) < thisSite.MaxMineSize {
							if goldmineIncome(thisSite) > 0 {
								thisSite.Param1++
							} else {
								thisSite.Param1 = 1
							}
						}
					} else {
						thisSite.Param1 = 1
						if thisSite.Gold < 0 {
							thisSite.Gold = approximateGoldAmount
						}
					}
				}
				thisSite.Type = simulationBuild.Type
				stepsites[simulationBuild.Id] = thisSite
				simulationBuilt = true
				//				fmt.Fprintln(os.Stderr, "built", stepsites[simulationBuild.Id])
			}
		}
	}

	stepunits[friendly.description+queen.description][0] = q
}

func stepSites() {
	for i, v := range stepsites {
		if v.Type == goldmine.index {
			if v.Owner == friendly.index {
				dug := int(math.Min(float64(v.Gold), float64(goldmineIncome(v))))
				stepgold = stepgold + dug
				v.Gold = v.Gold - dug

				if v.Gold <= 0 {
					var site Site
					site.Id = v.Id
					site.Type = noStructure.index
					site.Location.x = v.Location.x
					site.Location.y = v.Location.y
					site.Location.radius = v.Location.radius
				}

				stepsites[i] = v
			}
		} else if v.Type == barracks.index {
			if barracksTurnsRemaining(v) > 0 {
				newTurnsLeft := barracksTurnsRemaining(v) - 1
				if newTurnsLeft == 0 {
					var newUnit Unit
					newUnit.Location = v.Location
					newUnit.Owner = v.Owner
					newUnit.Type = barracksUnitType(v)
					if newUnit.Type == knight.index {
						newUnit.Location.radius = knightRadius
						newUnit.Speed = knightSpeed
						newUnit.Mass = knightMass
						newUnit.Health = knightHealth
					} else if newUnit.Type == archer.index {
						newUnit.Location.radius = archerRadius
						newUnit.Speed = archerSpeed
						newUnit.Range = archerRange
						newUnit.Mass = archerMass
						newUnit.Health = archerHealth
					} else if newUnit.Type == giant.index {
						newUnit.Location.radius = giantRadius
						newUnit.Speed = giantSpeed
						newUnit.Mass = giantMass
						newUnit.Health = giantHealth
					}

					for i := 0; i < v.Production; i++ {
						stepunits[unitGroupIdentifier(newUnit)] = append(stepunits[unitGroupIdentifier(newUnit)], newUnit)
						//						fmt.Fprintln(os.Stderr, "produce", unitString(newUnit))
					}
				}
				v.Param1 = newTurnsLeft
				stepsites[i] = v
			}
		} else if v.Type == tower.index {
			unit, index, distance, err := stepTowerTarget(v)
			damage := 0
			if err == nil {
				damage = 1 + (towerAttackRadius(v)-distance)/200
				if unit.Type != queen.index {
					damage = damage + 2
				}
				unit.Health = unit.Health - damage
				stepunits[unitGroupIdentifier(unit)][index] = unit
				if unit.Type == queen.index {
					//					fmt.Fprintln(os.Stderr, "tower->queen", unitGroupIdentifier(unit), index, stepunits[unitGroupIdentifier(unit)][index])
				}
			}
			v.Param1 -= 4
			v.Param2 = int(math.Sqrt(float64(towerHP(v)*towerRangeCoefficient+v.Location.radius) / math.Pi))
			stepsites[i] = v
		}
	}
}

func stepCreepMove() {
	for _, v := range stepunits {
		for i, u := range v {
			targetTeam := friendly
			if u.Owner == friendly.index {
				targetTeam = enemy
			}

			if u.Type == knight.index {
				u.Location = moveToward(u, i, stepunits[targetTeam.description+queen.description][0].Location, u.Location.radius)
			} else if u.Type == archer.index {
				targetUnit, targetUnitIndex := closestOpposingUnit(u)
				if distanceSpecifyRadius(u.Location, u.Range, targetUnit.Location, targetUnit.Location.radius) <= 0 {
					u.Location = moveToward(u, i, stepunits[unitGroupIdentifier(targetUnit)][targetUnitIndex].Location, u.Range)
				}
			} else if u.Type == giant.index {
				var descriptors []OwnerTypeDescriptors
				descriptors = append(descriptors, OwnerTypeDescriptors{targetTeam, tower})
				targetSite, err := getSiteByDistanceDescriptorIndex(u, descriptors, 0, false)
				if err != nil {
					u.Location = moveToward(u, i, stepsites[targetSite.Id].Location, u.Location.radius)
				}
			}
		}
	}
}

func stepCreepAge() {
	for k, v := range stepunits {
		for i, u := range v {
			// Aging
			if u.Type != queen.index {
				u.Health--
				stepunits[k][i] = u
			}
		}
	}
}

func stepCreepAttack() {
	for _, v := range stepunits {
		for _, u := range v {
			damage := 1
			targetTeam := friendly
			if u.Owner == friendly.index {
				targetTeam = enemy
			}

			if u.Type == knight.index {
				if distance(u.Location, stepunits[targetTeam.description+queen.description][0].Location) < touchingDelta {
					stepunits[targetTeam.description+queen.description][0].Health = stepunits[targetTeam.description+queen.description][0].Health - damage
				}
			} else if u.Type == archer.index {
				targetUnit, targetUnitIndex := closestOpposingUnit(u)
				damage = 2
				if targetUnit.Type == giant.index {
					damage = 10
				}
				if distanceSpecifyRadius(u.Location, u.Range, targetUnit.Location, targetUnit.Location.radius) <= 0 {
					stepunits[unitGroupIdentifier(targetUnit)][targetUnitIndex].Health = stepunits[unitGroupIdentifier(targetUnit)][targetUnitIndex].Health - damage
				}
			} else if u.Type == giant.index {
				damage = 80
				var descriptors []OwnerTypeDescriptors
				descriptors = append(descriptors, OwnerTypeDescriptors{targetTeam, tower})
				targetSite, err := getSiteByDistanceDescriptorIndex(u, descriptors, 0, false)
				if err != nil {
					if distance(u.Location, stepsites[targetSite.Id].Location) < touchingDelta {
						targetSite.Param1 = towerHP(targetSite) - damage
						stepsites[targetSite.Id] = targetSite
					}

					if towerHP(targetSite) <= 0 {
						var site Site
						site.Id = targetSite.Id
						site.Type = noStructure.index
						site.Location.x = targetSite.Location.x
						site.Location.y = targetSite.Location.y
						site.Location.radius = targetSite.Location.radius
						stepsites[targetSite.Id] = site
					}
				}
			}
		}
	}
}

func stepUnitsCollapseBuildings() {
	for k, v := range stepsites {
		if v.Type == goldmine.index {
			for _, uni := range stepunits {
				for _, u := range uni {
					if v.Owner != u.Owner && distance(v.Location, u.Location) < touchingDelta {
						var site Site
						site.Id = v.Id
						site.Type = noStructure.index
						site.Location.x = v.Location.x
						site.Location.y = v.Location.y
						site.Location.radius = v.Location.radius
						stepsites[k] = site
					}
				}
			}
		} else if v.Type == barracks.index {
			queenString := friendly.description
			if v.Owner == friendly.index {
				queenString = enemy.description
			}
			queenString = queenString + queen.description
			if distance(v.Location, stepunits[queenString][0].Location) < touchingDelta {
				var site Site
				site.Id = v.Id
				site.Type = noStructure.index
				site.Location.x = v.Location.x
				site.Location.y = v.Location.y
				site.Location.radius = v.Location.radius
				stepsites[k] = site
			}
		}
	}
}

func stepRemoveDead() {
	for _, v := range stepunits {
		deleted := 0
		for i := range v {
			j := i - deleted
			if v[j].Type != queen.index && v[j].Health <= 0 {
				v = v[:j+copy(v[j:], v[j+1:])]
				deleted++
			}
		}
	}
}

func stepCheckGameEnd() int {
	if stepunits[friendly.description+queen.description][0].Health > 0 && stepunits[enemy.description+queen.description][0].Health <= 0 {
		return friendly.index
	} else if stepunits[friendly.description+queen.description][0].Health <= 0 && stepunits[enemy.description+queen.description][0].Health > 0 {
		return enemy.index
	}

	return noOwner.index
}

func initializeSimulation() {
	stepunits = make(map[string][]Unit)
	stepsites = make(map[int]Site)
	for k, v := range units {
		for _, u := range v {
			stepunits[k] = append(stepunits[k], u)
		}
	}
	for k, v := range sites {
		stepsites[k] = v
	}
	stepgold = gold
	simulationBuilt = false
	simulationTrained = false
}

func fixCollisions() {
	for i := 0; i < 10; i++ {
		if !collisionCheck() {
			return
		}
	}
}

func collisionCheck() bool {
	for t, u := range stepunits {
		for i := range u {
			if collision(t, i) {
				return true
			}
		}
	}

	return false
}

func collision(unitName string, index int) bool {
	u1 := stepunits[unitName][index]
	if u1.Location.x < 0 {
		u1.Location.x = 0
		stepunits[unitName][index] = u1
		//				fmt.Fprintln(os.Stderr, "bonk A", u1.Location)
		return true
	}
	if u1.Location.x > worldMaxX {
		u1.Location.x = worldMaxX
		stepunits[unitName][index] = u1
		//				fmt.Fprintln(os.Stderr, "bonk B", u1.Location)
		return true
	}
	if u1.Location.y < 0 {
		u1.Location.y = 0
		stepunits[unitName][index] = u1
		//				fmt.Fprintln(os.Stderr, "bonk C", u1.Location)
		return true
	}
	if u1.Location.y > worldMaxY {
		u1.Location.y = worldMaxY
		stepunits[unitName][index] = u1
		//				fmt.Fprintln(os.Stderr, "bonk D", u1.Location)
		return true
	}

	for t2, v := range stepunits {
		for j, u2 := range v {
			if unitName == t2 && index == j {
				// Skip if colliding with self
				continue
			}
			overlapCheck := overlap(u1.Location, u2.Location)
			if overlapCheck > 0.000001 {
				d1 := float64(u2.Mass / (u2.Mass + u1.Mass))
				d2 := float64(u1.Mass / (u2.Mass + u1.Mass))
				gap := 1.0
				len := distanceSpecifyRadiusFloat(u1.Location, 0, u2.Location, 0)
				normalizedX := 1.0
				normalizedY := 0.0
				if len >= 0.000001 {
					normalizedX = float64(u2.Location.x-u1.Location.x) / len
					normalizedY = float64(u2.Location.y-u1.Location.y) / len
				}
				u1.Location.x -= int(normalizedX * (d1*overlapCheck + gap))
				u1.Location.y -= int(normalizedY * (d1*overlapCheck + gap))
				u2.Location.x += int(normalizedX * (d2*overlapCheck + gap))
				u2.Location.y += int(normalizedY * (d2*overlapCheck + gap))
				stepunits[unitName][index] = u1
				stepunits[t2][j] = u2
				//						fmt.Fprintln(os.Stderr, "bonk E", u1.Location, u2.Location)
				return true
			}
		}
	}

	for _, s := range stepsites {
		overlapCheck := overlap(u1.Location, s.Location)
		if overlapCheck <= 0.000001 {
			return false
		} else {
			d1 := 1.0
			gap := 1.0
			len := distanceSpecifyRadiusFloat(u1.Location, 0, s.Location, 0)
			normalizedX := 1.0
			normalizedY := 0.0
			if len >= 0.000001 {
				normalizedX = float64(s.Location.x-u1.Location.x) / len
				normalizedY = float64(s.Location.y-u1.Location.y) / len
			}
			u1.Location.x -= int(normalizedX * (d1*overlapCheck + gap))
			u1.Location.y -= int(normalizedY * (d1*overlapCheck + gap))
			stepunits[unitName][index] = u1
			//					fmt.Fprintln(os.Stderr, "bonk F", u1.Location, s.Location)
			return true
		}
	}

	return false
}

func coveredByTowers(unit Unit) int {
	coveredBy := 0
	for _, v := range stepsites {
		if v.Type == tower.index {
			if distanceSpecifyRadius(unit.Location, 0, v.Location, towerAttackRadius(v)) < 0 {
				if v.Owner == unit.Owner {
					coveredBy++
				}
			}
		}
	}
	return coveredBy
}

func simulationScore() int {
	score := 0
	simulationMyQueen := stepunits[friendly.description+queen.description][0]
	simulationEnemyQueen := stepunits[enemy.description+queen.description][0]
	scordy := "&"

	if stepCheckGameEnd() == friendly.index {
		score += 1000000
		scordy += "A"
	} else if stepCheckGameEnd() == enemy.index {
		score -= 1000000
		scordy += "B"
	} else {
		score = score + simulationMyQueen.Health*100
		score = score + simulationEnemyQueen.Health*-100
	}

	// Evaluate builds on existing buildings
	if simulationBuild.Id >= 0 {
		// Don't build goldmines if no gold is left
		if simulationBuild.Type == goldmine.index && sites[simulationBuild.Id].Gold == 0 {
			score -= 100000
			scordy += "C"
		}

		// Don't replace your own barracks
		if sites[simulationBuild.Id].Owner == friendly.index {
			if sites[simulationBuild.Id].Type == barracks.index {
				score -= 100
				scordy += "D"
			} else if sites[simulationBuild.Id].Type != simulationBuild.Type {
				score -= 100
				scordy += "E"
			}
		}

		// Building on empty sites is good
		if sites[simulationBuild.Id].Owner == noOwner.index {
			score += 100
			scordy += "F"
		}
	} else {
		// Not building at all is bad
		score -= 1000
		scordy += "G"
	}

	myMines := 0
	myTowers := 0
	myKnightBarracks := 0
	myArcherBarracks := 0
	myGiantBarracks := 0
	enemyKnightBarracks := 0
	enemyTowers := 0
	netTowerHealth := 0
	myKnights := len(stepunits[friendly.description+knight.description])
	enemyKnights := len(stepunits[enemy.description+knight.description])
	myBuildingKnights := 0
	for _, s := range sites {
		if s.Owner == friendly.index && s.Type == barracks.index && barracksTurnsRemaining(s) > 0 {
			myBuildingKnights++
		}
	}

	for _, s := range stepsites {
		if s.Owner == friendly.index {
			if s.Type == goldmine.index {
				myMines++
			} else if s.Type == tower.index {
				myTowers++
				netTowerHealth += towerHP(s)
			} else if s.Type == barracks.index {
				switch creep := barracksUnitType(s); creep {
				case knight.index:
					myKnightBarracks++
				case archer.index:
					myArcherBarracks++
				case giant.index:
					myGiantBarracks++
				}
			}
		} else if s.Owner == enemy.index {
			switch buildingType := s.Type; buildingType {
			case tower.index:
				enemyTowers++
				netTowerHealth -= towerHP(s)
			case barracks.index:
				if barracksUnitType(s) == knight.index {
					enemyKnightBarracks++
				}
			}
		}
	}

	if simulationTrain >= 0 {
		if netTowerHealth < towerInitialHP && barracksUnitType(stepsites[simulationTrain]) == giant.index {
			score += 10
			scordy += "a"
		}
		if coveredByTowers(simulationEnemyQueen) >= 3 && barracksUnitType(stepsites[simulationTrain]) == giant.index {
			score += 10
			scordy += "aa"
		}
		if myKnights+knightProduction < enemyKnights && barracksUnitType(stepsites[simulationTrain]) == archer.index {
			score += 10
			scordy += "b"
		}
		if simulationMyQueen.Health < simulationEnemyQueen.Health && barracksUnitType(stepsites[simulationTrain]) == knight.index {
			score += 10
			scordy += "c"
		}
		if myBuildingKnights == 0 && gold < giantCost {
			score -= 1000
			scordy += "d"
		}
	} else {
		//		score -= 1
		//		scordy += "e"
	}

	if myMines < 3 {
		score -= 100000 * (3 - myMines)
		scordy += "H" + strconv.Itoa(myMines)
	}
	if myKnightBarracks == 0 {
		score -= 9990
		scordy += "I"
	} else if myKnightBarracks < 2 {
		score -= 1000
		scordy += "J"
	} else if myKnightBarracks > 3 {
		score -= 1000
		scordy += "j"
	}
	if myTowers == 0 {
		score -= 500
		scordy += "K"
	}
	if myArcherBarracks == 0 {
		score -= 10
		scordy += "L"
	} else if myArcherBarracks > 1 {
		score -= 10000
		scordy += "l"
	}
	if myGiantBarracks == 0 {
		score -= 10
		scordy += "M"
	} else if myGiantBarracks > 1 {
		score -= 10000
		scordy += "m"
	}
	if simulationBuild.Type == goldmine.index && sites[simulationBuild.Id].Type == goldmine.index && goldmineIncome(sites[simulationBuild.Id]) < sites[simulationBuild.Id].MaxMineSize {
		score += 1000
		scordy += "N"
	}

	if maxScore < score {
		scordy = "^^" + scordy
	}
	//	fmt.Fprintln(os.Stderr, "score", score, scordy, siteString(simulationBuild), simulationMove, simulationTrain)

	return score
}

var maxScore int
var bestBuild Site
var bestMove Coordinate
var bestTrain int

func simulateIterate() {
	initializeSimulation()
	// Don't simulate 50 or more because tower hp=200-4*iteration
	for i := 0; i < simulateTurns; i++ {
		stepQueen()
		stepCreepMove()
		stepCreepAttack()
		stepUnitsCollapseBuildings()
		stepSites()
		stepCreepAge()
		stepRemoveDead()
		if stepCheckGameEnd() != noOwner.index {
			break
		}
	}
	thisScore := simulationScore()
	if maxScore < thisScore {
		maxScore = thisScore
		bestBuild = simulationBuild
		bestMove = simulationMove
		bestTrain = simulationTrain
	}
}

func simulate() {
	maxScore = math.MinInt32
	simulationBuild.Id = -1
	simulationMove.x = -1
	simulationTrain = -1

	bestBuild = simulationBuild
	bestMove = simulationMove
	bestTrain = simulationTrain
	initializeSimulation()
	simulationMove.x = -1
	simulationTrain = -1

	considerSites := make([]Site, 0, len(stepsites))
	if touchedSite >= 0 && stepsites[touchedSite].Owner == noOwner.index {
		considerSites = append(considerSites, stepsites[touchedSite])
	} else {
		var consider Site
		var err error
		var descriptors []OwnerTypeDescriptors
		descriptors = append(descriptors, OwnerTypeDescriptors{friendly, goldmine})
		sortSitesByDistance(myQueen, true)
		for i := 0; i < 3; i++ {
			consider, err = getSiteByDistanceDescriptorIndex(myQueen, descriptors, i, false)
			if err == nil && goldmineIncome(consider) < consider.MaxMineSize {
				considerSites = append(considerSites, consider)
				break
			}
		}
		descriptors = descriptors[:0]
		descriptors = append(descriptors, OwnerTypeDescriptors{enemy, barracks})
		descriptors = append(descriptors, OwnerTypeDescriptors{enemy, goldmine})
		descriptors = append(descriptors, OwnerTypeDescriptors{noOwner, noStructure})
		for i := 0; i < 3; i++ {
			consider, err = getSiteByDistanceDescriptorIndex(myQueen, descriptors, i, false)
			if err == nil {
				considerSites = append(considerSites, consider)
			}
		}
		descriptors = descriptors[:0]
		descriptors = append(descriptors, OwnerTypeDescriptors{friendly, tower})
		consider, err = getSiteByDistanceDescriptorIndex(myQueen, descriptors, 0, false)
		if err == nil {
			considerSites = append(considerSites, consider)
		}
	}

	for _, v := range considerSites {
		if v.Owner == enemy.index && v.Type == tower.index {
			continue
		} else if v.Owner == friendly.index && v.Type == barracks.index {
			continue
		} else if v.Owner == friendly.index && v.Type == goldmine.index && goldmineIncome(v) >= v.MaxMineSize {
			continue
		}

		if v.Owner != friendly.index || v.Type == tower.index {
			simulationBuild = v
			simulationBuild.Type = tower.index
			simulateIterate()
		}

		if v.Owner != friendly.index {
			simulationBuild = v
			simulationBuild.Type = barracks.index
			simulationBuild.Param2 = knight.index
			simulateIterate()

			simulationBuild = v
			simulationBuild.Type = barracks.index
			simulationBuild.Param2 = archer.index
			simulateIterate()

			simulationBuild = v
			simulationBuild.Type = barracks.index
			simulationBuild.Param2 = giant.index
			simulateIterate()
		}

		simulationTrain = -1
		if v.Owner != friendly.index || v.Type == goldmine.index {
			simulationBuild = v
			simulationBuild.Type = goldmine.index
			simulateIterate()
		}
	}

	// Try moving after doing the build
	simulationBuild = bestBuild
	simulationMove.x = myQueen.Location.x
	simulationMove.y = 0
	simulateIterate()
	simulationMove.x = myQueen.Location.x
	simulationMove.y = worldMaxY
	simulateIterate()
	simulationMove.x = 0
	simulationMove.y = myQueen.Location.y
	simulateIterate()
	simulationMove.x = worldMaxX
	simulationMove.y = myQueen.Location.y
	simulateIterate()

	// Try moving without building
	simulationBuild.Id = -1
	simulationMove.x = myQueen.Location.x
	simulationMove.y = 0
	simulateIterate()
	simulationMove.x = myQueen.Location.x
	simulationMove.y = worldMaxY
	simulateIterate()
	simulationMove.x = 0
	simulationMove.y = myQueen.Location.y
	simulateIterate()
	simulationMove.x = worldMaxX
	simulationMove.y = myQueen.Location.y
	simulateIterate()

	// Set training sites to what they would be after doing best action
	simulationBuild = bestBuild
	simulationMove = bestMove
	simulateIterate()
	var trainingSites []int
	for _, s := range sites {
		// Do not only consider build time at 0.  It will be 0 during simulation and it may be good to wait.
		if s.Owner == friendly.index && s.Type == barracks.index && barracksTurnsRemaining(s) == 0 {
			trainingSites = append(trainingSites, s.Id)
		}
	}

	// Try building at each single barracks
	for _, t := range trainingSites {
		simulationTrain = t
		simulateIterate()
	}

	simulationBuild = bestBuild
	simulationMove = bestMove
	simulationTrain = bestTrain
}

func main() {
	initialize()
	var numSites int
	fmt.Scan(&numSites)

	for i := 0; i < numSites; i++ {
		var site Site
		fmt.Scan(&site.Id, &site.Location.x, &site.Location.y, &site.Location.radius)
		sites[site.Id] = site
	}
	for {
		clearUnits()
		// touchedSite: -1 if none
		fmt.Scan(&gold, &touchedSite)

		for i := 0; i < numSites; i++ {
			var site Site
			fmt.Scan(&site.Id, &site.Gold, &site.MaxMineSize, &site.Type, &site.Owner, &site.Param1, &site.Param2)
			site.Location = sites[site.Id].Location
			site.Cost = 0
			if barracksUnitType(site) == knight.index {
				site.Cost = knightCost
				site.Production = knightProduction
				site.BuildTime = knightBuildTime
			}
			if barracksUnitType(site) == archer.index {
				site.Cost = archerCost
				site.Production = archerProduction
				site.BuildTime = archerBuildTime
			}
			if barracksUnitType(site) == giant.index {
				site.Cost = giantCost
				site.Production = giantProduction
				site.BuildTime = giantBuildTime
			}
			sites[site.Id] = site
		}
		var numUnits int
		fmt.Scan(&numUnits)

		for i := 0; i < numUnits; i++ {
			var unit Unit
			fmt.Scan(&unit.Location.x, &unit.Location.y, &unit.Owner, &unit.Type, &unit.Health)
			if unit.Type == queen.index {
				unit.Location.radius = queenRadius
				unit.Speed = queenSpeed
				unit.Mass = queenMass
			} else if unit.Type == knight.index {
				unit.Location.radius = knightRadius
				unit.Speed = knightSpeed
				unit.Mass = knightMass
			} else if unit.Type == archer.index {
				unit.Location.radius = archerRadius
				unit.Speed = archerSpeed
				unit.Range = archerRange
				unit.Mass = archerMass
			} else if unit.Type == giant.index {
				unit.Location.radius = giantRadius
				unit.Speed = giantSpeed
				unit.Mass = giantMass
			}

			units[unitGroupIdentifier(unit)] = append(units[unitGroupIdentifier(unit)], unit)
		}
		myQueen = units[friendly.description+queen.description][0]

		simulate()
		if simulationBuild.Id >= 0 {
			buildString := siteType[simulationBuild.Type]
			if simulationBuild.Type == barracks.index {
				buildString += "-" + unitType[barracksUnitType(simulationBuild)]
			}
			fmt.Println("BUILD " + strconv.Itoa(simulationBuild.Id) + " " + buildString)
		} else if simulationMove.x >= 0 {
			fmt.Println("MOVE " + strconv.Itoa(simulationMove.x) + " " + strconv.Itoa(simulationMove.y))
		} else {
			fmt.Println("WAIT")
		}
		trainString := ""
		if simulationTrain >= 0 {
			if sites[simulationTrain].Owner == friendly.index && sites[simulationTrain].Type == barracks.index {
				if sites[simulationTrain].Cost <= gold {
					trainString = trainString + " " + strconv.Itoa(simulationTrain)
					gold -= sites[simulationTrain].Cost
				}
			}
		}
		fmt.Println("TRAIN" + trainString)
	}
}
