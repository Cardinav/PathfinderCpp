#include "Pathfinder.h"
#include <vector>
#include <queue>
#include <cmath>

#define STEP_LEFT(node) Vector2(node->GetPosition().X - 1, node->GetPosition().Y, xStep, yStep)
#define STEP_RIGHT(node) Vector2(node->GetPosition().X + 1, node->GetPosition().Y, xStep, yStep)
#define STEP_UP(node) Vector2(node->GetPosition().X, node->GetPosition().Y - 1, xStep, yStep)
#define STEP_DOWN(node) Vector2(node->GetPosition().X, node->GetPosition().Y + 1, xStep, yStep)

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

	int PosToIndex() const { return X * m_xStep + Y * m_yStep; }

	bool operator==(const Vector2 r) const { return X == r.X && Y == r.Y; }
	bool operator!=(const Vector2 r) const { return X != r.X || Y != r.Y; }
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

PathNode* Visit(PathNode* pFrom, const Vector2 to, std::vector<bool>& visited, const unsigned char* pMap, const int& nWidth, const int& nHeight)
{
	const int index = to.PosToIndex();
	if (to.X < 0 || to.Y < 0 || to.X >= nWidth || to.Y >= nHeight || visited[index])
		return nullptr;

	visited[index] = 1;
	return pMap[index] ? new PathNode(to, pFrom) : nullptr;
}

int GetXScore(PathNode* pTo, Vector2& current, Vector2& target)
{
	if (!pTo || current.X == target.X) 
		return PUSH;
	if (abs(current.X - target.X) >= abs(pTo->GetPosition().X - target.X)) 
		return PROGRESS;

	return NO_PROGRESS;
}

int GetYScore(PathNode* pTo, Vector2& current, Vector2& target)
{
	if (!pTo || current.Y == target.Y) 
		return PUSH;
	if (abs(current.Y - target.Y) >= abs(pTo->GetPosition().Y - target.Y)) 
		return PROGRESS;

	return NO_PROGRESS;
}

void SetInsertionOrder(PathNode* pA, PathNode* pB, PathNode** pOutFirst, PathNode** pOutSecond, bool favorA)
{
	if (favorA)
	{
		*pOutFirst = pA;
		*pOutSecond = pB;
	}
	else
	{
		*pOutFirst = pB;
		*pOutSecond = pA;
	}
}

PathNode* MoveToNext(std::queue<PathNode*>* pVisitQueues)
{
	PathNode* next = nullptr;
	for (int i = 0; i < 4; i++)
	{
		if (!pVisitQueues[i].empty())
		{
			next = pVisitQueues[i].front();
			pVisitQueues[i].pop();
			break;
		}
	}

	return next;
}

PathNode* EnqueueAndMoveToNext(
	std::queue<PathNode*>* pPriorityQueues, 
	PathNode* l, 
	PathNode* r, 
	PathNode* u, 
	PathNode* d, 
	Vector2& current, 
	Vector2& target)
{
	// What I'd really like here is to consider the score of each node relative to the goal
	// and have the queue live in a single heap, instead of only testing progress, but at least 
	// with a binary progress/no progress we ensure each step we're always heading in the right 
	// direction when possible, even if we're not looking back for the most recent optimal node.

	PathNode* first  = nullptr;
	PathNode* second = nullptr;
	PathNode* third  = nullptr;
	PathNode* fourth = nullptr;

	const int lScore = GetXScore(l, current, target);
	const int rScore = GetXScore(r, current, target);
	const int uScore = GetYScore(u, current, target);
	const int dScore = GetYScore(d, current, target);
	if (lScore == PROGRESS || rScore == PROGRESS)
	{
		SetInsertionOrder(l, r, &first, &fourth, lScore == PROGRESS);
		SetInsertionOrder(u, d, &second, &third, uScore == PROGRESS);
	}
	else
	{
		SetInsertionOrder(u, d, &first, &fourth, uScore == PROGRESS);
		SetInsertionOrder(l, r, &second, &third, lScore == PROGRESS);
	}

	if (first)  pPriorityQueues[0].push(first);
	if (second) pPriorityQueues[1].push(second);
	if (third)  pPriorityQueues[2].push(third);
	if (fourth) pPriorityQueues[3].push(fourth);

	return MoveToNext(pPriorityQueues);
}

void FreeQueuedNodes(std::queue<PathNode*>* pDirectedPriorityQueues)
{
	while (PathNode * ptr = MoveToNext(pDirectedPriorityQueues))
	{
		delete ptr;
	}
}

int FindPath(const int nStartX, const int nStartY,
	const int nTargetX, const int nTargetY,
	const unsigned char* pMap, const int nMapWidth, const int nMapHeight,
	int* pOutBuffer, const int nOutBufferSize)
{
	const int yStep = sizeof(char) * nMapWidth;
	const int xStep = sizeof(char);

	Vector2 target(nTargetX, nTargetY, xStep, yStep);
	Vector2 start(nStartX, nStartY, xStep, yStep);

	// Cast to cleanup overflow warning.
	std::vector<bool> visited(static_cast<const int64_t>(nMapHeight) * nMapWidth, 0);
	visited[start.PosToIndex()] = 1;

	std::queue<PathNode*> directedPriorityQueues[4];	
	auto current = new PathNode(start, nullptr);

	int stepCount = NO_PATH_EXISTS;
	
	while (current && current->GetDistanceFromStart() <= nOutBufferSize)
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
				
		PathNode* l = Visit(current, STEP_LEFT(current), visited, pMap, nMapWidth, nMapHeight);
		PathNode* r = Visit(current, STEP_RIGHT(current), visited, pMap, nMapWidth, nMapHeight);
		PathNode* u = Visit(current, STEP_UP(current), visited, pMap, nMapWidth, nMapHeight);
		PathNode* d = Visit(current, STEP_DOWN(current), visited, pMap, nMapWidth, nMapHeight);
		current = EnqueueAndMoveToNext(directedPriorityQueues, l, r, u, d, current->GetPosition(), target);
	}

	FreeQueuedNodes(directedPriorityQueues);
	while (current)
	{
		auto currPtr = current;
		current = current->GetLinkedNode();
		delete currPtr;
	}
	
	return stepCount;
}
