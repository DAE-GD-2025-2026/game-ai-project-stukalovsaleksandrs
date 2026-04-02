#include "AStar.h"

#include <cassert>
#include <unordered_set>
#include <list>

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
    : m_pGraph(pGraph)
    , HeuristicFunction(hFunction)
{
}

std::vector<Node*> AStar::FindPath(Node* const pStartNode, Node* const pGoalNode)
{
    // Kickstarting the loop  
    // TODO: Use a binary tree with sorting upon insertion  
    std::list openList{ NodeRecord{
        .pNode = pStartNode,
        .pConnection = nullptr,
        .costSoFar = 0.0f,
        .estimatedTotalCost = GetHeuristicCost(pStartNode, pGoalNode)
    }};
    std::list<NodeRecord> closedList{};
    
    // The While Loop  
    NodeRecord currentNodeRecord;
    while (!openList.empty())
    {
        // 2.A Taking the node with the lowest F-Cost
        currentNodeRecord = *std::min_element(openList.begin(), openList.end());

        // 2.B. Returning if found a goal node
        if (currentNodeRecord.pNode == pGoalNode) break;

        // 2.C. Looping over the neighbors
        for (auto* pConnectionToNeighbor : m_pGraph->FindConnectionsFrom(currentNodeRecord.pNode->GetId()))
        {
            float const costSoFar{currentNodeRecord.costSoFar + pConnectionToNeighbor->GetWeight()};
            
            // 2.D./2.E. Removing the node from open/closed lists if it is more expensive there
            // Returns if this neighbor should be skipped or not
            auto tryErasing{ [pConnectionToNeighbor, costSoFar](std::list<NodeRecord>& list){
                auto const it = std::ranges::find_if(list, 
                    [&](const NodeRecord& record) { 
                        return record.pNode->GetId() == pConnectionToNeighbor->GetToId(); 
                    });
            
                if (it != list.end())
                {
                    if (costSoFar >= it->costSoFar)
                        return true;  // Skipping this neighbor
                    list.erase(it);
                }
                
                return false;
            }};
            if (tryErasing(closedList) || tryErasing(openList)) continue;

            // 2.F. Adding the new record for this neighbor to the open list
            Node* const pNeighborNode{ m_pGraph->GetNode(pConnectionToNeighbor->GetToId()).get() };
            openList.emplace_back(NodeRecord{  
                .pNode = pNeighborNode,
                .pConnection = pConnectionToNeighbor,  
                .costSoFar = costSoFar,  
                .estimatedTotalCost = costSoFar + GetHeuristicCost(pNeighborNode, pGoalNode)}  
            );  
        }
        
        // 2.G. Removing the currentNodeRecord from the openList and adding it to closedList  
        openList.erase(std::ranges::find(openList, currentNodeRecord));
        closedList.emplace_back(currentNodeRecord);
    }

    // Backtracking
    if (currentNodeRecord.pNode != pGoalNode)
    {
        return {pStartNode};  // No path found
    }
    
    std::vector<Node*> path{};
    while (currentNodeRecord.pNode != pStartNode)
    {
        path.emplace_back(currentNodeRecord.pNode);
    
        auto it = std::ranges::find_if(closedList, [&currentNodeRecord](auto const& node) {
            return node.pNode->GetId() == currentNodeRecord.pConnection->GetFromId();
        });
    
        if (it != closedList.end())
            currentNodeRecord = *it;
    }
    path.emplace_back(pStartNode);
    std::ranges::reverse(path);
    return path;
}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
    FVector2D toDestination = m_pGraph->GetNode(pEndNode->GetId())->GetPosition() - m_pGraph->GetNode(pStartNode->GetId())->GetPosition();
    return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}
