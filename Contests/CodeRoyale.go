package main

import (
	"fmt"
	"math"
	"os"
	"strconv"
)

//import "os"

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
	return owner[unit.Owner] + " " + unitType[unit.Type] + coordinateString(unit.Location) + strconv.Itoa(unit.Health)
}

func siteString(site Site) string {
	returnString := owner[site.Owner] + " " + siteType[site.Type] + coordinateString(site.Location)
	if site.Type == barracks.index {
		returnString += unitType[site.Param2] + " Turns:" + strconv.Itoa(site.Param1)
	}
	if site.Type == tower.index {
		returnString += " HP:" + strconv.Itoa(site.Param1) + " Radius:" + strconv.Itoa(site.Param2)
	}
	if site.Type == goldmine.index {
		returnString += " Income:" + strconv.Itoa(site.Param1) + " Max:" + strconv.Itoa(site.MaxMineSize) + " Left:" + strconv.Itoa(site.Gold)
	}
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
	siteType[-1] = "No structure"
	siteType[goldmine.index] = goldmine.description
	siteType[tower.index] = tower.description
	siteType[barracks.index] = barracks.description
}

func getTraining() string {
	trainString := ""
	longestBuildTime := 0
	sitesToConsider := make([]Site, 0, len(sites))
	enemyTowers := 0
	for _, v := range sites {
		if v.Owner == enemy.index && v.Type == tower.index {
			enemyTowers++
		}
	}
	enemyKnights := len(units[enemy.description+knight.description])

	for _, v := range sites {
		// If my barracks and ready to build
		if v.Owner == friendly.index && v.Type == barracks.index && v.Param1 == 0 {
			if v.Param2 == giant.index && enemyTowers <= 3 {
				continue
			}
			if v.Param2 == archer.index && enemyKnights == 0 {
				continue
			}
			if v.BuildTime > longestBuildTime {
				longestBuildTime = v.BuildTime
				sitesToConsider = append(sitesToConsider, v)
			}
		}
	}
	for _, v := range sitesToConsider {
		// If can afford
		if v.BuildTime == longestBuildTime && v.Cost <= gold {
			trainString += " " + strconv.Itoa(v.Id)
			gold -= v.Cost
		}
	}

	return trainString
}

func getSiteToBuild() Site {
	closestMine, err := closestSiteOwnerType(myQueen, friendly, goldmine)
	if err == nil && closestMine.MaxMineSize > closestMine.Param1 {
		return closestMine
	}
	/*	closestTower, err := closestSiteOwnerType(myQueen, friendly, tower)
		if err == nil && closestTower.MaxMineSize > closestTower.Param1 {
			return closestTower
		}
	*/

	myMines := 0
	myTowers := 0
	myKnightBarracks := 0
	myArcherBarracks := 0
	myGiantBarracks := 0
	enemyKnightBarracks := 0
	enemyTowers := 0
	for _, s := range sites {
		if s.Owner == friendly.index {
			switch buildingType := s.Type; buildingType {
			case goldmine.index:
				myMines++
			case tower.index:
				myTowers++
			case barracks.index:
				switch creep := s.Param2; creep {
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
			case barracks.index:
				if s.Param2 == knight.index {
					enemyKnightBarracks++
				}
			}
		}
	}

	site := closestNonfriendlyNontowerSite(myQueen)
	if myMines == 0 {
		site.Type = goldmine.index
		return site
	} else if myKnightBarracks == 0 {
		site.Type = barracks.index
		site.Param2 = knight.index
		return site
	} else if myTowers == 0 {
		site.Type = tower.index
		return site
	} else if enemyKnightBarracks > 3 && myArcherBarracks == 0 {
		site.Type = barracks.index
		site.Param2 = archer.index
		return site
	} else if enemyTowers > 3 && myGiantBarracks == 0 {
		site.Type = barracks.index
		site.Param2 = giant.index
		return site
	} else if myMines <= myTowers {
		site.Type = goldmine.index
		return site
	} else if myTowers <= myKnightBarracks {
		site.Type = tower.index
		return site
	} else if myKnightBarracks <= 3 {
		site.Type = barracks.index
		site.Param2 = knight.index
		return site
	} else {
		site.Type = goldmine.index
		return site
	}
}

func initialize() {
	initializeDescriptors()
	initializeMaps()
	initializeStrings()
	gold = 0
	touchedSite = -1
}

func distance(start, end Coordinate) int {
	return int(math.Sqrt(math.Pow(float64(start.x-end.x), 2)+math.Pow(float64(start.y-end.y), 2))) - start.radius - end.radius
}

func distanceSpecifyRadius(start Coordinate, startRadius int, end Coordinate, endRadius int) int {
	return int(math.Sqrt(math.Pow(float64(start.x-end.x), 2)+math.Pow(float64(start.y-end.y), 2))) - startRadius - endRadius
}

func sameLocation(start, end Coordinate) bool {
	return start.x == end.x && start.y == end.y
}

func closestNonfriendlyNontowerSite(toUnit Unit) Site {
	unitLocation := toUnit.Location
	leastDistance := 100000
	var site Site
	for _, v := range sites {
		if v.Owner != friendly.index && v.Type != tower.index {
			thisDistance := distance(unitLocation, v.Location)
			if thisDistance < leastDistance {
				leastDistance = thisDistance
				site = v
			}
		}
	}

	return site
}

func closestFriendlyTower(toUnit Unit) (Site, error) {
	return closestSiteOwnerType(toUnit, friendly, tower)
}

func closestSiteOwnerType(toUnit Unit, owner Descriptor, structureType Descriptor) (Site, error) {
	unitLocation := toUnit.Location
	leastDistance := 100000
	siteFound := false
	var site Site
	for _, v := range sites {
		if v.Owner == owner.index && v.Type == structureType.index {
			thisDistance := distance(unitLocation, v.Location)
			if thisDistance < leastDistance {
				leastDistance = thisDistance
				site = v
				siteFound = true
			}
		}
	}

	if !siteFound {
		return site, fmt.Errorf("No %v %v site found", owner.description, structureType.description)
	}
	return site, nil
}

func closestSiteOwner(toUnit Unit, owner Descriptor) (Site, error) {
	unitLocation := toUnit.Location
	leastDistance := 100000
	siteFound := false
	var site Site
	for _, v := range sites {
		if v.Owner == owner.index {
			thisDistance := distance(unitLocation, v.Location)
			if thisDistance < leastDistance {
				leastDistance = thisDistance
				site = v
				siteFound = true
			}
		}
	}

	if !siteFound {
		return site, fmt.Errorf("No %v site found", owner.description)
	}
	return site, nil
}

// The closest site to the given unit
func closestSite(toUnit Unit) Site {
	unitLocation := toUnit.Location
	leastDistance := 100000
	var site Site
	for _, v := range sites {
		if !sameLocation(unitLocation, v.Location) {
			thisDistance := distance(unitLocation, v.Location)
			if thisDistance < leastDistance {
				leastDistance = thisDistance
				site = v
			}
		}
	}

	return site
}

// The closest unit to the given unit
func closestUnit(toUnit Unit) Unit {
	unitLocation := toUnit.Location
	leastDistance := 100000
	var unit Unit
	for _, v := range units {
		for _, u := range v {
			if !sameLocation(unitLocation, u.Location) {
				thisDistance := distance(unitLocation, u.Location)
				if thisDistance < leastDistance {
					leastDistance = thisDistance
					unit = u
				}
			}
		}
	}

	return unit
}

// towerTarget returns the found unit, unit index, distance, and error
func towerTarget(thisTower Site) (Unit, int, int, error) {
	var unit Unit
	if thisTower.Type != tower.index {
		return unit, 0, 0, fmt.Errorf("towerTarget: Site %d is not a tower", thisTower.Id)
	}

	leastDistance := 100000
	queenDistance := 100000
	var index int
	foundQueen := false
	foundOtherUnit := false
	for _, v := range units {
		for i, u := range v {
			if u.Owner != thisTower.Owner {
				thisDistance := distance(thisTower.Location, u.Location)
				if thisDistance < thisTower.Param2 {
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
var simulationTrain []int

func moveToward(mover Unit, destination Coordinate, moverRadius int) Coordinate {
	if distanceSpecifyRadius(mover.Location, moverRadius, destination, destination.radius) <= 0 {
		return mover.Location
	}

	var angleFinder Coordinate
	angleFinder.x = mover.Location.x - destination.x
	angleFinder.y = mover.Location.y - destination.y
	angle := math.Atan2(float64(angleFinder.y), float64(angleFinder.x))

	mover.Location.x = int(float64(mover.Speed) * math.Cos(angle))
	mover.Location.y = int(float64(mover.Speed) * math.Sin(angle))

	return mover.Location
}

func stepSites() {
	for i, v := range stepsites {
		if v.Type == goldmine.index {
			if v.Owner == friendly.index {
				dug := int(math.Min(float64(v.Gold), float64(v.Param1)))
				stepgold = stepgold + dug
				v.Gold = v.Gold - dug
				stepsites[i] = v
			}
		} else if v.Type == barracks.index {
			if v.Param1 > 0 {
				newTurnsLeft := v.Param1 - 1
				if newTurnsLeft == 0 {
					var newUnit Unit
					newUnit.Location = v.Location
					newUnit.Owner = v.Owner
					newUnit.Type = v.Param2
					if newUnit.Type == knight.index {
						newUnit.Location.radius = 20
						newUnit.Speed = 100
						newUnit.Mass = 400
						newUnit.Health = 30
					} else if newUnit.Type == archer.index {
						newUnit.Location.radius = 25
						newUnit.Speed = 75
						newUnit.Range = 200
						newUnit.Mass = 900
						newUnit.Health = 45
					} else if newUnit.Type == giant.index {
						newUnit.Location.radius = 40
						newUnit.Speed = 50
						newUnit.Mass = 2000
						newUnit.Health = 200
					}

					for i := 0; i < v.Production; i++ {
						stepunits[unitString(newUnit)] = append(stepunits[unitString(newUnit)], newUnit)
					}
				}
				v.Param1 = newTurnsLeft
				stepsites[i] = v
			} else if v.Type == tower.index {
				unit, index, distance, err := towerTarget(v)
				damage := 0
				if err == nil {
					damage = 1 + (v.Param2-distance)/200
					if unit.Type != queen.index {
						damage = damage + 2
					}
				}
				unit.Health = unit.Health - damage
				stepunits[unitString(unit)][index] = unit
			}
		}
	}
}

func stepCreeps() {
	for _, v := range stepunits {
		for i, u := range v {
			// Aging
			if u.Type != queen.index {
				u.Health--
			}

			damage := 1
			targetTeam := friendly
			targetType := queen.description
			if u.Owner == friendly.index {
				targetTeam = enemy
			}

			if u.Type == knight.index {
				u.Location = moveToward(u, stepunits[targetTeam.description+targetType][0].Location, u.Location.radius)
				if distance(u.Location, stepunits[targetTeam.description+targetType][0].Location) <= 0 {
					stepunits[targetTeam.description+targetType][0].Health = stepunits[targetTeam.description+targetType][0].Health - damage
				}
				stepunits[unitGroupIdentifier(u)][i] = u
			} else if u.Type == archer.index {
				targetUnit, targetUnitIndex := closestOpposingUnit(u)
				targetType := unitType[targetUnit.Type]
				damage = 2
				if targetUnit.Type == giant.index {
					damage = 10
				}

				u.Location = moveToward(u, stepunits[targetTeam.description+targetType][0].Location, u.Location.radius)
				if distanceSpecifyRadius(u.Location, u.Range, targetUnit.Location, targetUnit.Location.radius) <= 0 {
					stepunits[targetTeam.description+targetType][targetUnitIndex].Health = stepunits[targetTeam.description+targetType][targetUnitIndex].Health - damage
				}
				stepunits[unitGroupIdentifier(u)][i] = u
			} else if u.Type == giant.index {
				damage = 80
				targetSite, err := closestSiteOwnerType(u, targetTeam, tower)
				if err != nil {
					u.Location = moveToward(u, stepsites[targetSite.Id].Location, u.Location.radius)
					if distance(u.Location, stepsites[targetSite.Id].Location) <= 0 {
						targetSite.Param1 = targetSite.Param1 - damage
						stepsites[targetSite.Id] = targetSite
					}
					stepunits[unitGroupIdentifier(u)][i] = u
				}
			}
		}
	}
}

func stepRemoveDead() {
	for _, v := range stepunits {
		deleted := 0
		for i := range v {
			j := i - deleted
			if v[j].Health <= 0 {
				v = v[:j+copy(v[j:], v[j+1:])]
				deleted++
			}
		}
	}

	for k, v := range stepsites {
		if v.Type == tower.index && v.Param1 <= 0 {
			var site Site
			site.Id = v.Id
			site.Location.x = v.Location.x
			site.Location.y = v.Location.y
			site.Location.radius = v.Location.radius
			stepsites[k] = site
		} else if v.Type == goldmine.index {
			for _, uni := range stepunits {
				for _, u := range uni {
					if v.Owner != u.Owner && distance(v.Location, u.Location) <= 0 {
						var site Site
						site.Id = v.Id
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
			if distance(v.Location, stepunits[queenString][0].Location) <= 0 {
				var site Site
				site.Id = v.Id
				site.Location.x = v.Location.x
				site.Location.y = v.Location.y
				site.Location.radius = v.Location.radius
				stepsites[k] = site
			}
		}
	}
}

func initializeSimulation() {
	for k, v := range units {
		for _, u := range v {
			stepunits[k] = append(stepunits[k], u)
		}
	}
	for k, v := range sites {
		stepsites[k] = v
	}
	stepgold = gold
}

func simulate() {
	initializeSimulation()

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
			if site.Param2 == knight.index {
				site.Cost = 80
				site.Production = 4
				site.BuildTime = 5
			}
			if site.Param2 == archer.index {
				site.Cost = 100
				site.Production = 2
				site.BuildTime = 8
			}
			if site.Param2 == giant.index {
				site.Cost = 140
				site.Production = 1
				site.BuildTime = 10
			}
			sites[site.Id] = site
		}
		var numUnits int
		fmt.Scan(&numUnits)

		for i := 0; i < numUnits; i++ {
			var unit Unit
			fmt.Scan(&unit.Location.x, &unit.Location.y, &unit.Owner, &unit.Type, &unit.Health)
			if unit.Type == queen.index {
				unit.Location.radius = 30
				unit.Speed = 60
			} else if unit.Type == knight.index {
				unit.Location.radius = 20
				unit.Speed = 100
				unit.Mass = 400
				unit.Health = 30
			} else if unit.Type == archer.index {
				unit.Location.radius = 25
				unit.Speed = 75
				unit.Range = 200
				unit.Mass = 900
				unit.Health = 45
			} else if unit.Type == giant.index {
				unit.Location.radius = 40
				unit.Speed = 50
				unit.Mass = 2000
				unit.Health = 200
			}

			units[unitGroupIdentifier(unit)] = append(units[unitGroupIdentifier(unit)], unit)
		}

		myQueen = units[friendly.description+queen.description][0]

		threat := closestUnit(myQueen)
		queenAction := ""
		retreat := false
		if threat.Owner != friendly.index && distance(myQueen.Location, threat.Location) < 60 {
			fmt.Fprintln(os.Stderr, "Retreat")
			var retreatTo Site
			var err error
			retreatTo, err = closestSiteOwnerType(myQueen, friendly, tower)
			if err == nil {
				retreat = true
				queenAction = "BUILD " + strconv.Itoa(retreatTo.Id) + " " + tower.description
			} else {
				fmt.Fprintln(os.Stderr, "Didn't retreat A because %v", err)
				retreatTo, err = closestSiteOwner(myQueen, friendly)
				if err == nil {
					retreat = true
					queenAction = "MOVE " + strconv.Itoa(retreatTo.Location.x) + " " + strconv.Itoa(retreatTo.Location.y)
				}
			}
		}
		if !retreat {
			fmt.Fprintln(os.Stderr, "Advance")

			buildSite := getSiteToBuild()
			buildString := siteType[buildSite.Type]
			if buildSite.Type == barracks.index {
				buildString += "-" + unitType[buildSite.Param2]
			}
			queenAction = "BUILD " + strconv.Itoa(buildSite.Id) + " " + buildString
		}

		// First line: A valid queen action
		// Second line: A set of training instructions
		fmt.Println(queenAction)
		fmt.Println("TRAIN" + getTraining())
	}
}
