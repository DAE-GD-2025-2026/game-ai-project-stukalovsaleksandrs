#ifndef CHASE_STATE
#define CHASE_STATE

#include "BehaviorTree/BlackboardComponent.h"
#include "DecisionMaking/Guard.h"
#include "DecisionMaking/FSM/FSM.h"
#include "DecisionMaking/GameAIController.h"

namespace GameAI::FSM
{
	class FChaseState : public IState
	{
	public:
		explicit FChaseState(AGuard& ControlledGuard)
			: Guard{ ControlledGuard }
			, BlackboardComponent{ 
				Cast<AGameAIController>(ControlledGuard.GetController())->GetBrainComponent()->GetBlackboardComponent()
			}
		{}
		
		virtual void OnEnter() override
		{
			Guard.SetSubstateText("Chase");
		}
		
		virtual void OnExit() override
		{
			Guard.SetSubstateText("Chase");
		}
		
		virtual void Tick(float DeltaTime) override
		{
			FVector const TargetLocation{ GetTargetLocation() };
			
			Guard.SetTargetLocation(TargetLocation);
		}

	private:
		AGuard& Guard;
		UBlackboardComponent* BlackboardComponent{};
		
		[[nodiscard]] FVector GetTargetLocation() const
		{
			return BlackboardComponent->GetValueAsVector(Guard.TargetLocationKeyName);
		}
	};	
}

#endif
