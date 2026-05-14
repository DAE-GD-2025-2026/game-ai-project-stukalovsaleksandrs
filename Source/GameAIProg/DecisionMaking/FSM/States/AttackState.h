#pragma once
#include "DecisionMaking/Guard.h"
#include "DecisionMaking/FSM/FSM.h"

namespace GameAI::FSM
{
	class FAttackState final : public IState
	{
	public:
		DECLARE_EVENT(FSearchState, FEvent);
		FEvent DoneAttacking;
		
		explicit FAttackState(AGuard& ControlledGuard);
		virtual void OnEnter() override;
		virtual void OnExit() override;
		virtual void Tick(float DeltaTime) override;

	private:
		AGuard& Guard;

		float const AttackSec{ 1.f };
		float CurrentAttackSec{};
	};
}
