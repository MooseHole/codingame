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
	Id       int
	Location Coordinate
	Ignore1  int
	Ignore2  int
	Type     int
	Owner    int
	Param1   int
	Param2   int
	Cost     int
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
	return owner[site.Owner] + " " + siteType[site.Type] + coordinateString(site.Location) + unitType[site.Param2] + " T:" + strconv.Itoa(site.Param1)
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
	nextSite.Type = barracks.index
	nextSite.Param2 = archer.index
	buildOrder = append(buildOrder, nextSite)
	nextSite.Type = tower.index
	buildOrder = append(buildOrder, nextSite)
}

func initialize() {
	initializeDescriptors()
	initializeMaps()
	initializeStrings()
	initializeBuildOrder()
	gold = 0
	touchedSite = -1
}

func distance(start, end Coordinate) int {
	return int(math.Sqrt(math.Pow(float64(start.x-end.x), 2)+math.Pow(float64(start.y-end.y), 2))) - start.radius - end.radius
}

func sameLocation(start, end Coordinate) bool {
	return start.x == end.x && start.y == end.y
}

func closestNonfriendlySite(toUnit Unit) Site {
	unitLocation := toUnit.Location
	leastDistance := 100000
	var site Site
	for _, v := range sites {
		if v.Owner != friendly.index {
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
			// ignore1: used in future leagues
			// ignore2: used in future leagues
			// structureType: -1 = No structure, 2 = Barracks
			// owner: -1 = No structure, 0 = Friendly, 1 = Enemy
			var site Site
			fmt.Scan(&site.Id, &site.Ignore1, &site.Ignore2, &site.Type, &site.Owner, &site.Param1, &site.Param2)
			site.Location = sites[site.Id].Location
			site.Cost = 0
			if unitType[site.Param2] == "KNIGHT" {
				site.Cost = 80
			}
			if unitType[site.Param2] == "ARCHER" {
				site.Cost = 100
			}
			if unitType[site.Param2] == "GIANT" {
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
		closeSite := closestSite(myQueen)
		touchingClosestSite := distance(myQueen.Location, closeSite.Location) < 1
		if !touchingClosestSite {
			if threat.Owner != friendly.index && distance(myQueen.Location, threat.Location) < distance(myQueen.Location, closestNonfriendlySite(myQueen).Location) {
				var retreatTo Site
				var err error
				retreatTo, err = closestSiteOwnerType(myQueen, friendly, tower)
				if err != nil {
					retreat = true
					queenAction = "MOVE " + strconv.Itoa(retreatTo.Location.x) + " " + strconv.Itoa(retreatTo.Location.y)
				} else {
					retreatTo, err = closestSiteOwner(myQueen, friendly)
					if err != nil {
						retreat = true
						queenAction = "MOVE " + strconv.Itoa(retreatTo.Location.x) + " " + strconv.Itoa(retreatTo.Location.y)
					} else {
						retreatTo, err = closestSiteOwner(myQueen, noOwner)
						if err != nil {
							retreat = true
							queenAction = "MOVE " + strconv.Itoa(retreatTo.Location.x) + " " + strconv.Itoa(retreatTo.Location.y)
						}
					}
				}
			}
		}
		if !retreat {
			buildStructure := buildOrder[buildOrderIndex]
			buildString := siteType[buildStructure.Type]
			if buildStructure.Type == barracks.index {
				buildString += "-" + unitType[buildStructure.Param2]
			}
			buildSite := closestNonfriendlySite(myQueen)
			touchingBuildSite := distance(myQueen.Location, buildSite.Location) < 1

			queenAction = "BUILD " + strconv.Itoa(buildSite.Id) + " " + buildString
			if touchingBuildSite {
				buildOrderIndex++
				if buildOrderIndex >= len(buildOrder) {
					buildOrderIndex = 0
				}
			}
		}

		// First line: A valid queen action
		// Second line: A set of training instructions
		fmt.Println(queenAction)
		fmt.Println("TRAIN" + trainString)
	}
}
