#include "AStar.h"

#include <unordered_set>
#include <list>

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
	: m_pGraph(pGraph)
	, HeuristicFunction(hFunction)
{
}

std::vector<Node*>AStar::FindPath(Node* const pStartNode, Node* const pGoalNode)
{
	// Kickstarting the loop
	// TODO: Use a binary tree with sorting upon insertion
	std::list openList{ NodeRecord{
		.pNode = pStartNode,
		.estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode)
	}}
	, closedList;

	// The While Loop
	std::list<NodeRecord>::iterator pCurrentNodeRecord{};
	while (!openList.empty())
	{
		// 2.A Taking the node with the lowest F-Cost
		pCurrentNodeRecord = std::min_element(openList.begin(), openList.end());

		// 2.B. Returning if found a goal node
		if (pCurrentNodeRecord->pNode == pGoalNode) break;

		// 2.C. Looping over the neighbors
		for (auto* pNeighborConnection : m_pGraph->FindConnectionsFrom(pCurrentNodeRecord->pNode->GetId()))
		{
			// Getting the node this connection points to
			Node* pNeighborNode{ m_pGraph->GetNode(pNeighborConnection->GetFromId()).get() };
			// Calculating the cost for this neighbor
			float costSoFar{ pCurrentNodeRecord->costSoFar + pNeighborConnection->GetWeight() };

			// 2.D./2.E. Removing the node from open/closed lists if it is more expensive there
			auto removeIfMoreExpensive = [costSoFar](Node* pNode, std::list<NodeRecord>& list) -> bool
			{
				if (auto pNeighborExistingRecord{ std::find_if(list.begin(), list.end(),
					[=](NodeRecord const& nodeRecord)
					{
						return nodeRecord.pNode == pNode;
					})}; pNeighborExistingRecord != list.end())
				{
					if (pNeighborExistingRecord->costSoFar < costSoFar)
					{
						return true;// Next connection
					}
					list.erase(pNeighborExistingRecord);// It will be replaced
				}
				return false;
			};

			// If the existing node is cheaper or does not exist, then continue with the next connection
			if (!removeIfMoreExpensive(pNeighborNode, closedList) || !removeIfMoreExpensive(pNeighborNode, openList))
			{
				break;
			}
			
			// 2.F. Adding the new record for this neighbor to the open list
			openList.emplace_back(
				.pNode = pNeighborNode,
				.pConnection = pNeighborConnection,
				.costSoFar = costSoFar,
				.estimatedTotalCost = costSoFar + GetHeuristicCost(pNeighborNode, pGoalNode)
			);
		}
		
		// 2.G. Removing the currentNodeRecord from the openList and adding it to closedList
		openList.erase(pCurrentNodeRecord);
	}

	// Backtracking
	std::vector<Node*> path{};
	for(NodeRecord& currentNodeRecord{ *pCurrentNodeRecord };
		currentNodeRecord.pNode != pStartNode;)
	{
		path.push_back(currentNodeRecord.pNode);
		
		// Moving on to the previous node in path
		currentNodeRecord = *std::ranges::find_if(closedList, [=](NodeRecord const& nodeRecord)
		{
			return currentNodeRecord.pConnection->GetFromId() == nodeRecord.pNode->GetId(); 
		});
	}
	path.push_back(pStartNode);
	std::ranges::reverse(path);
	return std::move(path);
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
	FVector2D toDestination = m_pGraph->GetNode(pEndNode->GetId())->GetPosition() - m_pGraph->GetNode(pStartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}
