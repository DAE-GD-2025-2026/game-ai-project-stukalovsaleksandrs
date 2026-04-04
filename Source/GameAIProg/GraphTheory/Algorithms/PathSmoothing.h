#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "Algo/Compare.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

namespace GameAI
{
    class SSFA final
{
public:
    //=== SSFA Functions ===
    //--- References ---
    //http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
    //https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
    static std::vector<NavLine> FindPortals(std::vector<Node*> const & path, TriPolygon const & navPoly)
    {
        //Container
        // Are the lines from one portal node to another or how does it work?
        // So, the first portal is degenerate, it is a starting point.
        // Everything else is 
        std::vector<NavLine> portals;

        if (path.empty())
            return portals;
        
        portals.reserve(path.size());
        
        //For each node received, get it's corresponding line
        auto& edges{ navPoly.GetEdges() };
        NavGraphNode const* pPreviousNavNode{};
        for (auto const& node: path)
        {
            // Acquiring the edge
            NavGraphNode const * const pNavNode{static_cast<NavGraphNode*>(node)};
            auto const edgeIdx{ pNavNode->GetEdgeIdx() };
            check(edgeIdx != -1);
            auto const edge{ edges.at(edgeIdx) };

            //Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point
            FVector2D p1{edge.GetP1(navPoly)}, p2{edge.GetP2(navPoly)};
            if (pPreviousNavNode)// nullptr for the first one
            {
                FVector2D const pathDirection{ pNavNode->GetPosition() - pPreviousNavNode->GetPosition() },
                    edgeDirection{ p2 - p1 };
                if (FVector2D::CrossProduct(pathDirection, edgeDirection) > 0)
                {
                    Swap(p1, p2);
                }
            }
            pPreviousNavNode = pNavNode;

            //Store portal
            portals.emplace_back( NavLine(
                p1, p2
            ));
        }

        //Add degenerate portal to force end evaluation
        // No need, done in the main loop

        return portals;
    }

    static std::vector<FVector2D> OptimizePortals( std::vector<NavLine> const & portals, TriPolygon const & navPoly)
    {
        std::vector<FVector2D> path;

        if (portals.empty()) return {};
        
        // Adding the first point
        FVector2D apex{ portals.front().P1 };// P1 = P2
        path.push_back(apex);
        //P1 == right point of portal, P2 == left point of portal
        FVector2D rightLeg{ portals.front().P1 - apex }, leftLeg{ portals.front().P2 - apex };
        for (size_t portalIdx{ 1 }; portalIdx < portals.size(); ++portalIdx)// The first portal is already processed
        {
            NavLine const& portal{ portals.at(portalIdx) };
            //--- RIGHT CHECK ---
            //1. See if moving funnel inwards - RIGHT
            FVector2D const newRight{ portal.P1 - apex };
            if (FVector2D::CrossProduct(rightLeg, newRight) <= 0.f)
            {
                //2. See if new line degenerates a line segment - RIGHT
                if (FVector2D::CrossProduct(rightLeg, leftLeg) < 0.f)
                {
                    //Leftleg becomes new apex point
                    apex = leftLeg;
                    path.push_back(apex);
                    //Calculate new legs (if not the end)
                    if (portalIdx < portals.size() - 1)
                    {
                        rightLeg = portals.at(portalIdx).P1 - apex;
                        leftLeg = portals.at(portalIdx).P2 - apex;
                        continue;
                    }
                }
            }

            //--- LEFT CHECK ---
            //1. See if moving funnel inwards - LEFT
            FVector2D const newLeft{ portal.P2 - apex };
            if (FVector2D::CrossProduct(leftLeg, newLeft) <= 0.f)
            {
                //2. See if new line degenerates a line segment - LEFT
                if (FVector2D::CrossProduct(leftLeg, rightLeg) < 0.f)
                {
                    //Rightleg becomes new apex point
                    apex = rightLeg;
                    path.push_back(apex);
                    //Calculate new legs (if not the end)
                    if (portalIdx < portals.size() - 1)
                    {
                        rightLeg = portals.at(portalIdx).P1 - apex;
                        leftLeg = portals.at(portalIdx).P2 - apex;
                        continue;
                    }
                }
            }
        }
        
        // Add last path point
        path.push_back(portals.back().P1);

        return path;
    }
private:
    SSFA() {};
    ~SSFA() {};
};
}
