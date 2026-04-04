#include "NavGraphPathfinding.h"

#include <cassert>

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos,
    NavGraph* const pNavGraph, std::vector<FVector2D>& debugNodePositions, std::vector<NavLine>& debugPortals) 
{
    //Get the start and endTriangle
    // How to get a triangle from a point?
    auto& navPolygon{ *pNavGraph->GetNavPolygon() };
    auto const pStartTriangle{ navPolygon.GetTriangleAtPosition(startPos, true) },
        pEndTriangle{ navPolygon.GetTriangleAtPosition(endPos, true) };

    // Points not in triangles -> no path
    // NOTE: De Morgan's law: !A || !B = !(A && B)
    if (!(pStartTriangle && pEndTriangle)) return {};
    // Same triangle -> Moving directly
    if (pStartTriangle == pEndTriangle) return {startPos, endPos};
    
    //We have valid start/end triangles, and they are not the same
    //=> Start looking for a path
    //Copy the graph
    auto pNavGraphClone{ pNavGraph->Clone() };
    
    //Create Extra node for the Start and End nodes
    auto addNode{
        [&](FVector2D const& fromNodePos, TriPolygon::Triangle const& fromNodeTriangle)
        {
            // Adding node
            int const fromNodeId{ pNavGraphClone->AddNode(std::make_unique<Node>(fromNodePos)) };
            assert(fromNodeId >= 0);
            // Adding connections
            for (auto const& edge : fromNodeTriangle.GetEdges())
            {
                // 1. Getting a node from the edge
                // 1.1. Getting edge idx
                auto const edgeIdx { navPolygon.FindEdgeIndex(edge) };
                check(edgeIdx.has_value());
                // 1.2. Getting node from the edge idx
                int const toNodeId{ pNavGraphClone->GetNodeIdFromEdgeIndex(edgeIdx.value()) };
                if(toNodeId < 0) continue;// Not adding connections to edges with no nodes
                // 2. Creating a connection and add it to the graph
                auto pConnection{ std::make_unique<Connection>(fromNodeId, toNodeId) };
                pConnection->SetWeight( FVector2D::Distance(
                    pNavGraphClone->GetNode(fromNodeId)->GetPosition(),
                    pNavGraphClone->GetNode(toNodeId)->GetPosition())
                );
                pNavGraphClone->AddConnection(
                    std::move(pConnection)
                );
            }
            
            return fromNodeId;
        }
    };
    int const startNodeId{ addNode(startPos, *pStartTriangle) },
        endNodeId{ addNode(endPos, *pEndTriangle) };
    
    //Run A star on the new graph
    AStar astar{ pNavGraphClone.get(), HeuristicFunctions::Euclidean };
    auto nodePath{
        astar.FindPath(
            pNavGraphClone->GetNode(startNodeId).get(),
            pNavGraphClone->GetNode(endNodeId).get())
    };
    
    //Create the path to return
    std::vector<FVector2D> pointPath;
    pointPath.reserve(nodePath.size());
    std::ranges::transform(nodePath, std::back_inserter(pointPath),
                           [](auto const& node) { return node->GetPosition(); });
    
    //Debug Visualisation
    // Extra: Run optimiser on new graph (First check if everything works without SSFA!)
    debugPortals = SSFA::FindPortals(nodePath, *pNavGraph->GetNavPolygon());
    pointPath = SSFA::OptimizePortals(debugPortals, *pNavGraph->GetNavPolygon());
    
    return pointPath;
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph)
{
    std::vector<FVector2D> debugNodePositions{};
    std::vector<NavLine> debugPortals{};

    return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}