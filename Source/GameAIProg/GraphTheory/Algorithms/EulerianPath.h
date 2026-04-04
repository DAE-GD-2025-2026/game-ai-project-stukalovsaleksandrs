#pragma once
#include <stack>
#include <stdexcept>

#include "Shared/Graph/Graph.h"

namespace GameAI
{
    enum class Eulerianity
    {
        notEulerian,
        semiEulerian,
        eulerian,
    };

    class EulerianPath final
    {
    public:
        EulerianPath(Graph* const pGraph);

        Eulerianity GetEulerianity() const;
        std::vector<Node*> FindPath(Eulerianity& outEulerianity) const;

    private:
        void VisitAllNodesDFS(const std::vector<Node*>& pNodes, std::vector<bool>& visited, int startIndex) const;
        [[nodiscard]] bool IsConnected() const;
        [[nodiscard]] bool HasEvenDegree(int nodeId) const;
        [[nodiscard]] bool HasConnections(int const nodeId) const;

        Graph* m_pGraph;
    };

    inline EulerianPath::EulerianPath(Graph* const pGraph)
        : m_pGraph(pGraph)
    {}

    inline Eulerianity EulerianPath::GetEulerianity() const
    {
        // DONE_TODO If the graph is not connected, there can be no Eulerian Trail
        if (!IsConnected()) return Eulerianity::notEulerian;

        // DONE_TODO Count nodes with odd degree 
        auto nodes{ m_pGraph->GetActiveNodes() };
        size_t const nodeCount{ nodes.size() };
        size_t oddDegreeNodeCount{};
        for (auto* pNode : nodes)
        {
            if (!HasEvenDegree(pNode->GetId()))
            {
                ++oddDegreeNodeCount;
            }
        }
        // DONE_TODO A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
        if (oddDegreeNodeCount > 2) return Eulerianity::notEulerian;

        // DONE_TODO A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
        // DONE_TODO An Euler trail can be made, but only starting and ending in these 2 nodes
        if (oddDegreeNodeCount == 2 && nodeCount > 2) return Eulerianity::semiEulerian;

        // DONE_TODO A connected graph with no odd nodes is Eulerian
        // NOTE: Check for whether the graph is connected was already performed
        if (!oddDegreeNodeCount && nodeCount) return Eulerianity::eulerian;
        
        return Eulerianity::notEulerian;
    }

    inline std::vector<Node*> EulerianPath::FindPath(Eulerianity& outEulerianity) const
    {
        assert(!m_pGraph->GetIsDirectional() && "EulerianPath::FindPath: Directional graphs not supported");
        // Get a copy of the graph because this algorithm involves removing edges
        Graph graphCopy = m_pGraph->Clone();
        std::vector<Node*> path = {};
        std::vector<Node*> nodes = graphCopy.GetActiveNodes();
        if (nodes.empty()) return path;
        int currentNodeId{};
        
        // DONE_TODO Check if there can be an Euler path
        outEulerianity = GetEulerianity();
        
        // DONE_TODO Start algorithm loop
        // 1. Starting with an empty stack and an empty path
        std::stack<int> nodeStack;

        // 2. Choosing a starting node(current node)
        switch (outEulerianity)
        {
        case Eulerianity::eulerian:
            // All nodes even degree -> choosing any(using the first one)
            currentNodeId = nodes.at(0)->GetId();
            break;
        case Eulerianity::semiEulerian:
            // 2 nodes with odd degree -> choosing one with the odd degree
            for (auto const * const pNode : nodes)
            {
                if (!HasEvenDegree(pNode->GetId()))
                {
                    currentNodeId = pNode->GetId();
                    break;
                }
            }
            assert(!m_pGraph->HasEvenDegree(currentNodeId));// There must be odd nodes
            break;
        case Eulerianity::notEulerian:
            // DONE_TODO If this graph is not eulerian, return the empty path
            return path;
        default:
            throw std::invalid_argument("Unsupported Eulerianity");
        }
        
        // 3. Looping until the stack is empty and there are no more connections left for the current node
        while (true)
        {
            // 4. If the current node has neighbors:
            auto const connectionsFrom{ graphCopy.FindConnectionsFrom(currentNodeId) }; 
            if (!connectionsFrom.empty())
            {
                // 4.1. Adding the node to stack
                nodeStack.push(currentNodeId);
                // 4.2. Taking any of its neighbors(taking the last one for easier removal)
                Connection* pCurrentConnection{ connectionsFrom.at(connectionsFrom.size() - 1) };
                // 4.3. Setting that neighbor as the current node
                currentNodeId = pCurrentConnection->GetToId();
                // 4.4. Removing the edge between the selected neighbor and that node
                graphCopy.RemoveConnection(pCurrentConnection);
            }
            else
            {
                // 5. Adding the last current node(from the original graph) to the path
                path.push_back(m_pGraph->GetNode(currentNodeId).get());
                
                // We are done if current node has no more connections and there are no elements in stack
                if (nodeStack.empty()) break;
                
                // Unwinding the node stack to add the preceding nodes to the stack
                currentNodeId = nodeStack.top();
                nodeStack.pop();
            }
        }
        
        // 6. The results are in the reverse order, so 
        // they have to be reversed back
        std::ranges::reverse(path);
        return path;
    }

    inline void EulerianPath::VisitAllNodesDFS(const std::vector<Node*>& pNodes, std::vector<bool>& Visited, int const StartIdx ) const
    {
        // It's DFS, if the closest one is visited then so are the further ones
        if (Visited.at(StartIdx)) return;
        
        // DONE_TODO Mark the visited node
        Visited.at(StartIdx) = true;
        
        // DONE_TODO Ask the graph for the connections from that node
        auto const& ConnectionsFrom{ m_pGraph->FindConnectionsFrom(StartIdx) };
        
        // DONE_TODO recursively visit any valid connected nodes that were not visited before
        for (int ConnectionIdx{}; ConnectionIdx < ConnectionsFrom.size(); ++ConnectionIdx)
        {
            auto const * const Connection{ ConnectionsFrom.at(ConnectionIdx) };
            if (!Connection) continue;

            if (int const ToId{ Connection->GetToId() }; !Visited.at(ToId))
            {
                // Current connection is not visited -> recursively visit all the other ones
                VisitAllNodesDFS(pNodes, Visited, Connection->GetToId());
            }
        }
        
        // DONE_TODO Tip: use an index-based for-loop to find the correct index
    }

    inline bool EulerianPath::IsConnected() const
    {
        std::vector<Node*> Nodes = m_pGraph->GetActiveNodes();
        if (Nodes.size() == 0)
            return false;

        // DONE_TODO choose a starting node
        // Getting the first node with at least one connection
        int StartNodeIdx{};
        for ([[maybe_unused]] Node* Node : Nodes)
        {
            if (HasConnections(StartNodeIdx)) break;
            ++StartNodeIdx;
        }
        // Starting node still has no connections -> none of the nodes does, bad coms(
        if (!HasConnections(StartNodeIdx)) return false;
        
        // DONE_TODO start a depth-first-search traversal from the node that has at least one connection
        std::vector<bool> VisitedNodes(Nodes.size());
        VisitAllNodesDFS(Nodes, VisitedNodes, StartNodeIdx);
        
        // DONE_TODO if a node was never visited, this graph is not connected
        // Returning if all nodes were visited
        return std::ranges::any_of(VisitedNodes, [&](bool const bVisited){ return !bVisited; });
    }

    inline bool EulerianPath::HasEvenDegree(int const nodeId) const
    {
        size_t const ConnectionCount{
            m_pGraph->FindConnectionsFrom(nodeId).size() + m_pGraph->FindConnectionsTo(nodeId).size()
        };

        return ConnectionCount % 2 == 0;
    }

    inline bool EulerianPath::HasConnections(int const nodeId) const
    {
        return m_pGraph->FindConnectionsFrom(nodeId).size() > 0 && m_pGraph->FindConnectionsTo(nodeId).size() > 0;
    }
}
