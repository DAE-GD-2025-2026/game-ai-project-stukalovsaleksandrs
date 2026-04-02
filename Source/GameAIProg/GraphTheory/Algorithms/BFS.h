#pragma once
#include <vector>

namespace GameAI
{
	class Graph;
	class Node;

	class BFS
	{
	public:
		BFS(Graph* const pGraph);

		std::vector<Node*> FindPath(Node* const pStartNode, Node* const pDestinationNode, UWorld* pWorld) const;

	private:
		Graph* m_pGraph;
	};
}
