#pragma once

// Toggle this define to enable/disable spatial partitioning
// #define GAMEAI_USE_SPACE_PARTITIONING

#include "FlockingSteeringBehaviors.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"
#include "Movement/SteeringBehaviors/SteeringHelpers.h"
#include "Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
#include <memory>
#include "imgui.h"
#include <string_view>
#include <unordered_map>

#include "Shared/WorldTrimVolume.h"

#ifdef GAMEAI_USE_SPACE_PARTITIONING
#include "../SpacePartitioning/SpacePartitioning.h"
#endif

class FCellSpace;

class FFlock final
{
public:
	FFlock(
		UWorld* World,
		TSubclassOf<ASteeringAgent> AgentClass,
		int FlockSize = 5, 
		float TrimSideLength = 100.f, 
		ASteeringAgent* const AgentToEvade = nullptr
		);

	~FFlock();

	void Tick(float DeltaTime);
	void RenderDebug();
	void ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize, AWorldTrimVolume* TrimWorld);

#ifdef GAMEAI_USE_SPACE_PARTITIONING
	//const TArray<ASteeringAgent*>& GetNeighbors() const { return pPartitionedSpace->GetNeighbors(); }
	//int GetNrOfNeighbors() const { return pPartitionedSpace->GetNrOfNeighbors(); }
#else // No space partitioning
	/**
	 * @def Populates the m_Neighbors with the
	 * neighbors of the input agent
	 */
	void RegisterNeighbors(ASteeringAgent const* Agent);
	int GetNeighborCount() const { return NeighborCount; }
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
		return NeighborhoodRadius;
	}

private:
	// For debug rendering purposes
	UWorld* pWorld{};
	
	int FlockSize{};
	// All the boids in the flock
	TArray<ASteeringAgent*> Agents{};
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	//std::unique_ptr<CellSpace> pPartitionedSpace{};
	//int NrOfCellsX{ 10 };
	//TArray<FVector2D> OldPositions{};
#else // No space partitioning
	// Array re-used between different agents to keep track of their neighbors
	TArray<ASteeringAgent*> m_Neighbors{};
#endif // USE_SPACE_PARTITIONING

	// Radius of the furthest distance between boids for them to get considered neighbors.
	float NeighborhoodRadius{ 200.f };
	int NeighborCount{};

	ASteeringAgent* AgentToEvade{};
	
	//Steering Behaviors
	std::unique_ptr<Separation> SeparationBehavior{
		std::make_unique<Separation>(this)
	};
	std::unique_ptr<Cohesion> CohesionBehavior{
		std::make_unique<Cohesion>(this)
	};
	std::unique_ptr<VelocityMatch> VelMatchBehavior{
		std::make_unique<VelocityMatch>(this)
	};
	std::unique_ptr<Wander> WanderBehavior{
		std::make_unique<Wander>()
	};
	std::unique_ptr<Seek> SeekBehavior{
		std::make_unique<Seek>()
	};
	std::unique_ptr<FBlendedSteering> BlendedBehavior{
		std::make_unique<FBlendedSteering>(
			std::vector<FBlendedSteering::FWeightedBehavior>{
				{ SeparationBehavior.get(), 1.f },
				{ VelMatchBehavior.get(), 0.28f },
				{ CohesionBehavior.get(), 1.f },
				{ WanderBehavior.get(), 0.54f },
        	}
		)
	};
	std::unique_ptr<Evade> EvadeBehavior{
		std::make_unique<Evade>(350.f)
	};
	std::unique_ptr<FPrioritySteering> PriorityBehavior{
		std::make_unique<FPrioritySteering>(
			std::vector<ISteeringBehavior*>{
				EvadeBehavior.get(), BlendedBehavior.get()
		})
	};

	// UI and rendering
	bool DebugRenderSteering{}
		, DebugRenderNeighborhood{ true }
		, DebugRenderPartitions{ true }
		, UseSpatialPartitioning{ true };

	std::unique_ptr<FCellSpace>	CellSpace{};
	
	void RenderNeighborhood();
	void DrawBehaviorSliders() const;
	void DrawBehaviorSlider(std::string_view Name, unsigned int Index) const;
};
