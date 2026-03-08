#include "Flock.h"
#include "../SpacePartitioning/SpacePartitioning.h"
#include "Shared/ImGuiHelpers.h"
#include <cstdint>

#include "Shared/WorldTrimVolume.h"

FFlock::FFlock(
	UWorld* InWorld,
	TSubclassOf<ASteeringAgent> const AgentClass,
	int const FlockSize,
	float const TrimSideLength,
	ASteeringAgent* const AgentToEvade)
	: World{InWorld}
	, FlockSize{ FlockSize }
	, CellSpace{ std::make_unique<FCellSpace>(
		World,
		TrimSideLength,
		TrimSideLength,
		10, 10, FlockSize 
		)
	}
	, AgentToEvade{AgentToEvade}
{
	// Populating the Agents array
	Agents.SetNum(FlockSize);
	// NOTE: Reference is not redundant here
	// Without it, copies will be used instead of the sources
	for (auto& Agent : Agents)
	{
		// If UE refuses to spawn an actor
		unsigned int TryCount{};
		static unsigned constexpr MaxTryCount{ 10 };
		while (Agent == nullptr && TryCount++ < MaxTryCount)
		{
			// Spawning an actor
			float const QuarterTrimSize{ 0.25f * TrimSideLength };
			Agent = World->SpawnActor<ASteeringAgent>(
				AgentClass,
				FVector{
					FMath::RandRange(-QuarterTrimSize, QuarterTrimSize)
					,FMath::RandRange(-QuarterTrimSize, QuarterTrimSize) 
					, 90
				},
				FRotator::ZeroRotator
			);
		}
		OldLocations.Add(Agent->GetLocation());
		CellSpace->AddAgent(Agent);
		Agent->SetSteeringBehavior(PriorityBehavior.get());
		// Disabling the ticking to prevent the agent from
		// running its own steering behavior independently.
		// We need sequential update for consistency.
		Agent->SetActorTickEnabled(false);
	}

	// Validating all agents
	for (auto Agent : Agents){ assert(Agent != nullptr); }

#ifndef GAMEAI_USE_SPACE_PARTITIONING
	// Initializing the flock and the memory pool of neighbors
	// NOTE: Each boid can have at max all boids in the flock as neighbors,
	// but it will never count itself as a neighbor.
	FlockNeighborMemoryPool.SetNum(FlockSize - 1);

	// Saving the old positions
	
#endif
}

FFlock::~FFlock()
{
	Agents.Empty();
#ifndef GAMEAI_USE_SPACE_PARTITIONING
	FlockNeighborMemoryPool.Empty();
#endif
}

void FFlock::Tick(float const DeltaTime)
{
	// Updating all agents
	for (size_t AgentIdx{}; AgentIdx < Agents.Num(); ++AgentIdx)
	{
		// Updating the agent using the neighbors in the memory pool
		Agents[AgentIdx]->Tick(DeltaTime);

#ifdef GAMEAI_USE_SPACE_PARTITIONING
		// Updating agent's cell
		CellSpace->UpdateAgentCell(Agents[AgentIdx], OldLocations[AgentIdx]);
		// NOTE: Updating old location after Agent->Tick(),
		// and not before because Agent->Tick() is not executed 
		// immediately after calling, but at some point after
		OldLocations[AgentIdx] = Agents[AgentIdx]->GetLocation();
#endif
		// Populating the neighbor memory pool
		RegisterNeighbors(*Agents[AgentIdx]);
	}
	// Setting the agent to evade as target
	EvadeBehavior->SetTarget(FTargetData{
		AgentToEvade->GetLocation(),
		AgentToEvade->GetRotation(),
		AgentToEvade->GetLinearVelocity(),
		AgentToEvade->GetAngularVelocity(),
	});
}

void FFlock::RenderDebug()
{
	if (!DebugRenderSteering) return;

	// Steering behavior debug
	FlushPersistentDebugLines(World);
	ASteeringAgent const& DebugAgent{ *Agents[0] };
	BlendedBehavior->DebugDraw(DebugAgent);// Red 
	WanderBehavior->DebugDraw(DebugAgent);// Green + Emerald
	SeparationBehavior->DebugDraw(DebugAgent, NeighborhoodRadius);// Blue
	EvadeBehavior->DebugDraw(DebugAgent);// Purple

	// Neighbor debug
	RenderNeighborhood(DebugAgent);
	
#ifdef GAMEAI_USE_SPACE_PARTITIONING
	// Cell grid
	CellSpace->RenderCells();
#endif
}

void FFlock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize, AWorldTrimVolume* TrimWorld)
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

	ImGui::Checkbox("Trim World", &TrimWorld->bShouldTrimWorld);
	if (TrimWorld->bShouldTrimWorld)
	{
		ImGuiHelpers::ImGuiSliderFloatWithSetter("Trim Size",
			TrimWorld->GetTrimWorldSize(), 1000.f, 3000.f,
			[this, TrimWorld](float const InVal) { TrimWorld->SetTrimWorldSize(InVal); });
	}

	ImGui::Checkbox("Debug Rendering", &DebugRenderSteering);
	// TODO: Make space partitioning toggleable at runtime
	// ImGui::Checkbox("Use spatial partitioning", &bUseSpatialPartitioning);

	// Separation coefficient
	ImGuiHelpers::ImGuiSliderFloatWithSetter("Separation factor",
		SeparationBehavior->GetSeparationFactor(), 10.f, 250.f,
		[this](float const InVal) { SeparationBehavior->SetSeparationFactor(InVal); }, "%.2f");
	
	ImGui::Text("Behavior Weights");
	ImGui::Spacing();

	DrawBehaviorSliders();
		
	//End
	ImGui::End();
}

#ifdef GAMEAI_USE_SPACE_PARTITIONING
void FFlock::RegisterNeighbors(ASteeringAgent const& Agent)
{
	CellSpace->RegisterNeighbors(Agent, NeighborhoodRadius);
	NeighborCount = CellSpace->GetNeighborCount();
}
#endif

void FFlock::RenderNeighborhood(ASteeringAgent const& Agent)
{
	RegisterNeighbors(Agent);
	UE_LOG(LogTemp, Warning, TEXT("Neighbor count: %u"), NeighborCount);
	for (uint32_t NeighborIdx{}; NeighborIdx < NeighborCount; ++NeighborIdx)// Memory pool might not be full
	{
		auto const Neighbor{ GetNeighbors()[NeighborIdx] };
		DrawDebugCircle(
			Neighbor->GetWorld(),
			Neighbor->GetActorLocation(),
			50.f,
			32, FColor::Cyan, false, -1.f, 0, 5,
			FVector(0, 1, 0), FVector(1, 0, 0), false
		);
	}
}

void FFlock::DrawBehaviorSliders() const
{
	DrawBehaviorSlider("Separation", 0);
	DrawBehaviorSlider("VelMatch", 1);
	DrawBehaviorSlider("Cohesion", 2);
	DrawBehaviorSlider("Wander", 3);
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
void FFlock::RegisterNeighbors(ASteeringAgent const& Agent)
{
	NeighborCount = 0;
	// Filling the memory pool with the neighbors for the currently evaluated agent
	for (auto const OtherAgent : Agents)
	{
		if (&Agent == OtherAgent) continue;
		if( (OtherAgent->GetActorLocation() - Agent.GetActorLocation()).Length() < NeighborhoodRadius )
		{
			assert(NeighborCount < m_Neighbors.Num());
			FlockNeighborMemoryPool[NeighborCount++] = OtherAgent; 
		}
	}
}

#endif

std::optional<FVector2D> FFlock::GetAverageNeighborLocation() const
{
	if (NeighborCount == 0) return std::nullopt;
	auto const& Neighbors{ GetNeighbors() };
	FVector2D AverageLocation{};
	for (int32_t NeighborIdx{}; NeighborIdx < NeighborCount; ++NeighborIdx)
	{
		AverageLocation += Neighbors[NeighborIdx]->GetLocation();
	}
	if (NeighborCount > 0)
	{
		AverageLocation /= static_cast<float>(NeighborCount);
	}
	return AverageLocation;
}

FVector2D FFlock::GetAverageNeighborVelocity() const
{
	FVector2D AverageVelocity{};
	for (int32_t NeighborIdx{}; NeighborIdx < NeighborCount; ++NeighborIdx)
	{
		AverageVelocity += GetNeighbors()[NeighborIdx]->GetLocation();
	}
	AverageVelocity /= static_cast<float>(NeighborCount);
	return AverageVelocity;
}

void FFlock::SetTarget_Seek(FSteeringParams const& Target)
{
	SeekBehavior->SetTarget(Target);
}

