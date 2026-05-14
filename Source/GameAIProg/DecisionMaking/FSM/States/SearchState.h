#pragma once

#include "FSM.h"
#include "DecisionMaking/GameAIController.h"
#include "DecisionMaking/Guard.h"
#include "DecisionMaking/FSM/FSM.h"
#include <ranges>

namespace GameAI::FSM
{
	class FSearchState final : public IState
	{
	public:
		explicit FSearchState(AGuard& ControlledAgent)
			: ControlledAgent{ ControlledAgent }
		{}

		virtual void OnEnter() override
		{
			// 1. Getting the player location
			// 2. Scattering points around this location
			// 3. Patroling these points
			// 4. Getting the fuck out

			// 1. Getting the blackboard
			auto const* const BlackboardComponent{
				Cast<AGameAIController>(ControlledGuard.GetController())->GetBrainComponent()->GetBlackboardComponent()
			};
			ensureAlways(BlackboardComponent);
			// 2. Getting the player location
			FVector const PlayerLocation{ BlackboardComponent->GetValueAsVector(ControlledGuard.TargetLocationKeyName) };
			// 3. Spawning the points around
			SpawnRandomPointsAround(PlayerLocation, 3);
		}
		
		virtual void OnExit() override{}
		
		virtual void Update(float DeltaTime) override
		{
			
		}

	private:
		AAgent& ControlledAgent;

		TObjectPtr<UBlackboardComponent> BlackboardComponent{};

		std::vector<FVector> SearchPoints{};


		void SpawnRandomPointsAround(FVector const Origin, int32_t const Count)
		{
			SearchPoints.reserve(Count);
			std::ranges::for_each(std::views::iota(0, Count), [](int32_t)
			{
				SpawnRandomPointsAround(Origin);
			});
		}

		void SpawnRandPointAround(FVector const Origin)
		{
			float constexpr MinCentimeters{ 100.f }, MaxCentimeters{ 1000.f };
			float constexpr MinRadians{ 0.f }, MaxRadians{ 360.f };
			float const RandomCentimeters{ FMath::RandRange(MinCentimeters, MaxCentimeters) };
			float const RandomRadians{ FMath::RandRange(MinRadians, MaxRadians) };

			FVector const Offset{
				std::cos(RandomRadians) * RandomCentimeters,
				std::sin(RandomRadians) * RandomCentimeters,
				0.f
			};

			SearchPoints.emplace_back(Origin + Offset);
		}

	};

}
