// Game
#include "SearchState.h"
#include "DecisionMaking/GameAIController.h"
#include "PatrolState.h"
// Engine
#include "BrainComponent.h"
// Standard
#include <ranges>

GameAI::FSM::FSearchState::FSearchState(AGuard& ControlledGuard)
	: Guard{ ControlledGuard }
{}

void GameAI::FSM::FSearchState::OnEnter()
{
	Guard.SetStateText("Search");
	Guard.SetSubstateText("Patrol");
	RemainingSeconds = MaxSeconds;
	
	// Searching is patrolling random points around the target's last location
	FSM.AddState(std::make_unique<FPatrolState>(
		Guard, GetRandomPointsAroundTarget(3)
	));
	FSM.Start();
}

void GameAI::FSM::FSearchState::OnExit()
{
	Guard.SetStateText("Patrol");
	Guard.SetSubstateText("");
	FSM.Reset();
}

void GameAI::FSM::FSearchState::Tick(float const DeltaTime)
{
	FSM.Tick(DeltaTime);

	// Updating the time
	RemainingSeconds -= DeltaTime;
	if (RemainingSeconds < 0.f)
	{
		OnTimerOut.Broadcast();
	}
}

TArray<FVector> GameAI::FSM::FSearchState::GetRandomPointsAroundTarget(int32_t const Count) const
{
	// 1. Getting the blackboard
	auto const* const BlackboardComponent{
		Cast<AGameAIController>(Guard.GetController())->GetBrainComponent()->GetBlackboardComponent()
	};
	ensureAlways(BlackboardComponent);
	
	// 2. Getting the player location
	FVector const PlayerLocation{ BlackboardComponent->GetValueAsVector(Guard.TargetLocationKeyName) };
	
	// 3. Spawning the points around
	TArray<FVector> Points;
	Points.Reserve(Count);
	std::ranges::for_each(std::views::iota(0, Count), [&](int32_t)
	{
		Points.Emplace(GetRandomPointAround(PlayerLocation));
	});

	return Points;
}

FVector GameAI::FSM::FSearchState::GetRandomPointAround(FVector const& Origin)
{
	float constexpr MinCentimeters{ 100.f }, MaxCentimeters{ 500.f };
	float constexpr MinRadians{ 0.f }, MaxRadians{ 360.f };
	float const RandomCentimeters{ FMath::RandRange(MinCentimeters, MaxCentimeters) };
	float const RandomRadians{ FMath::RandRange(MinRadians, MaxRadians) };

	FVector const Offset{
		std::cos(RandomRadians) * RandomCentimeters,
		std::sin(RandomRadians) * RandomCentimeters,
		0.f
	};

	return Origin + Offset;
}
