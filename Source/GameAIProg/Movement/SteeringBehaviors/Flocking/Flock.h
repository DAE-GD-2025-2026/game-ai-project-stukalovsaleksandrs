#pragma once

// Toggle this define to enable/disable spatial partitioning
// #define GAMEAI_USE_SPACE_PARTITIONING

#include "FlockingSteeringBehaviors.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"
#include "Movement/SteeringBehaviors/SteeringHelpers.h"
#include "Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
#include <memory>
#include "imgui.h"
#ifdef GAMEAI_USE_SPACE_PARTITIONING
#include "../SpacePartitioning/SpacePartitioning.h"
#endif

class Flock final
{
public:
	Flock(
		UWorld* pWorld,
		TSubclassOf<ASteeringAgent> AgentClass,
		int FlockSize = 10, 
		float WorldSize = 100.f, 
		ASteeringAgent* const pAgentToEvade = nullptr, 
		bool bTrimWorld = false);

	~Flock();

	void Tick(float DeltaTime);
	void RenderDebug();
	void ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize);

#ifdef GAMEAI_USE_SPACE_PARTITIONING
	//const TArray<ASteeringAgent*>& GetNeighbors() const { return pPartitionedSpace->GetNeighbors(); }
	//int GetNrOfNeighbors() const { return pPartitionedSpace->GetNrOfNeighbors(); }
#else // No space partitioning
	/**
	 * @def Populates the m_Neighbors with the
	 * neighbors of the input agent
	 */
	void RegisterNeighbors(ASteeringAgent const* Agent);
	int GetNeighborCount() const { return m_NeighborCount; }
	const TArray<ASteeringAgent*>& GetNeighbors() const { return m_Neighbors; }
#endif // USE_SPACE_PARTITIONING

	/**
	 * @def Returns the arithmetical average
	 * between locations of all the given agent's
	 * neighbors
	 */
	FVector2D GetAverageNeighborLocation() const;
	/**
	* @def Returns the arithmetical average
	 * between velocities of all the given agent's
	 * neighbors
	 */
	FVector2D GetAverageNeighborVelocity() const;

	void SetTarget_Seek(FSteeringParams const & Target);

	float GetNeighborhoodRadius() const
	{
		return m_NeighborhoodRadius;
	}

private:
	// For debug rendering purposes
	UWorld* m_pWorld{};
	
	int m_FlockSize{};
	// All the boids in the flock
	TArray<ASteeringAgent*> m_Agents{};
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	//std::unique_ptr<CellSpace> pPartitionedSpace{};
	//int NrOfCellsX{ 10 };
	//TArray<FVector2D> OldPositions{};
#else // No space partitioning
	// Array re-used between different agents to keep track of their neighbors
	TArray<ASteeringAgent*> m_Neighbors{};
#endif // USE_SPACE_PARTITIONING

	// Radius of the furthest distance between boids for them to get considered neighbors.
	float m_NeighborhoodRadius{ 200.f };
	int m_NeighborCount{};

	ASteeringAgent* m_pAgentToEvade{};
	
	//Steering Behaviors
	std::unique_ptr<Separation> pSeparationBehavior{};
	std::unique_ptr<Cohesion> pCohesionBehavior{};
	std::unique_ptr<VelocityMatch> pVelMatchBehavior{};
	std::unique_ptr<Seek> pSeekBehavior{};
	std::unique_ptr<Wander> pWanderBehavior{};
	std::unique_ptr<Evade> pEvadeBehavior{};
	std::unique_ptr<FBlendedSteering> m_pBlendedSteering{};
	std::unique_ptr<FPrioritySteering> m_pPrioritySteering{};

	// UI and rendering
	bool m_DebugRenderSteering{};
	bool m_DebugRenderNeighborhood{true};
	bool m_DebugRenderPartitions{true};

	void RenderNeighborhood();
};
