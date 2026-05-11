#pragma once

#include "FSM.h"

namespace GameAI::FSM
{
	class FMoveForwardState final : public IState
	{
	public:
		explicit FMoveForwardState(APawn& ControlledPawn)
			: ControlledPawn{ ControlledPawn }{}
		
		virtual void OnEnter() override{}
		
		virtual void OnExit() override{}
		
		virtual void Update(float DeltaTime) override
		{
			ControlledPawn.AddMovementInput(ControlledPawn.GetActorForwardVector(), 1.f);
		}

	private:
		APawn& ControlledPawn;
		
	};

}
