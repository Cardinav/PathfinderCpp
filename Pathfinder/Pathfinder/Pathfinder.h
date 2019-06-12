#pragma once

#define DEBUG_OUTPUT 0

constexpr auto IMPASSIBLE = 0;
constexpr auto NO_PATH_EXISTS = -1;
constexpr auto PROGRESS = 1;
constexpr auto PUSH = 0;
constexpr auto NO_PROGRESS = -1;

int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize);