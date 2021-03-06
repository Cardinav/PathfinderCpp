#include "Pathfinder.h"
#include <vector>
#include <queue>
#include <cmath>
#if DEBUG_OUTPUT
#include <iostream>
#include <string>
#endif
#define INT_MAX 2147483647 // Redefining here for environment compatibility.

struct Vector2
{
	Vector2() : X(0), Y(0) {}
	Vector2(int x, int y) 
	{
		X = x;
		Y = y;
	}

	int X;
	int Y;

	int SqDistance(const Vector2 to) const { return pow(X - to.X, 2) + pow(Y - to.Y, 2); }
	int GetMinDistanceTo(const Vector2& to) const { return abs(to.X - X) + abs(to.Y - Y); }
	bool operator==(const Vector2 r) const { return X == r.X && Y == r.Y; }
	bool operator!=(const Vector2 r) const { return X != r.X || Y != r.Y; }
};

struct MapContext
{
	const unsigned char* m_pMap;
	int m_mapWidth;
	int m_mapHeight;
	int m_xStep;
	int m_yStep;
	Vector2 m_start;
	Vector2 m_target;
	char m_validChar;

	MapContext(
		const unsigned char* pMap, 
		int mapWidth, 
		int mapHeight, 
		int xStep, 
		int yStep, 
		const Vector2& start, 
		const Vector2& target)
	{
		m_pMap = pMap;
		m_mapWidth = mapWidth;
		m_mapHeight = mapHeight;
		m_xStep = xStep;
		m_yStep = yStep;
		m_start = start;
		m_target = target;
		auto& exampleEntry = m_pMap[PosToIndex(m_start)];
		m_validChar = exampleEntry == 48 /* '0' */ || exampleEntry == 49 /* '1' */ ? '1' : 1;
	}

	int PosToIndex(const Vector2& pos) const { return (pos.X * m_xStep) + (pos.Y * m_yStep); }
	bool IndexIsValid(const int& index) const { return m_validChar == m_pMap[index]; }
};

class PathNode
{
	PathNode* m_from;
	Vector2 m_position;
	int m_distanceFromStart;
	float m_score;
	PathNode()
	{
		m_score = 0;
		m_from = nullptr;
		m_distanceFromStart = 0;
	}

public:
	PathNode(Vector2 pos, PathNode* from)
	{
		m_score = 0;
		m_position = pos;
		if (from)
		{
			m_distanceFromStart = from->m_distanceFromStart + 1;
			m_from = from;
		}
		else 
		{
			m_distanceFromStart = 0;
			m_from = nullptr;
		}		
	}

	const float& GetScore() const { return m_score; }
	void SetScore(const Vector2& target)
	{
		int distance = m_position.SqDistance(target);
		m_score = distance == 0 ? INT_MAX : 1.0f / m_position.SqDistance(target);
	}
	Vector2& GetPosition()			{ return m_position; }
	int& GetDistanceFromStart()		{ return m_distanceFromStart; }
	PathNode* GetLinkedNode() const	{ return m_from; }
};

void PrintDebug(PathNode* pFrom, const Vector2& to, const MapContext& context)
{
#if DEBUG_OUTPUT
	Vector2& fromPos = pFrom->GetPosition();
	int index = context.PosToIndex(to);
	char buffer[64];
	sprintf_s(
		buffer, 
		"Point: %c - %d; (%d, %d) To (%d, %d)", 
		context.m_pMap[context.PosToIndex(to)], 
		index,
		fromPos.X, 
		fromPos.Y,
		to.X, 
		to.Y);
	std::cout << buffer << std::endl;
#endif
}

bool TestPosition(const Vector2& to, const MapContext& context)
{
	return !(to.X < 0 || to.Y < 0 || to.X >= context.m_mapWidth || to.Y >= context.m_mapHeight || !context.IndexIsValid(context.PosToIndex(to)));
}

PathNode* Visit(PathNode* pFrom, const Vector2 to, std::vector<PathNode*>& visited, const MapContext& context)
{
	PrintDebug(pFrom, to, context);
	if (!TestPosition(to, context))
	{
		return nullptr;
	}

	const int index = context.PosToIndex(to);
	if (visited[index] && pFrom->GetDistanceFromStart() + 1 >= visited[index]->GetDistanceFromStart())
	{
		return nullptr;
	}

	if (!visited[index])
	{
		visited[index] = new PathNode(to, pFrom);
	}
	else
	{
		*visited[index] = PathNode(to, pFrom);
	}
	
	return visited[index];
}

bool Compare(PathNode* a, PathNode* b)
{
	if (!a) return false;
	if (!b) return true;
	return a->GetScore() < b->GetScore();
}

PathNode* MoveToNext(std::vector<PathNode*>& priorityHeap)
{	
	if (priorityHeap.empty())
	{
		return nullptr;
	}

	const auto ret = priorityHeap[0];
	std::pop_heap(priorityHeap.begin(), priorityHeap.end(), Compare);
	priorityHeap.pop_back();
	return ret;
}

void AddToHeapIfExists(PathNode* pPathNode, const Vector2& target, std::vector<PathNode*>& priorityHeap)
{
	if (pPathNode)
	{
		pPathNode->SetScore(target);
		priorityHeap.push_back(pPathNode);
		std::push_heap(priorityHeap.begin(), priorityHeap.end(), Compare);
	}
}

void Enqueue(
	std::vector<PathNode*>& priorityHeap,
	PathNode* l, 
	PathNode* r, 
	PathNode* u, 
	PathNode* d, 
	const Vector2& target)
{
	AddToHeapIfExists(l, target, priorityHeap);
	AddToHeapIfExists(u, target, priorityHeap);
	AddToHeapIfExists(r, target, priorityHeap);
	AddToHeapIfExists(d, target, priorityHeap);	
}

bool SanityTestContext(const MapContext& context)
{
	const Vector2& start  = context.m_start;
	const Vector2& target = context.m_target;

	// Sanity the significant points are viable...
	if (!context.IndexIsValid(context.PosToIndex(start)) || !context.IndexIsValid(context.PosToIndex(target)))
	{
		return false;
	}

	// ...and target isn't an island. L, U, R, D
	if (!TestPosition(Vector2(target.X - 1, target.Y), context) &&
		!TestPosition(Vector2(target.X, target.Y - 1), context) &&
		!TestPosition(Vector2(target.X + 1, target.Y), context) &&
		!TestPosition(Vector2(target.X, target.Y + 1), context))
	{
		return false;
	}

	return true;
}

void FreeNodes(const std::vector<PathNode*>& nodes)
{
	for (const auto& node : nodes)
		delete node;
}

int FindPath(
	const int nStartX, 
	const int nStartY,
	const int nTargetX, 
	const int nTargetY,
	const unsigned char* pMap, 
	const int nMapWidth, 
	const int nMapHeight,
	int* pOutBuffer, 
	const int nOutBufferSize)
{
	const int yStep = nMapWidth;
	const int xStep = 1;

	const Vector2 target(nTargetX, nTargetY);
	const Vector2 start(nStartX, nStartY);
	if (start.GetMinDistanceTo(target) > nOutBufferSize)
	{
		return NO_PATH_EXISTS;
	}

	const MapContext context(pMap, nMapWidth, nMapHeight, xStep, yStep, start, target);
	if (!SanityTestContext(context))
	{
		return NO_PATH_EXISTS;
	}

	// Cast to cleanup overflow warning.
	std::vector<PathNode*> pathMap(static_cast<const int64_t>(nMapHeight) * nMapWidth, nullptr);
	std::vector<PathNode*> availableNodes;
	auto pCurrent = new PathNode(start, nullptr);

	pCurrent->SetScore(target);
	pathMap[context.PosToIndex(start)] = pCurrent;
		
	int stepCount = NO_PATH_EXISTS;
	while (pCurrent)
	{
		auto& currentNode = *pCurrent;
		const auto& thisPosition = currentNode.GetPosition();
		if (thisPosition == target)
		{
			// Not backing out here once we've found the target is a weakness on larger maps with 
			// little obstruction, in terms of big O and a massive pOutputBufferLen, but on a map 
			// where all nodes score the same and have the same weight, it ensures correctness.
			stepCount = currentNode.GetDistanceFromStart();
		}
		
		// Initially we want to be sure we have enough rope to reach the target, and once
		// we've found it we want to make sure not to walk any further than we already know we have to.
		if (thisPosition.GetMinDistanceTo(target) <= nOutBufferSize - currentNode.GetDistanceFromStart() &&
			(stepCount == NO_PATH_EXISTS || currentNode.GetDistanceFromStart() + thisPosition.GetMinDistanceTo(target) <= stepCount))
		{
			PathNode* l = Visit(pCurrent, Vector2(thisPosition.X - 1, thisPosition.Y), pathMap, context);
			PathNode* r = Visit(pCurrent, Vector2(thisPosition.X + 1, thisPosition.Y), pathMap, context);
			PathNode* u = Visit(pCurrent, Vector2(thisPosition.X, thisPosition.Y - 1), pathMap, context);
			PathNode* d = Visit(pCurrent, Vector2(thisPosition.X, thisPosition.Y + 1), pathMap, context);
			Enqueue(availableNodes, l, r, u, d, target);				
		}
		
		pCurrent = MoveToNext(availableNodes);
	}

	PathNode* goal = pathMap[context.PosToIndex(target)];
	if (goal)
	{
		while (goal->GetLinkedNode())
		{
			pOutBuffer[goal->GetDistanceFromStart() - 1] = context.PosToIndex(goal->GetPosition());
			goal = goal->GetLinkedNode();
		}
	}

	FreeNodes(availableNodes);
	FreeNodes(pathMap);
	return stepCount;
}
