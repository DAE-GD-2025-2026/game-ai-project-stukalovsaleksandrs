#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Shared/ImGuiHelpers.h"
#include <cstdint>


Flock::Flock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize,
	float WorldSize,
	ASteeringAgent* const pAgentToEvade,
	bool bTrimWorld)
	: pWorld{pWorld}
	, FlockSize{ FlockSize }
	, pAgentToEvade{pAgentToEvade}
{
	// Populating the Agents array
	Agents.SetNum(FlockSize);
	for (auto& Agent : Agents)
	{
		Agent = pWorld->SpawnActor<ASteeringAgent>(
			AgentClass,
			FVector{0, 0, 90},
			FRotator::ZeroRotator
		);
		assert(Agent);
		Agent->SetSteeringBehavior(BlendedSteering.get());
		// Disabling the ticking to prevent the agent from
		// running its own steering behavior independently.
		// We need sequential update for consistency.
		Agent->SetActorTickEnabled(false);
	}

	// Initializing the flock and the memory pool of neighbors
	// NOTE: Each boid can have at max all boids in the flock as neighbors,
	// but it will never count itself as a neighbor.
	m_Neighbors.SetNum(FlockSize - 1);
}

Flock::~Flock()
{
	Agents.Empty();
	m_Neighbors.Empty();
}

void Flock::Tick(float const DeltaTime)
{
	for (ASteeringAgent* pAgent : Agents)
	{
		// Populating the neighbor memory pool
		RegisterNeighbors(pAgent);
		// Updating the agent using the neighbors in the memory pool
		pAgent->Tick(DeltaTime);
	}
}

void Flock::RenderDebug()
{
 // TODO: Render all the agents in the flock
 for (auto Agent : Agents)
 {
 	// Debug rendering
 }
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	//UI
	{
		//Setup
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

		if (ImGui::Checkbox("Debug Rendering", &DebugRenderSteering))
		{
			
		}

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();

  // TODO: implement ImGUI sliders for steering behavior weights here
		//End
		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent const * const Agent)
{
	NeighborCount = 0;
	// Filling the memory pool with the neighbors for the currently evaluated agent
	for (auto const& OtherAgent : Agents)
	{
		if (Agent == OtherAgent) continue;
		if( (OtherAgent->GetActorLocation() - Agent->GetActorLocation()).Length() < NeighborhoodRadius )
		{
			assert(NeighborCount < m_Neighbors.Num());
			m_Neighbors[NeighborCount++] = OtherAgent; 
		}
	}
}

#endif

FVector2D Flock::GetAverageNeighborLocation() const
{
	FVector2D AverageLocation{};
	for (int32_t NeighborIdx{}; NeighborIdx < NeighborCount; ++NeighborIdx)
	{
		AverageLocation += m_Neighbors[NeighborIdx]->GetLocation();
	}
	AverageLocation /= static_cast<float>(NeighborCount);
	return AverageLocation;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D AverageVelocity{};
	for (int32_t NeighborIdx{}; NeighborIdx < NeighborCount; ++NeighborIdx)
	{
		AverageVelocity += m_Neighbors[NeighborIdx]->GetLocation();
	}
	AverageVelocity /= static_cast<float>(NeighborCount);
	return AverageVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
	SeekBehavior->SetTarget(Target);
}

