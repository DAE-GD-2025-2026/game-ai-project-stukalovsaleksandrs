#ifndef CHASE_STATE
#define CHASE_STATE

#include "BehaviorTree/BlackboardComponent.h"
#include "DecisionMaking/Guard.h"
#include "DecisionMaking/FSM/FSM.h"

namespace GameAI::FSM
{
	class FChaseState : public IState
	{
	public:
		explicit FChaseState(AGuard& ControlledGuard)
			: ControlledGuard{ ControlledGuard }
			, BlackboardComponent{ 
				Cast<AGameAIController>(ControlledGuard.GetController())->GetBrainComponent()->GetBlackboardComponent()
			}
		{}
		
		virtual void OnEnter() override {}
		
		virtual void OnExit() override {}
		
		virtual void Tick(float DeltaTime) override
		{
			FVector const TargetLocation{ GetTargetLocation() };
			
			ControlledGuard.SetTargetLocation(TargetLocation);
		}

	private:
		AGuard& ControlledGuard;
		UBlackboardComponent* BlackboardComponent{};
		
		[[nodiscard]] FVector GetTargetLocation() const
		{
			return BlackboardComponent->GetValueAsVector(ControlledGuard.TargetLocationKeyName);
		}
	};	
}

#endif
