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

PathNode* Visit(PathNode* pFrom, const Vector2 to, std::vector<bool>& visited, const unsigned char* pMap, const int& nWidth, const int& nHeight)
{
	const int index = to.PosToIndex();
	if (to.X < 0 || to.Y < 0 || to.X >= nWidth || to.Y >= nHeight || visited[index])
		return nullptr;

	visited[index] = true;
	return pMap[index] ? new PathNode(to, pFrom) : nullptr;
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

void FreeQueuedNodes(std::vector<PathNode*>& pDirectedPriorityQueues)
{
	while (PathNode * ptr = MoveToNext(pDirectedPriorityQueues))
	{
		delete ptr;
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
	const int yStep = sizeof(char) * nMapWidth;
	const int xStep = sizeof(char);

	const Vector2 target(nTargetX, nTargetY, xStep, yStep);
	const Vector2 start(nStartX, nStartY, xStep, yStep);

	// Cast to cleanup overflow warning.
	std::vector<bool> visited(static_cast<const int64_t>(nMapHeight) * nMapWidth, 0);
	visited[start.PosToIndex()] = true;

	std::vector<PathNode*> availableNodes;
	auto current = new PathNode(start, nullptr);
	current->SetScore(target);

	int stepCount = NO_PATH_EXISTS;
	while (current)
	{
		if (current->GetPosition() == target)
		{
			stepCount = current->GetDistanceFromStart();
			while (current->GetLinkedNode())
			{
				pOutBuffer[current->GetDistanceFromStart() - 1] = current->GetPosition().PosToIndex();
				auto currPtr = current;
				current = current->GetLinkedNode();
				delete currPtr;
			}

			break;
		}

		// If we're here and current->GetDistanceFromStart() == nOutBufferSize skip 
		// visitation, this is no longer a path, but it's worth testing remaining potentials
		// in case we have some matching scores that were unlucky in sorting.
		if (current->GetDistanceFromStart() < nOutBufferSize)
		{
			PathNode* l = Visit(current, STEP_LEFT(current), visited, pMap, nMapWidth, nMapHeight);
			PathNode* r = Visit(current, STEP_RIGHT(current), visited, pMap, nMapWidth, nMapHeight);
			PathNode* u = Visit(current, STEP_UP(current), visited, pMap, nMapWidth, nMapHeight);
			PathNode* d = Visit(current, STEP_DOWN(current), visited, pMap, nMapWidth, nMapHeight);
			Enqueue(availableNodes, l, r, u, d, target);
		}
				
		current = MoveToNext(availableNodes);		
	}

	FreeQueuedNodes(availableNodes);
	while (current)
	{
		auto currPtr = current;
		current = current->GetLinkedNode();
		delete currPtr;
	}
	
	return stepCount;
}
