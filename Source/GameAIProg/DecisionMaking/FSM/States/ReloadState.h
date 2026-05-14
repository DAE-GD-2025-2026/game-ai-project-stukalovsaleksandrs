#pragma once
#include "DecisionMaking/Guard.h"
#include "DecisionMaking/FSM/FSM.h"

namespace GameAI::FSM
{
	class FReloadState final : public IState
	{
	public:
		DECLARE_EVENT(FSearchState, FEvent);
		FEvent DoneReloading;
		
		explicit FReloadState(AGuard& ControlledGuard);
		virtual void OnEnter() override;
		virtual void OnExit() override;
		virtual void Tick(float DeltaTime) override;

	private:
		float const ReloadSec{ 5.f };
		float CurrentReloadSec{};
		AGuard& Guard;
		UBlackboardComponent* BlackboardComponent{};
		
	};
}
