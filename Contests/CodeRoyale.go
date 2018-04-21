package main

import (
	"fmt"
	"math"
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
var gold int
var touchedSite int

func coordinateString(coordinate Coordinate) string {
	return "[" + strconv.Itoa(coordinate.x) + ", " + strconv.Itoa(coordinate.y) + "(" + strconv.Itoa(coordinate.radius) + ")" + "]"
}

func unitGroupIdentifier(unit Unit) string {
	return owner[unit.Owner] + " " + unitType[unit.Type]
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

func initializeStrings() {
	unitType[-1] = "QUEEN"
	unitType[0] = "KNIGHT"
	unitType[1] = "ARCHER"
	owner[-1] = "None"
	owner[0] = "Friendly"
	owner[1] = "Enemy"
	siteType[-1] = "No structure"
	siteType[2] = "Barracks"
}

func initialize() {
	initializeMaps()
	initializeStrings()
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
		if !sameLocation(unitLocation, v.Location) && owner[v.Owner] != "Friendly" {
			thisDistance := distance(unitLocation, v.Location)
			if thisDistance < leastDistance {
				leastDistance = thisDistance
				site = v
			}
		}
	}

	return site
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
		/*
			for _, v := range units {
				for _, u := range v {
					fmt.Fprintln(os.Stderr, unitString(u))
				}
			}

			for _, v := range sites {
				fmt.Fprintln(os.Stderr, siteString(v))
			}
		*/
		trainString := ""
		for _, v := range sites {
			if owner[v.Owner] == "Friendly" {
				if v.Cost <= gold && v.Param1 == 0 {
					trainString += " " + strconv.Itoa(v.Id)
					gold -= v.Cost
				}
			}
		}
		// fmt.Fprintln(os.Stderr, "Debug messages...")

		// First line: A valid queen action
		// Second line: A set of training instructions
		fmt.Println("BUILD " + strconv.Itoa(closestNonfriendlySite(units["Friendly QUEEN"][0]).Id) + " BARRACKS-KNIGHT")
		fmt.Println("TRAIN" + trainString)
	}
}
