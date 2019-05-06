
#include "Pathfinder.h"
#include <iostream>

int main()
{
	// Test code.
	int startX = 0, startY = 0, endX = 3, endY = 3;
	int mapWidth = 4, mapHeight = 4;
	const int outBufferLen = 20;
	int outBuffer[outBufferLen];
	unsigned char map[16] =
	{
		1, 1, 0, 1,
		1, 0, 1, 0,
		1, 0, 1, 0,
		1, 1, 1, 1,
	};

	int steps = FindPath(startX, startY, endX, endY, map, mapWidth, mapHeight, outBuffer, outBufferLen);
	if (steps == NO_PATH_EXISTS)
	{
		std::cout << "No path exists." << std::endl;
	}
	else
	{
		char outStr[16];
		sprintf_s(outStr, "Steps %d", steps);
		std::cout << outStr << std::endl;
	}

	int counter = steps == NO_PATH_EXISTS ? outBufferLen : steps;

	for (int i = 0; i < counter; i++)
	{
		std::cout << outBuffer[i] << std::endl;
	}

	return steps;
}