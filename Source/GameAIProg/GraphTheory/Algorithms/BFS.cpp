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
std::vector<Node*> BFS::FindPath(Node* const pStartNode, Node* const pDestinationNode) const
{
	// Initializing the containers
	std::queue openList(std::deque{pStartNode});// Nodes to be visited
	std::unordered_set closedList{pStartNode};// Nodes that were already visited
	std::unordered_map<Node*, Node*> childToParent;// Nodes used for path reconstruction

	auto reconstructPath{
		[=]
		{
			std::vector<Node*> path;
			for (Node* pCurrentNode{ pDestinationNode }; pCurrentNode != pStartNode;)
			{
				path.push_back(pCurrentNode);
				pCurrentNode = childToParent.at(pCurrentNode);
			}
			path.push_back(pStartNode);
			std::ranges::reverse(path);
			return std::move(path);
		}
	};
	
	// Performing the search 
	while (!openList.empty())
	{
		Node* pParentNode{ openList.front() };
		openList.pop();

		if (pParentNode == pDestinationNode) return reconstructPath();

		for (auto* pConnection : m_pGraph->FindConnectionsFrom(pStartNode->GetId()))
		{
			Node* pChildNode{ m_pGraph->GetNode(pConnection->GetToId()).get() };
			if (closedList.contains(pChildNode)) continue;
			closedList.insert(pChildNode);
			childToParent[pChildNode] = pParentNode;
			openList.push(pChildNode);
		}
	}

	return {};// No path found
}
