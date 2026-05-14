#pragma once

#define ENABLE_DEBUGGING

// Game
#include "DecisionMaking/Guard.h"
#include "DecisionMaking/FSM/FSM.h"
#include "DecisionMaking/FSM/Constants.h"
#include "DecisionMaking/GameAIController.h"
// Engine
#include "BrainComponent.h"

namespace GameAI::FSM
{
	class FPatrolState final : public IState
	{
	public:
		explicit FPatrolState(AGuard& ControlledGuard, TArray<FVector> PatrolPoints)
			: Guard{ ControlledGuard }
			, PatrolPoints{ PatrolPoints }
			, BlackboardComponent{
				Cast<AGameAIController>(ControlledGuard.GetController())->GetBrainComponent()->GetBlackboardComponent()
			}
		{}

		virtual void OnEnter() override
		{
			Guard.SetTargetLocation( PatrolPoints[CurrentPatrolPointIdx] );
		}
		
		virtual void OnExit() override{}
		
		virtual void Tick(float DeltaTime) override
		{
			if (PatrolPoints.IsEmpty()) return;
			FVector const ToCurrentPoint = PatrolPoints[CurrentPatrolPointIdx] - Guard.GetActorLocation();
			// TODO: See if it'll work without squaring the right one
			// NOTE: Not considering the height
			if (FVector2D(ToCurrentPoint.X, ToCurrentPoint.Y).SizeSquared() < Epsilon)
			{
				// TODO: Stay idle for a few seconds
				// Changing the point
				CurrentPatrolPointIdx = (CurrentPatrolPointIdx + 1) % PatrolPoints.Num();

				Guard.SetTargetLocation( PatrolPoints[CurrentPatrolPointIdx] );
			}

			// Debug rendering patrol points
#ifdef ENABLE_DEBUGGING
			for (auto PatrolPoint: PatrolPoints)
			{
				DrawDebugSphere(Guard.GetWorld(), PatrolPoint, 100, 10, FColor::Red, false, -1, 0, 8);
			}
#endif// ENABLE_DEBUGGING
		}

	private:
		AGuard& Guard;
		TArray<FVector> PatrolPoints;
		int32_t CurrentPatrolPointIdx{};

		TObjectPtr<UBlackboardComponent> BlackboardComponent{};
		
	};

}
