#ifndef CHASE_STATE
#define CHASE_STATE

#include "DecisionMaking/FSM/FSM.h"

namespace GameAI::FSM
{
	class FChaseState : public IState
	{
	public:
		explicit FChaseState(AAgent& ControlledAgent)
			: ControlledAgent{ ControlledAgent }
		{}
		
		virtual void SetBlackboard(TObjectPtr<UBlackboardComponent> const Blackboard) override
		{
			BlackboardComponent = Blackboard;
		}

		virtual void OnEnter() override
		{
			// 
		}
		
		virtual void OnExit() override{}
		
		virtual void Update(float DeltaTime) override;

	private:
		UBlackboardComponent* BlackboardComponent{};
		AAgent& ControlledAgent;
		
	};	
}

#endif
