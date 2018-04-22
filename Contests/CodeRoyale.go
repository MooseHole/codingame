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
var buildOrder []Site
var buildOrderIndex int
var gold int
var touchedSite int
var previousSite int

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

func initializeBuildOrder() {
	buildOrderIndex = 0
	buildOrder = make([]Site, 0, 10)
	var nextSite Site
	nextSite.Type = goldmine.index
	buildOrder = append(buildOrder, nextSite)
	nextSite.Type = barracks.index
	nextSite.Param2 = knight.index
	buildOrder = append(buildOrder, nextSite)
	nextSite.Type = tower.index
	buildOrder = append(buildOrder, nextSite)
	nextSite.Type = barracks.index
	nextSite.Param2 = archer.index
	buildOrder = append(buildOrder, nextSite)
}

func getSpecialBarracksParam2() Descriptor {
	enemyTowers := 0
	enemyKnightBarracks := 0
	for _, s := range sites {
		if s.Owner == enemy.index {
			if s.Type == tower.index {
				enemyTowers++
			} else if s.Type == barracks.index && s.Param2 == knight.index {
				enemyKnightBarracks++
			}
		}
	}
	if enemyTowers > enemyKnightBarracks {
		return giant
	}

	return archer
}

func initialize() {
	initializeDescriptors()
	initializeMaps()
	initializeStrings()
	initializeBuildOrder()
	gold = 0
	touchedSite = -1
	previousSite = -1
}

func distance(start, end Coordinate) int {
	return int(math.Sqrt(math.Pow(float64(start.x-end.x), 2)+math.Pow(float64(start.y-end.y), 2))) - start.radius - end.radius
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

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/

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
			}
			if site.Param2 == archer.index {
				site.Cost = 100
			}
			if site.Param2 == giant.index {
				site.Cost = 140
			}
			sites[site.Id] = site
		}
		var numUnits int
		fmt.Scan(&numUnits)

		for i := 0; i < numUnits; i++ {
			var unit Unit
			fmt.Scan(&unit.Location.x, &unit.Location.y, &unit.Owner, &unit.Type, &unit.Health)
			unit.Location.radius = 30
			units[unitGroupIdentifier(unit)] = append(units[unitGroupIdentifier(unit)], unit)
		}

		// printEntities()

		trainString := ""
		for _, v := range sites {
			if v.Owner == friendly.index {
				if v.Cost <= gold && v.Param1 == 0 {
					trainString += " " + strconv.Itoa(v.Id)
					gold -= v.Cost
				}
			}
		}

		myQueen := units[friendly.description+queen.description][0]

		threat := closestUnit(myQueen)
		queenAction := ""
		retreat := false
		//		closeSite := closestSite(myQueen)
		//		touchingClosestSite := distance(myQueen.Location, closeSite.Location) < 1
		//		if !touchingClosestSite {
		//		if threat.Owner != friendly.index && distance(myQueen.Location, threat.Location) < distance(myQueen.Location, closestNonfriendlySite(myQueen).Location) {
		if threat.Owner != friendly.index && distance(myQueen.Location, threat.Location) < 60 {
			fmt.Fprintln(os.Stderr, "Retreat")
			var retreatTo Site
			var err error
			retreatTo, err = closestSiteOwnerType(myQueen, friendly, tower)
			if err == nil {
				retreat = true
				queenAction = "MOVE " + strconv.Itoa(retreatTo.Location.x) + " " + strconv.Itoa(retreatTo.Location.y)
			} else {
				fmt.Fprintln(os.Stderr, "Didn't retreat A because %v", err)
				retreatTo, err = closestSiteOwner(myQueen, friendly)
				if err == nil {
					retreat = true
					queenAction = "MOVE " + strconv.Itoa(retreatTo.Location.x) + " " + strconv.Itoa(retreatTo.Location.y)
				} else {
					fmt.Fprintln(os.Stderr, "Didn't retreat B because %v", err)
					retreatTo, err = closestSiteOwner(myQueen, noOwner)
					if err == nil {
						fmt.Fprintln(os.Stderr, "Didn't retreat C because %v", err)
						retreat = true
						queenAction = "MOVE " + strconv.Itoa(retreatTo.Location.x) + " " + strconv.Itoa(retreatTo.Location.y)
					}
				}
			}
		}
		//		}
		if !retreat {
			fmt.Fprintln(os.Stderr, "Advance")
			buildStructure := buildOrder[buildOrderIndex]
			buildString := siteType[buildStructure.Type]
			if buildStructure.Type == barracks.index {
				if buildStructure.Param2 != knight.index {
					buildStructure.Param2 = getSpecialBarracksParam2().index
				}
				buildString += "-" + unitType[buildStructure.Param2]
			}
			buildSite := closestNonfriendlyNontowerSite(myQueen)
			nextSiteOk := true
			if sites[previousSite].Type == goldmine.index {
				fmt.Fprintln(os.Stderr, "Previous was goldmine")
				closestMine, err := closestSiteOwnerType(myQueen, friendly, goldmine)
				fmt.Fprintln(os.Stderr, "closestMine %v err %v", siteString(closestMine), err)
				if err == nil && closestMine.MaxMineSize > closestMine.Param1 {
					fmt.Fprintln(os.Stderr, "Upgrading this mine %d %d", closestMine.MaxMineSize, closestMine.Param1)
					buildSite = closestMine
					nextSiteOk = false
				}
			}

			queenAction = "BUILD " + strconv.Itoa(buildSite.Id) + " " + buildString
			// If build is complete
			if nextSiteOk && previousSite >= 0 && buildSite.Id != previousSite {
				buildOrderIndex++
				if buildOrderIndex >= len(buildOrder) {
					buildOrderIndex = 0
				}
			}

			previousSite = buildSite.Id
		}

		// First line: A valid queen action
		// Second line: A set of training instructions
		fmt.Println(queenAction)
		fmt.Println("TRAIN" + trainString)
	}
}
