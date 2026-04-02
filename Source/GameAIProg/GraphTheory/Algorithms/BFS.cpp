#include "BFS.h"

#include <cassert>
#include <unordered_map>
#include <queue>
#include <unordered_set>

#include "Shared/Graph/Graph.h"

using namespace GameAI;

BFS::BFS(Graph* const pGraph)
	: m_pGraph(pGraph)
{
	assert(pGraph && "BFS: Graph must not be nullptr");
}

// DONE_TODO Breath First Search Algorithm searches for a path from the startNode to the destinationNode
std::vector<Node*> BFS::FindPath(Node* const pStartNode, Node* const pDestinationNode, UWorld* pWorld) const
{
	// Initializing the containers
std::queue openList(std::deque{pStartNode});// Nodes to be visited
	std::unordered_set closedList{pStartNode};// Nodes that were already visited
	std::unordered_map<Node*, Node*> childToParent;// Nodes used for path reconstruction

	auto reconstructPath{
		[&childToParent, pStartNode, pDestinationNode]
		{
			std::vector<Node*> path;
			for (Node* pCurrentNode{ pDestinationNode }; pCurrentNode != pStartNode;)
			{
				path.push_back(pCurrentNode);
				pCurrentNode = childToParent.at(pCurrentNode);
			}
			path.push_back(pStartNode);
			std::ranges::reverse(path);
			return path;
		}
	};
	
	// Performing the search 
	while (!openList.empty())
	{
		Node* pParentNode{ openList.front() };
		openList.pop();

		if (pParentNode == pDestinationNode) return reconstructPath();

		for (auto* pConnection : m_pGraph->FindConnectionsFrom(pParentNode->GetId()))
		{
			Node* pChildNode{ m_pGraph->GetNode(pConnection->GetToId()).get() };
			if (closedList.contains(pChildNode)) continue;
			closedList.insert(pChildNode);
			childToParent[pChildNode] = pParentNode;
			openList.push(pChildNode);
		}
	}

	// Debug rendering(beware, rn only works when path not found)
	// ENGINE_API void DrawDebugBox(const UWorld* InWorld, FVector const& Center, FVector const& Extent, FColor const& Color, bool bPersistentLines = false, float LifeTime=-1.f, uint8 DepthPriority = 0, float Thickness = 0.f);
	/** Draw a debug box with rotation */
	//ENGINE_API void DrawDebugBox(const UWorld* InWorld, FVector const& Center, FVector const& Extent, const FQuat& Rotation, FColor const& Color, bool bPersistentLines = false, float LifeTime=-1.f, uint8 DepthPriority = 0, float Thickness = 0.f);
	// for (auto* pNode : closedList)
	// {
	// 	DrawDebugBox(pWorld, pNode->GetPosition(), pNode->)
	// }
	
	UE_LOG(LogTemp, Display, TEXT("No path found"));
	return {};// No path found
}