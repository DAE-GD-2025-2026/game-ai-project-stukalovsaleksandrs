#pragma once

// Game
#include "DecisionMaking/Guard.h"
#include "DecisionMaking/FSM/FSM.h"

namespace GameAI::FSM
{
	class FSearchState final : public IState
	{
	public:
		DECLARE_EVENT(FSearchState, FEvent);
		FEvent OnTimerOut;
		
		explicit FSearchState(AGuard& ControlledGuard);

		virtual void OnEnter() override;

		virtual void OnExit() override;

		virtual void Tick(float DeltaTime) override;

	private:
		AGuard& ControlledGuard;

		FSM FSM;

		float const MaxSeconds{ 10.f };
		float RemainingSeconds{};

		[[nodiscard]] TArray<FVector> GetRandomPointsAroundTarget(int32_t const Count) const;
		[[nodiscard]] static FVector GetRandomPointAround(FVector const& Origin);
	};

}
