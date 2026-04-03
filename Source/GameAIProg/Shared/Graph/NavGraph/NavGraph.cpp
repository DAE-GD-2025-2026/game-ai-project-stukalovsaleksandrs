#include "NavGraph.h"

#include <cassert>

#include "NavGraphNode.h"

GameAI::NavGraph::NavGraph(std::unique_ptr<TriPolygon> && NavPoly)
    : Graph{false}
    , m_pNavPolygon{std::move(NavPoly)}
{
    CreateNavigationGraph();
}

GameAI::NavGraph::NavGraph(const NavGraph& Other)
    : Graph(false)
{
    Nodes.reserve(Other.Nodes.size());
    for (std::unique_ptr<Node> const & OtherNode : Other.Nodes)
    {
        Nodes.push_back(std::make_unique<NavGraphNode>(*static_cast<NavGraphNode*>(OtherNode.get())));
    }
        
    Connections.reserve(Other.Connections.size());
    for (std::unique_ptr<Connection> const & OtherConnection : Other.Connections)
    {
        Connections.push_back(std::make_unique<Connection>(*OtherConnection.get()));
    }
}

std::unique_ptr<GameAI::NavGraph> GameAI::NavGraph::Clone() const
{
    return std::make_unique<NavGraph>(*this);
}

int GameAI::NavGraph::GetNodeIdFromEdgeIndex(int EdgeIdx) const
{
    if (EdgeIdx >= 0)
    {
        for (auto const & pNode : Nodes)
        {
            if (reinterpret_cast<NavGraphNode*>(pNode.get())->GetEdgeIdx() == EdgeIdx)
            {
                return pNode->GetId();
            }
        }
    }
    
    return Graphs::InvalidNodeId;
}

// Creates the nodes and connections from the m_pNavPolygons
void GameAI::NavGraph::CreateNavigationGraph()
{
    //1. Go over all the edges of the navigation mesh and create nodes
    for (TriPolygon::Edge const& edge : m_pNavPolygon->GetEdges())
    {
        auto const edgeIdx{ m_pNavPolygon->FindEdgeIndex(edge) };
        assert(edgeIdx.has_value() && "CreateNavigationGraph: Edge is found in GetEdges() but not in FindEdgeIndex()");
        if (IsEdgeShared(edgeIdx.value()))
        {
            // Edge shared -> add node
            FVector2D const location{
                0.5f * (edge.GetP1(*m_pNavPolygon.get()) + edge.GetP2(*m_pNavPolygon.get()))
            };
            AddNode(std::make_unique<Node>(location));
        }
    }
    
    //2. Create connections now that every node is created
        //2 valid nodes -> 1 connection
        //3 valid nodes -> 3 connections
    size_t constexpr maxNodeCountPerTriangle{ 3 };
    std::array<int, maxNodeCountPerTriangle> nodeIdPool;
    size_t nodeCount{};
    for (auto const& triangle : m_pNavPolygon->GetTriangles())
    {
        // Populating node ids
        for (TriPolygon::Edge const& edge : triangle.GetEdges())
        {
            auto const edgeIdx{ m_pNavPolygon->FindEdgeIndex(edge) };
            assert(edgeIdx.has_value() && "CreateNavigationGraph: Edge is found in GetEdges() but not in FindEdgeIndex()");
            auto const nodeId{ GetNodeIdFromEdgeIndex(edgeIdx.value()) };
            assert(nodeId != Graphs::InvalidNodeId);
            nodeIdPool[nodeCount++] = nodeId;
        }
        
        // Adding the connections between all the nodes
        switch (nodeCount)
        {
        case 2: 
            AddConnection(nodeIdPool[0], nodeIdPool[1]);
            break;
        case 3:
            AddConnection(nodeIdPool[0], nodeIdPool[1]);
            AddConnection(nodeIdPool[1], nodeIdPool[2]);
            AddConnection(nodeIdPool[2], nodeIdPool[0]);
            break;
        default:
            assert(false && "CreateNavigationGraph: Invalid node count");
        }
        
        nodeCount = 0;
    }
    
    //3. Set the connections cost to the actual distance
    for (auto const& pConnection : Connections)
    {
        Node const &nodeFrom{ *GetNode(pConnection->GetFromId()) }, &nodeTo{ *GetNode(pConnection->GetToId()) };
        double const distance{ std::abs((nodeFrom.GetPosition() - nodeTo.GetPosition()).Length()) };
        pConnection->SetWeight(distance);
    }
}

[[nodiscard]] bool GameAI::NavGraph::IsEdgeShared(uint32_t const edgeIdx) const
{
    size_t triangleCount{};
    for (auto& triangle : m_pNavPolygon->GetTriangles())
    {
        auto edges{ triangle.GetEdges() };
        for (auto& edge : edges)
        {
            if (m_pNavPolygon->FindEdgeIndex(edge) == edgeIdx) ++triangleCount;
        }
    }
    
    assert(triangleCount > 2 && "NavMesh contains a non-manifold");
    
    return triangleCount > 1;
}
