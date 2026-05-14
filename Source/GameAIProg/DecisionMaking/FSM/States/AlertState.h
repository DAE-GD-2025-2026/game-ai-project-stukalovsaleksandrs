#pragma once

// Game
#include "DecisionMaking/Guard.h"
#include "DecisionMaking/FSM/FSM.h"

namespace GameAI::FSM
{
	class FAlertState final : public IState
	{
	public:
		explicit FAlertState(AGuard& ControlledGuard);
		virtual void OnEnter() override;
		virtual void OnExit() override;
		virtual void Tick(float DeltaTime) override;

	private:
		AGuard& Guard;
		FSM FSM;
		UBlackboardComponent* GuardBlackboardComponent{};

		[[nodiscard]] bool IsWithinAttackReach() const;
	};
}
