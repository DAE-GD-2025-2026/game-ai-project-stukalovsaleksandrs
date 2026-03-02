#include "Flock.h"
#include "Shared/ImGuiHelpers.h"
#include <cstdint>

FFlock::FFlock(
	UWorld* World,
	TSubclassOf<ASteeringAgent> AgentClass,
	int FlockSize,
	float WorldSize,
	ASteeringAgent* const AgentToEvade,
	bool bTrimWorld)
	: pWorld{World}
	, FlockSize{ FlockSize }
	, pAgentToEvade{AgentToEvade}
{
	// Populating the Agents array
	Agents.SetNum(FlockSize);
	for (auto& Agent : Agents)
	{
		Agent = World->SpawnActor<ASteeringAgent>(
			AgentClass,
			FVector{0, 0, 90},
			FRotator::ZeroRotator
		);
		assert(Agent);
		Agent->SetSteeringBehavior(BlendedBehavior.get());
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

FFlock::~FFlock()
{
	Agents.Empty();
	m_Neighbors.Empty();
}

void FFlock::Tick(float const DeltaTime)
{
	for (ASteeringAgent* pAgent : Agents)
	{
		// Populating the neighbor memory pool
		RegisterNeighbors(pAgent);
		// Updating the agent using the neighbors in the memory pool
		pAgent->Tick(DeltaTime);
	}
}

void FFlock::RenderDebug()
{
 // TODO: Render all the agents in the flock
 for (auto Agent : Agents)
 {
 	// Debug rendering
 }
}

void FFlock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
{
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

		DrawBehaviorSliders();		
			
		//End
		ImGui::End();
	}
#pragma endregion
}

void FFlock::RenderNeighborhood()
{
 // TODO: Debugrender the neighbors for the first agent in the flock
}

void FFlock::DrawBehaviorSliders() const
{
	DrawBehaviorSlider("Separation", 0);
	// DrawBehaviorSlider("Cohesion", 1);
	// DrawBehaviorSlider("VelMatch", 2);
	DrawBehaviorSlider("Wander", 1);
}

void FFlock::DrawBehaviorSlider(std::string_view const Name, unsigned int const Index) const
{
	assert(Name.length() > 0);
	auto & BehaviorWeight{ BlendedBehavior->GetWeightedBehaviorsRef()[Index].Weight };
	ImGuiHelpers::ImGuiSliderFloatWithSetter(Name.data(),
		BehaviorWeight, 0.f, 1.f,
		[&BehaviorWeight](float const InVal) { BehaviorWeight = InVal; }, "%.2f");
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void FFlock::RegisterNeighbors(ASteeringAgent const * const Agent)
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

FVector2D FFlock::GetAverageNeighborLocation() const
{
	FVector2D AverageLocation{};
	for (int32_t NeighborIdx{}; NeighborIdx < NeighborCount; ++NeighborIdx)
	{
		AverageLocation += m_Neighbors[NeighborIdx]->GetLocation();
	}
	AverageLocation /= static_cast<float>(NeighborCount);
	return AverageLocation;
}

FVector2D FFlock::GetAverageNeighborVelocity() const
{
	FVector2D AverageVelocity{};
	for (int32_t NeighborIdx{}; NeighborIdx < NeighborCount; ++NeighborIdx)
	{
		AverageVelocity += m_Neighbors[NeighborIdx]->GetLocation();
	}
	AverageVelocity /= static_cast<float>(NeighborCount);
	return AverageVelocity;
}

void FFlock::SetTarget_Seek(FSteeringParams const& Target)
{
	SeekBehavior->SetTarget(Target);
}

