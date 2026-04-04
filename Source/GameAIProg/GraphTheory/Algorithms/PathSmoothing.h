#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"

namespace GameAI
{
	class SSFA final
	{
	public:

		static std::vector<NavLine> FindPortals(std::vector<Node*> const& path, TriPolygon const& navPoly)
		{
			//Container
			std::vector<NavLine> portals;

			if (path.empty())
				return portals;

			// Adding the start portal
			FVector2D const startPos = path.front()->GetPosition();
			portals.emplace_back(NavLine{
				.P1 = startPos,
				.P2 = startPos
			});

			// Adding all the non-degenerate portals
			// NOTE: Using a raw loop to be able to get access to the next element
			for (int pathNodeIdx{}; pathNodeIdx < path.size(); ++pathNodeIdx)
			{
				// Getting the edge the node is on
				Node* const pNode{ path.at(pathNodeIdx) };
				int const edgeIdx{ static_cast<NavGraphNode*>(pNode)->GetEdgeIdx() };
				if (edgeIdx == -1) continue;
				TriPolygon::Edge edge{ navPoly.GetEdges().at(edgeIdx) };
				
				//Redetermine it's "orientation" based on the required path (left-right vs right-left) - p1 should be right point
				FVector2D p1{ FVector2D(edge.GetP1(navPoly)) },
					p2{ FVector2D(edge.GetP2(navPoly)) };

				if (pathNodeIdx < path.size() - 1)
				{
					FVector2D const pathDirection{ path[pathNodeIdx + 1]->GetPosition() - pNode->GetPosition()},
						edgeDirection{ p2 - p1 };

					if (FVector2D::CrossProduct(pathDirection, edgeDirection) >= 0)
						Swap(p1, p2);	
				}
				
				//Store portal
				portals.emplace_back(NavLine{
					.P1 = p1,
					.P2 = p2
				});
			}

			//Add degenerate portal to force end evaluation
			FVector2D const endPos = path.back()->GetPosition();
			portals.emplace_back(NavLine{
				.P1 = endPos,
				.P2 = endPos 
			});

			return portals;
		}

static std::vector<FVector2D> OptimizePortals(std::vector<NavLine> const& portals, TriPolygon const& tryPolygon)
{
    if (portals.empty()) return {};

    FVector2D apex{ portals[0].P1 }, leftLeg{}, rightLeg{};
    std::vector<FVector2D> path{};
	path.reserve(portals.size());
    path.push_back(apex);

	// NOTE: Starting from 1, bc the first portal is already processed
	size_t apexIdx{}, leftLegIdx{};
    for (size_t portalIdx{ 1 }; portalIdx < portals.size(); ++portalIdx)
    {
	    NavLine const& portal{ portals.at(portalIdx) };

    	// TODO: Make a versitile lambda
    	//--- RIGHT CHECK ---
    	//1. See if moving funnel inwards - RIGHT
    	FVector2D const newRightLeg{ portal.P1 - apex };
    	if (FVector2D::CrossProduct(rightLeg, newRightLeg) <= 0.f)
    	{
    		//2. See if new line degenerates a line segment - RIGHT
    		if (FVector2D::CrossProduct(leftLeg, newRightLeg) < 0.f)
    		{
    			//Leftleg becomes new apex point
    			apex += leftLeg;
    			path.push_back(apex);
    			apexIdx = leftLegIdx;
    			//Calculate new legs (if not the end)
    			if (portalIdx < portals.size() - 1)
    			{
    				portalIdx = apexIdx;
    				rightLeg = portals.at(apexIdx).P1 - apex;
    				leftLeg = portals.at(apexIdx).P2 - apex;
    			}
    			continue;
    		}
    		rightLeg = newRightLeg;
    	}

    	//--- LEFT CHECK ---
    	//1. See if moving funnel inwards - LEFT
    	FVector2D const newLeftLeg{ portal.P2 - apex };
    	if (FVector2D::CrossProduct(leftLeg, newLeftLeg) >= 0.f)
    	{
    		//2. See if new line degenerates a line segment - LEFT
    		if (FVector2D::CrossProduct(leftLeg, rightLeg) < 0.f)
    		{
    			//Rightleg becomes new apex point
    			apex += rightLeg;
    			path.push_back(apex);
    			apexIdx = leftLegIdx;
    			//Calculate new legs (if not the end)
    			if (portalIdx < portals.size() - 1)
    			{
    				portalIdx = apexIdx;
    				leftLegIdx = apexIdx;
    				rightLeg = portals.at(apexIdx).P1 - apex;
    				leftLeg = portals.at(apexIdx).P2 - apex;
    			}
    			continue;
    		}
    		leftLeg = newLeftLeg;
    		leftLegIdx = portalIdx;
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
