#pragma once

#include "FSM.h"

namespace GameAI::FSM
{
	class FPatrolState final : public IState
	{
	public:
		explicit FPatrolState(AAgent& ControlledAgent, TArray<FVector> PatrolPoints)
			: ControlledAgent{ ControlledAgent }
			, PatrolPoints{ PatrolPoints }
		{}

		virtual void SetBlackboard(TObjectPtr<UBlackboardComponent> const Blackboard) override
		{
			BlackboardComponent = Blackboard;
		}

		virtual void OnEnter() override
		{
			ControlledAgent.SetTargetLocation( PatrolPoints[CurrentPatrolPointIdx] );
		}
		
		virtual void OnExit() override{}
		
		virtual void Update(float DeltaTime) override
		{
			if (PatrolPoints.IsEmpty()) return;
			FVector const ToCurrentPoint = PatrolPoints[CurrentPatrolPointIdx] - ControlledAgent.GetActorLocation();
			// TODO: See if it'll work without squaring the right one
			// NOTE: Not considering the height
			if (FVector2D(ToCurrentPoint.X, ToCurrentPoint.Y).SizeSquared() < KINDA_SMALL_NUMBER)
			{
				// TODO: Stay idle for a few seconds
				// Changing the point
				CurrentPatrolPointIdx = (CurrentPatrolPointIdx + 1) % PatrolPoints.Num();

				ControlledAgent.SetTargetLocation( PatrolPoints[CurrentPatrolPointIdx] );
			}

			// Debug rendering patrol points
#ifdef ENABLE_DEBUGGING
			for (auto PatrolPoint: PatrolPoints)
			{
				DrawDebugSphere(ControlledAgent.GetWorld(), PatrolPoint, 100, 10, FColor::Red, false, -1, 0, 8);
			}
#endif// ENABLE_DEBUGGING
		}

	private:
		AAgent& ControlledAgent;
		TArray<FVector> PatrolPoints;
		int32_t CurrentPatrolPointIdx{};

		TObjectPtr<UBlackboardComponent> BlackboardComponent{};
		
	};

}
