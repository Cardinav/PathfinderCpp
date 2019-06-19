#include "Pathfinder.h"
#include <vector>
#include <queue>
#include <cmath>

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
	bool PosIsValid(const Vector2& pos) const { return IndexIsValid(PosToIndex(pos)); }
	bool IndexIsValid(const int& index) const { return m_validChar == m_pMap[index]; }
};

class PathNode
{
	PathNode* m_from;
	Vector2 m_position;
	int m_distanceFromStart;
	PathNode()
	{
		m_from = nullptr;
		m_distanceFromStart = 0;
	}

public:
	PathNode(Vector2 pos, PathNode* from)
	{
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

	Vector2& GetPosition()			{ return m_position; }
	int& GetDistanceFromStart()		{ return m_distanceFromStart; }
	PathNode* GetLinkedNode() const	{ return m_from; }
};

bool TestPosition(const Vector2& to, const MapContext& context)
{
	return to.X >= 0 && to.Y >= 0 && to.X < context.m_mapWidth && to.Y < context.m_mapHeight && context.PosIsValid(to);
}

PathNode* Visit(PathNode* pFrom, const Vector2 to, std::vector<PathNode*>& visited, const MapContext& context)
{
	const int index = context.PosToIndex(to);
	if (!TestPosition(to, context) || visited[index])
	{
		return nullptr;
	}

	visited[index] = new PathNode(to, pFrom);	
	return visited[index];
}

PathNode* MoveToNext(std::queue<PathNode*>& queue)
{	
	if (queue.empty())
	{
		return nullptr;
	}

	const auto ret = queue.front();
	queue.pop();
	return ret;
}

void AddToQueueIfExists(PathNode* pPathNode, std::queue<PathNode*>& queue)
{
	if (pPathNode)
	{
		queue.push(pPathNode);
	}
}

void Enqueue(
	std::queue<PathNode*>& queue,
	PathNode* l, 
	PathNode* r, 
	PathNode* u, 
	PathNode* d)
{
	AddToQueueIfExists(l, queue);
	AddToQueueIfExists(r, queue);
	AddToQueueIfExists(u, queue);
	AddToQueueIfExists(d, queue);
}

bool SanityTestContext(const MapContext& context)
{
	const Vector2& start  = context.m_start;
	const Vector2& target = context.m_target;

	// Sanity the significant points are viable...
	if (!context.PosIsValid(start) || !context.PosIsValid(target))
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
	{
		delete node;
	}
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
	std::queue<PathNode*> availableNodes;
	auto pCurrent = new PathNode(start, nullptr);

	pathMap[context.PosToIndex(start)] = pCurrent;
		
	int stepCount = NO_PATH_EXISTS;
	while (pCurrent)
	{
		auto& currentNode = *pCurrent;
		const auto& thisPosition = currentNode.GetPosition();
		if (thisPosition == target)
		{
			stepCount = currentNode.GetDistanceFromStart();
			break;
		}

		if (thisPosition.GetMinDistanceTo(target) < nOutBufferSize - currentNode.GetDistanceFromStart())
		{
			PathNode* l = Visit(pCurrent, Vector2(thisPosition.X - 1, thisPosition.Y), pathMap, context);
			PathNode* r = Visit(pCurrent, Vector2(thisPosition.X + 1, thisPosition.Y), pathMap, context);
			PathNode* u = Visit(pCurrent, Vector2(thisPosition.X, thisPosition.Y - 1), pathMap, context);
			PathNode* d = Visit(pCurrent, Vector2(thisPosition.X, thisPosition.Y + 1), pathMap, context);
			Enqueue(availableNodes, l, r, u, d);
		}

		pCurrent = MoveToNext(availableNodes);
	}

	PathNode* goal = pathMap[context.PosToIndex(target)];
	if (goal && stepCount != NO_PATH_EXISTS)
	{
		while (goal->GetLinkedNode())
		{
			pOutBuffer[goal->GetDistanceFromStart() - 1] = context.PosToIndex(goal->GetPosition());
			goal = goal->GetLinkedNode();
		}
	}

	FreeNodes(pathMap);
	return stepCount;
}
