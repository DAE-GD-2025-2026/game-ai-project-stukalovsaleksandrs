#pragma once
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"

namespace GameAI
{
    class NavGraph : public Graph
    {
    public:
        explicit NavGraph(std::unique_ptr<TriPolygon> && NavPoly);
        NavGraph(const NavGraph& Other);
        
        std::unique_ptr<NavGraph> Clone() const;
        
        TriPolygon const * GetNavPolygon() const {return m_pNavPolygon.get();}
        int GetNodeIdFromEdgeIndex(int EdgeIdx) const;
        
    private:
        std::unique_ptr<TriPolygon> m_pNavPolygon;

        void CreateNavigationGraph();
        
        // Returns whether an edge is shared by 2 triangles or only by 1
        [[nodiscard]] bool IsEdgeShared(uint32_t edgeIdx) const;
    };
}
