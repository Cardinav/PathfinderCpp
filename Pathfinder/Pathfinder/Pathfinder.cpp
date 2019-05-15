#include "Pathfinder.h"
#include <vector>
#include <queue>
#include <cmath>

#define STEP_LEFT(node)	 Vector2(node->GetPosition().X - 1, node->GetPosition().Y)
#define STEP_RIGHT(node) Vector2(node->GetPosition().X + 1, node->GetPosition().Y)
#define STEP_UP(node)	 Vector2(node->GetPosition().X, node->GetPosition().Y - 1)
#define STEP_DOWN(node)  Vector2(node->GetPosition().X, node->GetPosition().Y + 1)

#define INT_MAX 2147483647 // Redefining here for environment compatibility.

// Compiled using Visual Studio 16.0.1

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
	bool operator==(const Vector2 r) const { return X == r.X && Y == r.Y; }
	bool operator!=(const Vector2 r) const { return X != r.X || Y != r.Y; }
};

struct MapContext
{
	MapContext(const unsigned char* pMap, int mapWidth, int mapHeight, int xStep, int yStep, const Vector2& start, const Vector2& target)
	{
		Map = pMap;
		MapWidth = mapWidth;
		MapHeight = mapHeight;
		X_Step = xStep;
		Y_Step = yStep;
		Start = start;
		Target = target;
	}

	const unsigned char* Map;
	int MapWidth;
	int MapHeight;
	int X_Step;
	int Y_Step;
	Vector2 Start;
	Vector2 Target;
	int PosToIndex(const Vector2& pos) const { return pos.X * X_Step + pos.Y * Y_Step; }
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

bool TestPosition(const Vector2& to, const MapContext& context)
{
	return !(to.X < 0 || to.Y < 0 || to.X >= context.MapWidth || to.Y >= context.MapHeight || !context.Map[context.PosToIndex(to)]);
}

PathNode* Visit(PathNode* pFrom, const Vector2 to, std::vector<PathNode*>& visited, const MapContext& context)
{
	if (!TestPosition(to, context))
		return nullptr;

	const int index = context.PosToIndex(to);
	if (visited[index] && pFrom->GetDistanceFromStart() + 1 >= visited[index]->GetDistanceFromStart())
		return nullptr;

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

	auto ret = priorityHeap[0];
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
	const Vector2& start  = context.Start;
	const Vector2& target = context.Target;

	// Sanity the points are viable...
	if (!context.Map[context.PosToIndex(context.Start)] || !context.Map[context.PosToIndex(target)])
	{
		return false;
	}

	// ...and aren't themselves islands. L, U, R, D
	if ((!TestPosition(Vector2(start.X - 1, start.Y), context) &&
		!TestPosition(Vector2(start.X, start.Y - 1), context) &&
		!TestPosition(Vector2(start.X + 1, start.Y), context) &&
		!TestPosition(Vector2(start.X, start.Y + 1), context)) ||
		(!TestPosition(Vector2(target.X - 1, target.Y), context) &&
		!TestPosition(Vector2(target.X, target.Y - 1), context) &&
		!TestPosition(Vector2(target.X + 1, target.Y), context) &&
		!TestPosition(Vector2(target.X, target.Y + 1), context)))
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
	// Minimum distance required.
	if (abs(nTargetX - nStartX) + abs(nTargetY - nStartY) > nOutBufferSize)
	{
		return NO_PATH_EXISTS;
	}

	const int yStep = sizeof(char) * nMapWidth;
	const int xStep = sizeof(char);

	const Vector2 target(nTargetX, nTargetY);
	const Vector2 start(nStartX, nStartY);

	const MapContext context(pMap, nMapWidth, nMapHeight, xStep, yStep, start, target);
	if (!SanityTestContext(context))
	{
		return NO_PATH_EXISTS;
	}

	// Cast to cleanup overflow warning.
	std::vector<PathNode*> pathMap(static_cast<const int64_t>(nMapHeight) * nMapWidth, nullptr);
	std::vector<PathNode*> availableNodes;
	auto current = new PathNode(start, nullptr);

	current->SetScore(target);
	pathMap[context.PosToIndex(start)] = current;

	int stepCount = NO_PATH_EXISTS;
	while (current)
	{
		if (current->GetPosition() == target)
		{
			stepCount = current->GetDistanceFromStart();
		}

		// Initially we want to be sure we have enough rope to reach the target, and once
		// we've found it we want to make sure not to walk any further than we already know we have to.
		if (current->GetDistanceFromStart() < nOutBufferSize && (stepCount == NO_PATH_EXISTS || current->GetDistanceFromStart() < stepCount))
		{
			PathNode* l = Visit(current, STEP_LEFT(current), pathMap, context);
			PathNode* r = Visit(current, STEP_RIGHT(current), pathMap, context);
			PathNode* u = Visit(current, STEP_UP(current), pathMap, context);
			PathNode* d = Visit(current, STEP_DOWN(current), pathMap, context);
			Enqueue(availableNodes, l, r, u, d, target);
		}
		
		current = MoveToNext(availableNodes);
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
