#pragma once

// Toggle this define to enable/disable spatial partitioning
#define GAMEAI_USE_SPACE_PARTITIONING

#include "FlockingSteeringBehaviors.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"
#include "Movement/SteeringBehaviors/SteeringHelpers.h"
#include "Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
#include <memory>
#include "imgui.h"
#include <string_view>
#include <optional>

#include "Shared/WorldTrimVolume.h"

#ifdef GAMEAI_USE_SPACE_PARTITIONING
#include "../SpacePartitioning/SpacePartitioning.h"
#endif

class FFlock final
{
public:
	FFlock(
		UWorld* InWorld,
		TSubclassOf<ASteeringAgent> AgentClass,
		int FlockSize = 1, 
		float TrimSideLength = 100.f, 
		ASteeringAgent* const AgentToEvade = nullptr
		);

	~FFlock();

	void Tick(float DeltaTime);
	void RenderDebug();
	void ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize, AWorldTrimVolume* TrimWorld);

#ifdef GAMEAI_USE_SPACE_PARTITIONING
	void RegisterNeighbors(ASteeringAgent const& Agent);
	const TArray<ASteeringAgent*>& GetNeighbors() const { return CellSpace->GetNeighbors(); }
	int GetNeighborCount() const { return CellSpace->GetNeighborCount(); }
#else // No space partitioning
	/**
	 * @def Populates the m_Neighbors with the
	 * neighbors of the input agent
	 */
	void RegisterNeighbors(ASteeringAgent const& Agent);
	int GetNeighborCount() const { return NeighborCount; }
	const TArray<ASteeringAgent*>& GetNeighbors() const { return FlockNeighborMemoryPool; }
#endif // USE_SPACE_PARTITIONING

	/**
	 * @def Returns the arithmetical average
	 * between locations of all the given agent's
	 * neighbors
	 */
	std::optional<FVector2D> GetAverageNeighborLocation() const;
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
	UWorld* World{};
	
	int FlockSize{};
	// All the boids in the flock
	TArray<ASteeringAgent*> Agents{};
	
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	std::unique_ptr<FCellSpace>	CellSpace{};
	TArray<FVector2D> OldLocations{};
#else // No space partitioning
	// Array re-used between different agents to keep track of their neighbors
	TArray<ASteeringAgent*> FlockNeighborMemoryPool{};
#endif // USE_SPACE_PARTITIONING

	// Radius of the furthest distance between boids for them to get considered neighbors.
	float NeighborhoodRadius{ 300.f };
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
				{ SeparationBehavior.get(), 0.71f },
				{ VelMatchBehavior.get(), 0.59f },
				{ CohesionBehavior.get(), 0.92f },
				{ WanderBehavior.get(), 0.33f },
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
	bool DebugRenderSteering{true}
		, DebugRenderNeighborhood{ true }
		, DebugRenderPartitions{ true };
	
	void RenderNeighborhood(ASteeringAgent const& Agent);
	void DrawBehaviorSliders() const;
	void DrawBehaviorSlider(std::string_view Name, unsigned int Index) const;
};
