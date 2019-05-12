#include "Pathfinder.h"
#include <vector>
#include <queue>
#include <cmath>

#define STEP_LEFT(node) Vector2(node->GetPosition().X - 1, node->GetPosition().Y, xStep, yStep)
#define STEP_RIGHT(node) Vector2(node->GetPosition().X + 1, node->GetPosition().Y, xStep, yStep)
#define STEP_UP(node) Vector2(node->GetPosition().X, node->GetPosition().Y - 1, xStep, yStep)
#define STEP_DOWN(node) Vector2(node->GetPosition().X, node->GetPosition().Y + 1, xStep, yStep)

#define INT_MAX 2147483647 // Redefining here for environment compatibility.

// Compiled using Visual Studio 16.0.1

struct Vector2
{
	Vector2() : X(0), Y(0), m_xStep(1), m_yStep(1) {}
	Vector2(int x, int y, int xStep, int yStep) 
	{
		X		= x;
		Y		= y;
		m_xStep = xStep;
		m_yStep = yStep;
	}

	int X;
	int Y;

	int m_xStep;
	int m_yStep;

	int PosToIndex()				 const { return X * m_xStep + Y * m_yStep; }
	int SqDistance(const Vector2 to) const { return pow(X - to.X, 2) + pow(Y - to.Y, 2); }

	bool operator==(const Vector2 r) const { return X == r.X && Y == r.Y; }
	bool operator!=(const Vector2 r) const { return X != r.X || Y != r.Y; }
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
		int dist = m_position.SqDistance(target);
		m_score = dist == 0 ? INT_MAX : 1.0f / m_position.SqDistance(target);
	}
	Vector2& GetPosition()			{ return m_position; }
	int& GetDistanceFromStart()		{ return m_distanceFromStart; }
	PathNode* GetLinkedNode() const	{ return m_from; }
};

PathNode* Visit(PathNode* pFrom, const Vector2 to, std::vector<PathNode*>& visited, const unsigned char* pMap, const int& nWidth, const int& nHeight)
{
	const int index = to.PosToIndex();
	if (to.X < 0 || to.Y < 0 || to.X >= nWidth || to.Y >= nHeight || !pMap[index])
		return nullptr;

	if (visited[index] && pFrom->GetDistanceFromStart() + 1 >= visited[index]->GetDistanceFromStart())
		return nullptr;

	visited[index] = new PathNode(to, pFrom);
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

	const Vector2 target(nTargetX, nTargetY, xStep, yStep);
	const Vector2 start(nStartX, nStartY, xStep, yStep);

	// Cast to cleanup overflow warning.
	std::vector<PathNode*> pathMap(static_cast<const int64_t>(nMapHeight) * nMapWidth, nullptr);
	std::vector<PathNode*> availableNodes;
	auto current = new PathNode(start, nullptr);
	current->SetScore(target);
	pathMap[start.PosToIndex()] = current;

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
			PathNode* l = Visit(current, STEP_LEFT(current), pathMap, pMap, nMapWidth, nMapHeight);
			PathNode* r = Visit(current, STEP_RIGHT(current), pathMap, pMap, nMapWidth, nMapHeight);
			PathNode* u = Visit(current, STEP_UP(current), pathMap, pMap, nMapWidth, nMapHeight);
			PathNode* d = Visit(current, STEP_DOWN(current), pathMap, pMap, nMapWidth, nMapHeight);
			Enqueue(availableNodes, l, r, u, d, target);
		}
		
		current = MoveToNext(availableNodes);
	}

	PathNode* goal = pathMap[target.PosToIndex()];
	if (goal)
	{
		while (goal->GetLinkedNode())
		{
			pOutBuffer[goal->GetDistanceFromStart() - 1] = goal->GetPosition().PosToIndex();
			auto currPtr = goal;
			goal = goal->GetLinkedNode();
			delete currPtr;
		}
	}

	return stepCount;
}
