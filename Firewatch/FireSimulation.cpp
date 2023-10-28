#include "FireSimulation.h"

/**
 * @brief Main method for retrieving data based on user input and initialize simulation data
 *
 * @param windDirections
 * @param width
 * @param height
 * @return
*/
Tree** readConfiguration(std::vector<int>& windDirections, int& width, int& height, int& burnTime) {
	std::fstream fileHandler;
	std::string buffer;
	Tree** treeList = nullptr;

	while (!fileHandler.is_open()) {
		std::cout << "Please enter the name of the configuration file: ";
		std::getline(std::cin, buffer);
		fileHandler.open(buffer);
		if (!fileHandler.is_open()) {
			std::cout << buffer << " does not exist! ";
			continue;
		}
		break;
	}

	std::regex integerPattern("\\d+");
	std::regex filePattern("([^ ]*).txt");
	std::smatch matches;
	int dataEntried = 0;
	burnTime = -1;

	while (fileHandler.peek() != EOF) {		// safe guard for corrupted file
		std::getline(fileHandler, buffer);
		switch (dataEntried) {
		case 0:
		case 1:
		case 2: {
			if (!std::regex_search(buffer, matches, integerPattern)) {
				throw std::exception("Incorrect or corrupted configuration data");
			}
			int data = std::stoi(matches[0]);
			if (dataEntried == 0) {
				height = data;
			}
			else if (dataEntried == 1) {
				width = data;
			}
			else {
				burnTime = data;
			}
			break;
		}
		case 3: {
			treeList = new Tree * [height];

			std::regex_search(buffer, matches, filePattern);
			if (matches.size() == 0) {
				throw std::exception("Incorrect or corrupted filepath for forest configuration");
			}
			std::fstream forestFileHandler(matches[0]);
			if (!forestFileHandler.is_open()) {
				std::cout << "Filename: " << matches[0];
				throw std::exception("Cannot open file forest configuration file");
			}
			for (int j = 0; j < height; j++) {
				treeList[j] = new Tree[width];
				for (int i = 0; i < width; i++) {
					char delim = ',';
					if (i == width - 1) delim = '\n';
					std::getline(forestFileHandler, buffer, delim);
					Tree newTree;
					newTree._burnedTime = 0;
					newTree._state = getTreeStateFromData(buffer);
					treeList[j][i] = newTree;
				}
			}
			forestFileHandler.close();
			break;
		}
		case 4: {
			std::regex_search(buffer, matches, filePattern);
			if (matches.size() == 0) {
				throw std::exception("Incorrect or corrupted filepath for forest configuration");
			}
			std::fstream windFileHandler(matches[0]);
			if (!windFileHandler.is_open()) {
				std::cout << "Filename: " << matches[0];
				throw std::exception("Cannot open file forest configuration file");
			}
			while (windFileHandler.peek() != EOF) {
				std::getline(windFileHandler, buffer);
				windDirections.push_back(getWindDirectionFromData(buffer));
			}
			windFileHandler.close();
			break;
		}
		default:
			break;
		}
		dataEntried++;
	}
	printForest(treeList, width, height);
	std::cout << std::endl;
	printWindForcast(windDirections);
	std::cout << std::endl;
	return treeList;
}

int getTreeStateFromData(std::string dat) {
	if (dat == "T") return TreeState::SAFE;
	if (dat == "F") return TreeState::FIRE;
	if (dat == "B") return TreeState::BURNED;
	return TreeState::LAND;
}

int getWindDirectionFromData(std::string dat) {
	if (dat == "N") return Direction::NORTH;
	if (dat == "S") return Direction::SOUTH;
	if (dat == "E") return Direction::EAST;
	if (dat == "W") return Direction::WEST;
	return Direction::CALM;
}

std::string treeStateToString(int state) {
	switch (state) {
	case TreeState::SAFE: return "T";
	case TreeState::FIRE: return "F";
	case TreeState::DEAD: return "t";
	case TreeState::BURNING_DEAD: return "f";
	case TreeState::BURNED: return "B";
	default: return " ";
	}
	return " ";
}

std::string windDirectionToString(int bounding) {
	switch (bounding) {
	case Direction::NORTH: return "N";
	case Direction::SOUTH: return "S";
	case Direction::EAST: return "E";
	case Direction::WEST: return "W";
	default: return "C";
	}
	return "C";
}

void printWindForcast(std::vector<int> windDirections) {
	std::cout << "The weather forecast is: " << std::endl;
	for (auto i : windDirections) {
		std::cout << windDirectionToString(i) << " ";
	}
	std::cout << std::endl;
}

void printCurrentWind(int wind) {
	std::cout << "The wind is blowing " << windDirectionToString(wind) << std::endl;
}

void printForest(Tree** treeList, const int& width, const int& height, int hour) {
	std::cout << "Hour " << hour << ":" << std::endl;
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			char delim = ',';
			std::cout << treeStateToString(treeList[i][j]._state);
			if (j == width - 1) delim = '\n';
			std::cout << delim;
		}
	}
}

SimulationData bundle(Tree** treeList, const int& width, const int& height, std::vector<int> windDirections, const int& burnTime) {
	SimulationData result;
	result._treeList = treeList;
	result._width = width;
	result._height = height;
	result._windDirections = windDirections;
	return result;
}

int spreadTo(SimulationData& pkg, const int& atX, const int& atY, int windDirection) {
	int startX = -1, startY = -1;
	int iterationX = 3, iterationY = 3;
	Tree currentTree = pkg._treeList[atY][atX];
	switch (windDirection) {
	case Direction::NORTH: {
		iterationY = 1;
		break;
	}
	case Direction::SOUTH: {
		startY = 1;
		iterationY = 1;
		break;
	}
	case Direction::EAST: {
		startX = 1;
		iterationX = 1;
		break;
	}
	case Direction::WEST: {
		iterationX = 1;
		break;
	}
	default: break;
	}

	int spreadCount = 0;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {

			int adjX = atX + j, adjY = atY + i;
			if (adjX < 0 || adjX >= pkg._width || adjY < 0 || adjY >= pkg._height) continue;
			Tree* treeRef = &pkg._treeList[adjY][adjX];
			int state = treeRef->_state;
			if (!(state == TreeState::SAFE || state == TreeState::DEAD)) {
				continue;
			}

			if ((i >= startY && i <= (startY + iterationY - 1)) && (j >= startX && j <= (startX + iterationX - 1))
				&& state != TreeState::DEAD) {
				treeRef->_state = TreeState::FIRE;
			}
			else if (state == TreeState::DEAD) {
				if (treeRef->_burnedTime <= -pkg._hour) continue;
				treeRef->_state = TreeState::BURNING_DEAD;
			}
			else continue;
			int initialBurningTime = ((adjX > atX && adjY == atY) || (adjY > atY)) ? -1 : 0;
			treeRef->_burnedTime = initialBurningTime;
			spreadCount++;
		}
	}
	return spreadCount;
}

int spreadAt(SimulationData& pkg, const int& atX, const int& atY, int windDirection) {
	int spreadCount = 0;
	if (windDirection == Direction::CALM) {
		int counter = 1;
		for (int k = 0; k < 4; k++) {
			spreadCount += spreadTo(pkg, atX, atY, k);
		}
	}
	else {
		spreadCount += spreadTo(pkg, atX, atY, windDirection);
	}
	return spreadCount;
}

void tick(SimulationData& pkg, int& totalSpreadCount, int& burningTree, const int& hour) {
	int currentDirection = pkg._windDirections[hour];

	for (int i = 0; i < pkg._height; i++) {
		for (int j = 0; j < pkg._width; j++) {
			Tree* treeAt = &(pkg._treeList[i][j]);
			int treeState = treeAt->_state;
			if (treeState == TreeState::FIRE || treeState == TreeState::BURNING_DEAD) {
				treeAt->_burnedTime++;


				if (treeAt->_burnedTime < 1) continue;
				burningTree++;
				int currentSpreadCount = spreadAt(pkg, j, i, currentDirection);
				burningTree += currentSpreadCount;

				int maxBurnTime = (treeState == TreeState::FIRE) ? pkg._burnTime : 1;
				if (treeAt->_burnedTime >= maxBurnTime) {
					burningTree--;
					treeAt->_state = (treeState == TreeState::FIRE) ? TreeState::DEAD : TreeState::BURNED;
					treeAt->_burnedTime = -hour;
					continue;
				}
			}

		}
	}
}

void startSimulation(Tree** treeList, const int& width, const int& height, const int& burningTime, std::vector<int> windDirections) {
	SimulationData pkg;
	pkg._treeList = treeList;
	pkg._width = width;
	pkg._height = height;
	pkg._windDirections = windDirections;
	pkg._hour = 1;
	pkg._averageSpread = 0;
	pkg._maxBurningPerHour = 0;
	pkg._maxBurningHour = 0;
	pkg._burnTime = burningTime;

	int treeBurningSum = 0;
	int addedTime = 0;
	int totalSpreadCount = 0;
	int k = 0;
	for (; k < windDirections.size(); k++) {
		pkg._hour = k;
		int burningTree = 0;
		tick(pkg, totalSpreadCount, burningTree, k);
		if (burningTree > pkg._maxBurningPerHour) {
			pkg._maxBurningPerHour = burningTree;
			pkg._maxBurningHour = k;
		}
		treeBurningSum += burningTree;
		if (burningTree == 0) break;
		addedTime++;

		printCurrentWind(pkg._windDirections[k]);
		printForest(pkg._treeList, pkg._width, pkg._height, k + 1);
		std::cout << std::endl;
	}

	std::cout << "added time: " << addedTime << "\n";
	double avg = (treeBurningSum * 1.0) / k;
	std::cout << "The forest took " << k << " hour to burn out." << std::endl;
	std::cout << "The maximum number of burning trees at once was " << pkg._maxBurningPerHour << " trees at hour " << pkg._maxBurningHour + 1 << "." << std::endl;
	std::cout << "The average number of burning trees per hour was " << std::fixed << std::setprecision(2) << avg << " trees." << std::endl;
}