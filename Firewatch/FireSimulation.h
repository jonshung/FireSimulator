#ifndef _FIREWATCH_HEADER_
#define _FIREWATCH_HEADER_

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <regex>
#include <iomanip>

enum TreeState {
	SAFE,
	FIRE,
	DEAD,
	BURNING_DEAD,
	BURNED,
	LAND
};

enum Direction {
	NORTH,
	SOUTH,
	EAST,
	WEST,
	CALM
};

enum SpreadDirection {
	SPREAD_NORTH = 0x01,
	SPREAD_SOUTH = 0x02,
	SPREAD_EAST = 0x04,
	SPREAD_WEST = 0x08
};

struct Tree {
	int _state;
	int _burnedTime;
};

struct SimulationData {
	Tree** _treeList;
	std::vector<int> _windDirections;
	int _width;
	int _height;
	int _hour;
	int _burnTime;
	/**
	 * @brief Storing the maximum number of tree that got set on fire per hour
	*/
	int _maxBurningPerHour;
	int _maxBurningHour;
	/**
	 * @brief Storing the average number of tree that got set on fire till call time
	*/
	double _averageSpread;
};

Tree** readConfiguration(std::vector<int>&, int&, int&, int&);

int getTreeStateFromData(std::string);
int getWindDirectionFromData(std::string dat);
std::string treeStateToString(int);
std::string windDirectionToString(int bounding);

void printWindForcast(std::vector<int>);
void printForest(Tree**, const int&, const int&, int hour = 0);
void printCurrentWind(int wind);

SimulationData bundle(Tree**, const int&, const int&, std::vector<int>);
void startSimulation(Tree** treeList, const int& width, const int& height, const int& burningTime, std::vector<int> windDirections);
int spreadAt(SimulationData& pkg, const int& atX, const int& atY, int windDirection);
int spreadTo(SimulationData& pkg, const int& atX, const int& atY, int to);
#endif