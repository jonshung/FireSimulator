#include <iostream>
#include "FireSimulation.h"

int main()
{
	std::vector<int> windDirections;
	int width, height, burningTime;
	Tree** treeList = readConfiguration(windDirections, width, height, burningTime);
	startSimulation(treeList, width, height, burningTime, windDirections);
}