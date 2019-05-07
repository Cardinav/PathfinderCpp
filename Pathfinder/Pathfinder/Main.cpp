
#include "Pathfinder.h"
#include <iostream>


void Test5By5Complex(
	int* outBuffer,
	int startX = 2,
	int startY = 2,
	int endX = 0,
	int endY = 0,
	int mapWidth = 5,
	int mapHeight = 5,	
	const int outBufferLen = 8)
{
	std::cout << "============5by5Complex============" << std::endl;
	// Test code.
	unsigned char map[25] =
	{
		1, 1, 1, 1, 1,
		1, 0, 0, 0, 1,
		1, 0, 1, 1, 1,
		1, 0, 0, 1, 0,
		1, 1, 1, 1, 0,
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
}

void Test8By8Complex(
	int* outBuffer,
	int startX = 0,
	int startY = 0,
	int endX = 2,
	int endY = 3,
	int mapWidth = 8,
	int mapHeight = 8,
	const int outBufferLen = 200)
{
	std::cout << "============8by8Complex============" << std::endl;
	// Test code.
	unsigned char map[64] =
	{
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 0, 0, 0, 0, 1, 1, 1,
		1, 0, 1, 1, 1, 1, 1, 1,
		1, 0, 1, 0, 1, 1, 1, 1,
		1, 0, 0, 0, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1,
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
}

void Test6By6Widening(
	int* outBuffer,
	int startX = 0,
	int startY = 0,
	int endX = 5,
	int endY = 0,
	int mapWidth = 6,
	int mapHeight = 6,
	const int outBufferLen = 20)
{
	std::cout << "============6by6Widening============" << std::endl;
	// Test code.
	unsigned char map[36] =
	{
		1, 1, 1, 1, 0, 1,
		1, 0, 0, 1, 0, 1,
		1, 0, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 0,
		1, 0, 1, 1, 1, 0,
		1, 0, 1, 1, 1, 0,
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
}

void Test5By5HollowCenter(
	int* outBuffer,
	int startX = 0,
	int startY = 0,
	int endX = 2,
	int endY = 2,
	int mapWidth = 5,
	int mapHeight = 5,
	const int outBufferLen = 12)
{
	std::cout << "============5by5HollowCenter============" << std::endl;

	// Test code.
	unsigned char map[25] =
	{
		1, 1, 1, 1, 1,
		1, 0, 0, 0, 1,
		1, 0, 1, 0, 1,
		1, 0, 0, 0, 1,
		1, 1, 1, 1, 1,
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
}

void Test5By5ZigZag(
	int* outBuffer,
	int startX = 0,
	int startY = 0,
	int endX = 4,
	int endY = 4,
	int mapWidth = 5,
	int mapHeight = 5,
	const int outBufferLen = 12)
{
	std::cout << "============5by5ZigZag============" << std::endl;

	// Test code.
	unsigned char map[25] =
	{
		1, 1, 0, 0, 1,
		0, 1, 0, 0, 1,
		1, 1, 1, 0, 0,
		1, 0, 1, 1, 0,
		1, 0, 1, 1, 1,
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
}

void Test5By5Open(
	int* outBuffer,
	int startX = 0,
	int startY = 0,
	int endX = 4,
	int endY = 4,
	int mapWidth = 5,
	int mapHeight = 5,
	const int outBufferLen = 12)
{
	std::cout << "============5by5Open============" << std::endl;

	// Test code.
	unsigned char map[25] =
	{
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
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
}

void Test5By4Winding(
	int* outBuffer,
	int startX = 0,
	int startY = 0,
	int endX = 4,
	int endY = 2,
	int mapWidth = 5,
	int mapHeight = 4,
	const int outBufferLen = 12)
{
	std::cout << "============5by4Winding============" << std::endl;

	// Test code.
	unsigned char map[25] =
	{
		1, 0, 1, 1, 1,
		1, 0, 1, 0, 1,
		1, 1, 1, 0, 1,
		0, 0, 1, 1, 1,
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
}

void Test5By5MiddleOut(
	int* outBuffer,
	int startX = 2,
	int startY = 2,
	int endX = 0,
	int endY = 0,
	int mapWidth = 5,
	int mapHeight = 5,
	const int outBufferLen = 12)
{
	std::cout << "============5by5MiddleOut============" << std::endl;

	// Test code.
	unsigned char map[25] =
	{
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
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
}

void Test5By5Reverse(
	int* outBuffer,
	int startX = 4,
	int startY = 4,
	int endX = 0,
	int endY = 0,
	int mapWidth = 5,
	int mapHeight = 5,
	const int outBufferLen = 12)
{
	std::cout << "============5by5Reverse============" << std::endl;

	// Test code.
	unsigned char map[25] =
	{
		1, 1, 0, 1, 1,
		1, 1, 0, 1, 1,
		1, 1, 0, 1, 1,
		1, 1, 1, 1, 1,
		1, 1, 1, 1, 1,
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
}

int main()
{
	const int outBufferLen = 25;
	int outBuffer[outBufferLen];
	//Test5By5Complex(outBuffer);
	//Test5By5HollowCenter(outBuffer);
	//Test5By5ZigZag(outBuffer);
	//Test5By5Open(outBuffer);
	Test8By8Complex(outBuffer);
	//Test6By6Widening(outBuffer);
	//Test5By5MiddleOut(outBuffer);
	//Test5By5Reverse(outBuffer);
	//Test5By4Winding(outBuffer);

	return 0;
}
